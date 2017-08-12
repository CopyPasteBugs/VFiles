#include "StdAfx.h"
#include "PlayerTank.h"
#include "PlayerTank\PTView.h"
#include "PlayerTank\PTFiring.h"


#include "GamePlugin.h"

#include "Entities\Helpers\ISimpleExtension.h"

#include <Cry3DEngine/I3DEngine.h>
#include <CryParticleSystem/IParticlesPfx2.h>
#include <CryAnimation/ICryAnimation.h>

#include "Enemy\Enemy.h"

class CPlayerTankRegistrator
	: public IEntityRegistrator
{
	virtual void Register() override
	{
		CGamePlugin::RegisterEntityWithDefaultComponent<CPlayerTank>("PlayerTank");
		CGamePlugin::RegisterEntityComponent<CTankView>("TankView");
		CGamePlugin::RegisterEntityComponent<CTankFiring>("TankFiring");

	}
	virtual void Unregister() override
	{
		
	}

public:
	CPlayerTankRegistrator() {}
	~CPlayerTankRegistrator()
	{

	}
};

static CPlayerTankRegistrator g_playerTankRegistrator;

CPlayerTank::CPlayerTank()
{
	m_bOnGround = false;
	m_inputFlags = 0;
	m_groundNormal = ZERO;
}

CPlayerTank::~CPlayerTank()
{
	GetGameObject()->ReleaseActions(this);
	//GetGameObject()->ReleaseView(this);
}

bool CPlayerTank::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);

	return true;
}

void CPlayerTank::PostInit(IGameObject *pGameObject)
{


	//Action Init
	{
		// NOTE: Since CRYENGINE 5.3, the game is responsible to initialize the action maps
		IActionMapManager *pActionMapManager = gEnv->pGameFramework->GetIActionMapManager();
		pActionMapManager->InitActionMaps("Libs/config/defaultprofile.xml");
		pActionMapManager->Enable(true);
		pActionMapManager->EnableActionMap("player", true);

		if (IActionMap *pActionMap = pActionMapManager->GetActionMap("player"))
		{
			pActionMap->SetActionListener(GetEntityId());
		}

		GetGameObject()->CaptureActions(this);

		m_mouseDeltaRotation = ZERO;

		// Populate the action handler callbacks so that we get action map events
		InitializeActionHandler();
	}

	// Movement Init 
	{
		pGameObject->EnableUpdateSlot(this, 0);
	}

	// View Init 
	{
		//pGameObject->CaptureView(this);
		pView = static_cast<CTankView *>(GetGameObject()->AcquireExtension("TankView"));
	}

	// Firing Init
	{
		pFire = static_cast<CTankFiring *>(GetGameObject()->AcquireExtension("TankFiring"));
	}

	// Shake child of root
	SEntitySpawnParams spawnParamsParticleHelper;
	spawnParamsParticleHelper.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParamsParticleHelper.vPosition = Vec3(0,0,0);
	spawnParamsParticleHelper.vScale = Vec3(1, 1, 1);
	spawnParamsParticleHelper.qRotation = IDENTITY;

	pParticleHelper = gEnv->pEntitySystem->SpawnEntity(spawnParamsParticleHelper);
	CRY_ASSERT(pParticleHelper != nullptr);
	GetEntity()->AttachChild(pParticleHelper);

	const char* m_particleEffectPath = "particles/123.pfx";

	if (IParticleEffect* pEffect = gEnv->pParticleManager->FindEffect(m_particleEffectPath, "123"))
	{
		 pParticleHelper->LoadParticleEmitter(0, pEffect);
	}

	//// Check if we have to unload first
	//if (m_particleSlot != -1)
	//{
	//	entity.FreeSlot(m_particleSlot);
	//	m_particleSlot = -1;
	//}

	//// Check if the light is active
	//if (!m_bActive)
	//	return;

	//if (IParticleEffect* pEffect = gEnv->pParticleManager->FindEffect(m_particleEffectPath, "ParticleEntity"))
	//{
	//	m_particleSlot = entity.LoadParticleEmitter(-1, pEffect);
	//}


	OnResetState();
}

void CPlayerTank::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_RESET:
	{
		switch (event.nParam[0])
		{
		case 0: // Game ends
			break;
		case 1: // Game starts
			OnResetState();
			break;
		default:
			break;
		}
	}
	case ENTITY_EVENT_UPDATE:
	{
		SEntityUpdateContext* param = (SEntityUpdateContext*)event.nParam[0];

		break;
	}
	case ENTITY_EVENT_START_LEVEL:
	{
		if (!gEnv->IsEditor())
		{
			OnResetState();
		}
		break;
	}
	default:
		break;
	}
}

void CPlayerTank::OnAction(const ActionId & action, int activationMode, float value)
{
	m_actionHandler.Dispatch(this, GetEntityId(), action, activationMode, value);
}

