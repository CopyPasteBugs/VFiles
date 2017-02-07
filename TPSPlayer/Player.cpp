#include "StdAfx.h"
#include "Player.h"
#include "GamePlugin.h"

#include <CryAnimation\ICryAnimation.h>


#include "View.h"
#include "Attack.h"
#include "Marker.h"
#include "Input.h"

class CPlayerRegistrator
	: public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CPlayer>("Player", "Defaults", "bird.bmp");
		
		//*** We don't needed expose "extensions" to editor's entities list
		//RegisterEntityWithDefaultComponent<CView>("View");
		//RegisterEntityWithDefaultComponent<CAttack>("Attack");
		//RegisterEntityWithDefaultComponent<CMarker>("Marker");
	}
	virtual void Unregister() override
	{

	}
public:
	CPlayerRegistrator() {}
	~CPlayerRegistrator()
	{
	}
};

static CPlayerRegistrator g_playerRegistrator;

CRYREGISTER_CLASS(CPlayer)

void CPlayer::Initialize()
{
	////GetEntity()->Activate(false);
	//LoadModel();

	////// Add Components
	//pMarker = GetEntity()->GetOrCreateComponent<CMarker>();
	//pAttack = GetEntity()->GetOrCreateComponent<CAttack>();
	//pInput = GetEntity()->GetOrCreateComponent<CInput>();
	//pView = GetEntity()->GetOrCreateComponent<CView>();

	OnResetState();

}

void CPlayer::ProcessEvent(SEntityEvent & event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_RESET:
	{
		switch (event.nParam[0])
		{
		case 0: // Game ends
			GetEntity()->Activate(false);
			//LoadModel();
			break;
		case 1: // Game starts
			GetEntity()->Activate(true);
			OnResetState();
			break;
		default:
			break;
		}
	}
	case ENTITY_EVENT_UPDATE:
	{
			//SEntityUpdateContext* param = (SEntityUpdateContext*)event.nParam[0];

			float dt = gEnv->pTimer->GetFrameTime();

			UpdateMovement(dt);
			// Update if needed all child component from host Entity's component
			//if (pInput) pInput->Update(param);
			//if (pAttack) pAttack->Update(param);
			//if (pMarker) pMarker->Update(param);

			if (pView) pView->Update(dt);
			
			break;
	}
	case ENTITY_EVENT_START_LEVEL:
	{
		if (!gEnv->IsEditor())
		{
			GetEntity()->Activate(true);
			OnResetState();			
		}
		break;
	}
	default:
		break;
	}
}

void CPlayer::SerializeProperties(Serialization::IArchive & archive)
{
	if (archive.isInput())
	{
		OnResetState();
	}
}

void CPlayer::OnShutDown()
{
	if (pView) pView->OnShutDown();
	if (pInput) pInput->OnShutDown();
	if (pAttack) pAttack->OnShutDown();
	if (pMarker) pMarker->OnShutDown();

	if (pInput) GetEntity()->RemoveComponent(pInput);
	if (pView) GetEntity()->RemoveComponent(pView);
	if (pMarker) GetEntity()->RemoveComponent(pMarker);
	if (pAttack) GetEntity()->RemoveComponent(pAttack);
	pMarker = nullptr;
	pView = nullptr;
	pAttack = nullptr;
	pInput = nullptr;
}

void CPlayer::OnResetState()
{
	LoadModel();
	Physicalize();

	GetEntity()->SetFlags(ENTITY_FLAG_CASTSHADOW | ENTITY_FLAG_CLIENT_ONLY | ENTITY_FLAG_TRIGGER_AREAS);

	pInput = GetEntity()->GetOrCreateComponent<CInput>();
	if (pInput) pInput->OnResetState();

	pView = GetEntity()->GetOrCreateComponent<CView>();
	pMarker = GetEntity()->GetOrCreateComponent<CMarker>();
	pAttack = GetEntity()->GetOrCreateComponent<CAttack>();


}

void CPlayer::Physicalize()
{
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_LIVING;
	physParams.nSlot = 0;
	physParams.mass = 30.0f;

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
	//playerDynamics.collTypes = ent_all | ent_areas | ent_triggers;
	physParams.pPlayerDynamics = &playerDynamics;

	GetEntity()->Physicalize(physParams);

}

void CPlayer::LoadModel()
{
	GetEntity()->LoadGeometry(0, "Assets/Objects/Default/primitive_sphere_small.cgf");
}

void CPlayer::Update(float dt)
{
	UpdateMovement(dt);
}

void CPlayer::UpdateMovement(float dt)
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
			UpdateMovementRequest(dt, *pPhysicalEntity);
		}
	}
	// Movement	
}

void CPlayer::GetLatestPhysicsStats(IPhysicalEntity & physicalEntity)
{
	pe_status_living livingStatus;
	if (physicalEntity.GetStatus(&livingStatus) != 0)
	{
		bIsOnGround = !livingStatus.bFlying;

		// Store the ground normal in case it is needed
		// Note that users have to check if we're on ground before using, is considered invalid in air.
		groundNormal = livingStatus.groundSlope;
	}
}

void CPlayer::UpdateMovementRequest(float frameTime, IPhysicalEntity & physicalEntity)
{
	pe_action_move moveAction;

	// Apply movement request directly to velocity
	moveAction.iJump = 2;

	const float moveSpeed = fSpeed;
	//moveAction.dir = GetEntity()->GetWorldRotation() * GetLocalMoveDirection() * moveSpeed * frameTime;

	if (!pView | !pInput) return;

	// Now, we convert local movement (pressing on the keys(left, right...)) into View plane movement
	moveAction.dir = pView->GetCameraRootRotation() * pInput->GetLocalMoveDirection() * moveSpeed * frameTime;

	// Dispatch the movement request
	physicalEntity.Action(&moveAction);

	if (pInput->GetJumpState())
	{
		pe_action_move moveAction;
		moveAction.iJump = 2;
		moveAction.dir = Vec3(0.0f, 0.0f, 1.0f) * 600.0f * frameTime;
		physicalEntity.Action(&moveAction);

		pInput->ClearJumpState();
	}
}