Vec3 CPlayerTank::GetLocalMoveDirection() const
{
	Vec3 moveDirection = ZERO;

	uint32 inputFlags = GetInputFlags();

	if (inputFlags & eInputFlag_MoveLeft)
	{
		moveDirection.x -= 1;
	}
	if (inputFlags & eInputFlag_MoveRight)
	{
		moveDirection.x += 1;
	}
	if (inputFlags & eInputFlag_MoveForward)
	{
		moveDirection.y += 1;
	}
	if (inputFlags & eInputFlag_MoveBack)
	{
		moveDirection.y -= 1;
	}

	return moveDirection;
}

void CPlayerTank::InitializeActionHandler()
{
	m_actionHandler.AddHandler(ActionId("moveleft"), &CPlayerTank::OnActionMoveLeft);
	m_actionHandler.AddHandler(ActionId("moveright"), &CPlayerTank::OnActionMoveRight);
	m_actionHandler.AddHandler(ActionId("moveforward"), &CPlayerTank::OnActionMoveForward);
	m_actionHandler.AddHandler(ActionId("moveback"), &CPlayerTank::OnActionMoveBack);

	m_actionHandler.AddHandler(ActionId("mouse_rotateyaw"), &CPlayerTank::OnActionMouseRotateYaw);
	m_actionHandler.AddHandler(ActionId("mouse_rotatepitch"), &CPlayerTank::OnActionMouseRotatePitch);

	m_actionHandler.AddHandler(ActionId("wheelup"), &CPlayerTank::OnActionMouseWheelUp);
	m_actionHandler.AddHandler(ActionId("wheeldown"), &CPlayerTank::OnActionMouseWheelDown);

	m_actionHandler.AddHandler(ActionId("shoot"), &CPlayerTank::OnActionShoot);
	m_actionHandler.AddHandler(ActionId("jump"), &CPlayerTank::OnActionJump);
	m_actionHandler.AddHandler(ActionId("findway"), &CPlayerTank::OnActionFindWay);
	m_actionHandler.AddHandler(ActionId("exit"), &CPlayerTank::OnExit);
	m_actionHandler.AddHandler(ActionId("use"), &CPlayerTank::OnUse);



}

void CPlayerTank::HandleInputFlagChange(EInputFlags flags, int activationMode, EInputFlagType type)
{
	switch (type)
	{
	case eInputFlagType_Hold:
	{
		if (activationMode == eIS_Released)
		{
			m_inputFlags &= ~flags;
		}
		else
		{
			m_inputFlags |= flags;
		}
	}
	break;
	case eInputFlagType_Toggle:
	{
		if (activationMode == eIS_Released)
		{
			// Toggle the bit(s)
			m_inputFlags ^= flags;
		}
	}
	break;
	}
}

void CPlayerTank::OnResetState()
{
	bPlayerWantsUse = false;
	m_inputFlags = 0;
	m_mouseDeltaRotation = ZERO;
	m_mouseDeltaWheel = 0.0f;
	m_lookOrientation = IDENTITY;

	GetEntity()->LoadGeometry(0, "Assets/Objects/Default/primitive_sphere_small.cgf");
	GetEntity()->SetFlags(EEntityFlags::ENTITY_FLAG_CASTSHADOW);

	SEntityPhysicalizeParams physParams;
	physParams.type = PE_LIVING;
	physParams.nSlot = -1;
	physParams.mass = 90.0f;

	pe_player_dimensions playerDimensions;
	// Prefer usage of a cylinder instead of capsule
	playerDimensions.bUseCapsule = 0;
	// Specify the size of our cylinder
	playerDimensions.sizeCollider = Vec3(0.3f, 0.3f, 0.2f);
	// Keep pivot at the player's feet (defined in player geometry) 
	playerDimensions.heightPivot = -0.1f;
	// Offset collider upwards
	playerDimensions.heightCollider = 0.0f;
	playerDimensions.groundContactEps = 0.04f;
	physParams.pPlayerDimensions = &playerDimensions;


	pe_player_dynamics playerDynamics;
	//playerDynamics.kAirControl = 0.5f;
	playerDynamics.kAirResistance = 1.0f;
	playerDynamics.mass = physParams.mass;
	physParams.pPlayerDynamics = &playerDynamics;

	GetEntity()->Physicalize(physParams);
}

bool CPlayerTank::OnUse(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	if (activationMode == eIS_Pressed)
		SetUseFlag(true);

	if (activationMode == eIS_Released)
		SetUseFlag(false);



	return true;
}

bool CPlayerTank::OnActionF(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{

	return true;
}

bool CPlayerTank::OnActionMoveLeft(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	HandleInputFlagChange(eInputFlag_MoveLeft, activationMode);
	return true;
}

bool CPlayerTank::OnActionMoveRight(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	HandleInputFlagChange(eInputFlag_MoveRight, activationMode);
	return true;
}

bool CPlayerTank::OnActionMoveForward(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	HandleInputFlagChange(eInputFlag_MoveForward, activationMode);
	return true;
}

bool CPlayerTank::OnActionMoveBack(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	HandleInputFlagChange(eInputFlag_MoveBack, activationMode);
	return true;
}

bool CPlayerTank::OnActionMouseRotateYaw(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	m_mouseDeltaRotation.x = -value;
	return true;
}

bool CPlayerTank::OnActionMouseRotatePitch(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	m_mouseDeltaRotation.y = -value;
	return true;
}

bool CPlayerTank::OnActionShoot(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	// Only fire on press, not release
	if (activationMode == eIS_Pressed)
	{
		//auto *pCharacter = GetEntity()->GetCharacter(0);
	 
		if (pFire != nullptr)
		{
			//auto *pBarrelOutAttachment = pCharacter->GetIAttachmentManager()->GetInterfaceByName("BulletSpawnPlace");
			//QuatTS bulletOrigin = pBarrelOutAttachment->GetAttWorldAbsolute();
			//IEntity * e = pFire->GetBulletSpawnEntity();
			Vec3 dir = pView->GetCameraRootForward();
			Vec3 startPos = GetEntity()->GetWorldPos() + dir;
			pFire->RequestFire(startPos, pView->GetCameraRootRotation());
		}
	}

	return true;
}

bool CPlayerTank::OnActionMouseWheelUp(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	m_mouseDeltaWheel = value;
	return true;
}

bool CPlayerTank::OnActionMouseWheelDown(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	m_mouseDeltaWheel = value;
	return true;
}

bool CPlayerTank::OnActionJump(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	if (activationMode == eIS_Pressed)
	{
		requestJump = true;
		pView->ShakeCamera(3.0f, 5.0f, 1.0f);
	}
	return true;
}

bool CPlayerTank::OnActionFindWay(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	auto *pEntityIterator = gEnv->pEntitySystem->GetEntityIterator();
	pEntityIterator->MoveFirst();

	auto *pSpawnerClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Enemy");
	//auto extensionId = gEnv->pGameFramework->GetIGameObjectSystem()->GetID("Dummy");

	while (!pEntityIterator->IsEnd())
	{
		IEntity *pEntity = pEntityIterator->Next();

		if (pEntity->GetClass() != pSpawnerClass)
			continue;

		auto* enemy = pEntity->GetComponent<CEnemy>();

		enemy->GoForAll();
		break;
	}

	return true;
}

bool CPlayerTank::OnExit(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	gEnv->pSystem->Quit();
	return true;
}

void CPlayerTank::GetLatestPhysicsStats(IPhysicalEntity & physicalEntity)
{
	pe_status_living livingStatus;
	if (physicalEntity.GetStatus(&livingStatus) != 0)
	{
		m_bOnGround = !livingStatus.bFlying;
		//m_bOnGround = !(livingStatus.timeFlying > 0.1f);

		// Store the ground normal in case it is needed
		// Note that users have to check if we're on ground before using, is considered invalid in air.
		m_groundNormal = livingStatus.groundSlope;
	}
}

void CPlayerTank::UpdateMovementRequest(float frameTime, IPhysicalEntity & physicalEntity)
{
	if (1)
	{
		pe_action_move moveAction;

		// Apply movement request directly to velocity
		moveAction.iJump = 2;

		const float moveSpeed = fSpeed;
		//moveAction.dir = GetEntity()->GetWorldRotation() * GetLocalMoveDirection() * moveSpeed * frameTime;
		
		// Now, we convert local movement (pressing on the keys(left, right...)) into View plane movement
		moveAction.dir = pView->GetCameraRootRotation() * GetLocalMoveDirection() * moveSpeed * frameTime;

		// Dispatch the movement request
		physicalEntity.Action(&moveAction);

		if (requestJump) 
		{
			pe_action_move moveAction;
			moveAction.iJump = 2;
			moveAction.dir = Vec3(0.0f,0.0f,1.0f) * 300.0f * frameTime;
			physicalEntity.Action(&moveAction);

			requestJump = false;
		}

	}
}

void CPlayerTank::Update(SEntityUpdateContext &ctx, int updateSlot)
{
	// Movement
	{
		IEntity &entity = *GetEntity();
		IPhysicalEntity *pPhysicalEntity = entity.GetPhysics();
		if (pPhysicalEntity != nullptr)
		{
			// Obtain stats from the living entity implementation
			GetLatestPhysicsStats(*pPhysicalEntity);

			// Send latest input data to physics indicating desired movement direction
			UpdateMovementRequest(ctx.fFrameTime, *pPhysicalEntity);
		}
	}
	// Movement

	//// View
	//{
	//	// Start by updating look dir
	//	Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));

	//	ypr.x += m_mouseDeltaRotation.x * fRotationSpeedYaw * ctx.fFrameTime;

	//	// TODO: Perform soft clamp here instead of hard wall, should reduce rot speed in this direction when close to limit.
	//	ypr.y = CLAMP(ypr.y + m_mouseDeltaRotation.y * fRotationSpeedPitch * ctx.fFrameTime, fRotationLimitsMinPitch, fRotationLimitsMaxPitch);

	//	ypr.z = 0;

	//	m_lookOrientation = Quat(CCamera::CreateOrientationYPR(ypr));

	//	// Reset every frame
	//	//m_mouseDeltaRotation = ZERO;
	//}
	//// View
}
