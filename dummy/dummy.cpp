#include "StdAfx.h"
#include "dummy.h"
#include "GamePlugin.h"

class CTestRegistrator
	: public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CDummyEntity>("Dummy", "UserStuff");
		//CGamePlugin::RegisterEntityComponent<CTest>("Test");
	}
	virtual void Unregister() override
	{

	}

public:
	CTestRegistrator() {}
	~CTestRegistrator()
	{

	}
};

static CTestRegistrator g_testRegistrator;

CRYREGISTER_CLASS(CDummyEntity)


void CDummyEntity::Initialize()
{
	Reset();
}

void CDummyEntity::ProcessEvent(SEntityEvent & event)
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
				Reset();
				InitAI();
				break;
			default:
				break;
			}
		}
		case ENTITY_EVENT_UPDATE:
		{
			SEntityUpdateContext* param = (SEntityUpdateContext*)event.nParam[0];
		}
		case ENTITY_EVENT_START_LEVEL:
		{
			if (!gEnv->IsEditor())
			{
				Reset();
				InitAI();
			}
		}
		default:
			break;
	}
}

void CDummyEntity::SerializeProperties(Serialization::IArchive & archive)
{
	
	if (archive.isInput())
	{ 
		//Reset();
		//InitAI();
	}
}

void CDummyEntity::Reset()
{
	GetEntity()->LoadGeometry(0, "Assets/Objects/Default/primitive_sphere_small.cgf");

	SEntityPhysicalizeParams physParams;
	physParams.type = PE_LIVING;
	physParams.nSlot = 0;
	physParams.mass = 10.0f;

	pe_player_dimensions playerDimensions;
	// Prefer usage of a cylinder instead of capsule
	playerDimensions.bUseCapsule = 0;
	// Specify the size of our cylinder
	playerDimensions.sizeCollider = Vec3(0.7f, 0.7f, 1.0f);
	// Keep pivot at the player's feet (defined in player geometry) 
	playerDimensions.heightPivot = 0.0f;
	// Offset collider upwards
	playerDimensions.heightCollider = 1.0f;
	playerDimensions.groundContactEps = 0.004f;
	physParams.pPlayerDimensions = &playerDimensions;


	pe_player_dynamics playerDynamics;
	playerDynamics.kAirControl = 0.0f;
	playerDynamics.kAirResistance = 1.0;
	playerDynamics.mass = physParams.mass;
	physParams.pPlayerDynamics = &playerDynamics;

	GetEntity()->Physicalize(physParams);
}

void CDummyEntity::ResetPF()
{
	gEnv->pAISystem->GetMovementSystem()->UnregisterEntity(GetEntityId());
	m_pPathFollower.reset();
	SAFE_RELEASE(m_pFoundPath);
}

void CDummyEntity::InitAI()
{
	ResetPF();

	m_navigationAgentTypeId = gEnv->pAISystem->GetNavigationSystem()->GetAgentTypeID("MediumSizedCharacters");

	m_callbacks.queuePathRequestFunction = functor(*this, &CDummyEntity::RequestPathTo);
	m_callbacks.checkOnPathfinderStateFunction = functor(*this, &CDummyEntity::GetPathfinderState);
	m_callbacks.getPathFollowerFunction = functor(*this, &CDummyEntity::GetPathFollower);
	m_callbacks.getPathFunction = functor(*this, &CDummyEntity::GetINavPath);

	gEnv->pAISystem->GetMovementSystem()->RegisterEntity(GetEntityId(), m_callbacks, *this);

	if (m_pPathFollower == nullptr)
	{
		PathFollowerParams params;
		params.maxAccel = 5.0f;
		params.maxSpeed = params.maxAccel;
		params.minSpeed = 0.f;
		params.normalSpeed = params.maxSpeed;

		params.use2D = false;

		m_pPathFollower = gEnv->pAISystem->CreateAndReturnNewDefaultPathFollower(params, m_pathObstacles);
	}

	m_movementAbility.b3DMove = true;
}

void CDummyEntity::RequestMoveTo(const Vec3 &position)
{
	CRY_ASSERT_MESSAGE(m_movementRequestId.id == 0, "RequestMoveTo can not be called while another request is being handled!");

	MovementRequest movementRequest;
	movementRequest.entityID = GetEntityId();
	movementRequest.destination = position;
	movementRequest.callback = functor(*this, &CDummyEntity::MovementRequestCallback);
	movementRequest.style.SetSpeed(MovementStyle::Walk);

	movementRequest.type = MovementRequest::Type::MoveTo;

	m_state = Movement::StillFinding;

	m_movementRequestId = gEnv->pAISystem->GetMovementSystem()->QueueRequest(movementRequest);
}

void CDummyEntity::RequestPathTo(MNMPathRequest &request)
{
	m_state = Movement::StillFinding;

	request.resultCallback = functor(*this, &CDummyEntity::OnMNMPathResult);
	request.agentTypeID = m_navigationAgentTypeId;

	m_pathFinderRequestId = gEnv->pAISystem->GetMNMPathfinder()->RequestPathTo(this, request);
}

void CDummyEntity::CancelCurrentRequest()
{
	CRY_ASSERT(m_movementRequestId.id != 0);

	gEnv->pAISystem->GetMovementSystem()->CancelRequest(m_movementRequestId);
	m_movementRequestId = 0;

	if (m_pathFinderRequestId != 0)
	{
		gEnv->pAISystem->GetMNMPathfinder()->CancelPathRequest(m_pathFinderRequestId);

		m_pathFinderRequestId = 0;
	}
}

void CDummyEntity::OnMNMPathResult(const MNM::QueuedPathID& requestId, MNMPathRequestResult& result)
{
	m_pathFinderRequestId = 0;

	if (result.HasPathBeenFound())
	{
		m_state = Movement::FoundPath;

		SAFE_DELETE(m_pFoundPath);
		m_pFoundPath = result.pPath->Clone();

		// Bump version
		m_pFoundPath->SetVersion(m_pFoundPath->GetVersion() + 1);

		m_pPathFollower->Reset();
		m_pPathFollower->AttachToPath(m_pFoundPath);
	}
	else
	{
		m_state = Movement::CouldNotFindPath;
	}
}


Vec3 CDummyEntity::GetVelocity() const
{
	return GetVelocity2();
}

void CDummyEntity::SetMovementOutputValue(const PathFollowResult& result)
{
	float frameTime = gEnv->pTimer->GetFrameTime();
	float moveStep = 5.0f * frameTime;

	if (result.velocityOut.GetLength() > moveStep)
	{
		RequestMove(result.velocityOut.GetNormalized() * moveStep);
	}
	else
	{
		SetVelocity(result.velocityOut);
	}
}

void CDummyEntity::ClearMovementState()
{
	SetVelocity(ZERO);
}


void CDummyEntity::Physicalize()
{
	// Physicalize the player as type Living.
	// This physical entity type is specifically implemented for players
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_LIVING;

	physParams.mass = 40.0f;

	pe_player_dimensions playerDimensions;

	// Prefer usage of a cylinder instead of capsule
	playerDimensions.bUseCapsule = 0;

	// Specify the size of our cylinder
	playerDimensions.sizeCollider = Vec3(0.45f, 0.45f, 0.9f * 0.5f);

	// Keep pivot at the player's feet (defined in player geometry) 
	playerDimensions.heightPivot = 0.f;
	// Offset collider upwards
	playerDimensions.heightCollider = 1.f;
	playerDimensions.groundContactEps = 0.004f;

	physParams.pPlayerDimensions = &playerDimensions;

	pe_player_dynamics playerDynamics;
	playerDynamics.kAirControl = 0.1f;
	playerDynamics.mass = physParams.mass;

	physParams.pPlayerDynamics = &playerDynamics;

	GetEntity()->Physicalize(physParams);
}

void CDummyEntity::RequestMove(const Vec3 &direction)
{
	if (auto *pPhysicalEntity = GetEntity()->GetPhysics())
	{
		pe_action_move moveAction;
		// Add dir to velocity
		moveAction.iJump = 2;
		moveAction.dir = direction;

		// Dispatch the movement request
		pPhysicalEntity->Action(&moveAction);
	}
}

void CDummyEntity::SetVelocity(const Vec3 &velocity)
{
	if (auto *pPhysicalEntity = GetEntity()->GetPhysics())
	{
		pe_action_move moveAction;
		// Change velocity instantaneously
		moveAction.iJump = 1;

		moveAction.dir = velocity;

		// Dispatch the movement request
		pPhysicalEntity->Action(&moveAction);
	}
}

Vec3 CDummyEntity::GetVelocity2() const
{
	if (auto *pPhysicalEntity = GetEntity()->GetPhysics())
	{
		pe_status_living status;
		if (pPhysicalEntity->GetStatus(&status))
			return status.vel;
	}

	return ZERO;
}

void CDummyEntity::Update(SEntityUpdateContext &ctx, int updateSlot)
{
	
	IEntity &entity = *GetEntity();
	IPhysicalEntity *pPhysicalEntity = entity.GetPhysics();
	if (pPhysicalEntity == nullptr)
		return;

	// Obtain stats from the living entity implementation
	GetLatestPhysicsStats(*pPhysicalEntity);
}

void CDummyEntity::GetLatestPhysicsStats(IPhysicalEntity &physicalEntity)
{
	pe_status_living livingStatus;
	if (physicalEntity.GetStatus(&livingStatus) != 0)
	{
		m_bOnGround = !livingStatus.bFlying;

		// Store the ground normal in case it is needed
		// Note that users have to check if we're on ground before using, is considered invalid in air.
		m_groundNormal = livingStatus.groundSlope;
	}
}

Vec3 CDummyEntity::RaycastRandom()
{
	int hit = 0;

	const float fTerrainSize = static_cast<float>(gEnv->p3DEngine->GetTerrainSize());
	const float fTerrainElevation = gEnv->p3DEngine->GetTerrainElevation(fTerrainSize * 0.5f, fTerrainSize * 0.5f);
	int hs = (int)(fTerrainSize * 0.5f);
	int rx = -16 + rand() % 32;
	int ry = -16 + rand() % 32;


	const Vec3 vSpawnLocation(hs + (float)(rx), hs + (float)(ry), fTerrainElevation + 60.0f);


	Vec3 origin = vSpawnLocation;
	Vec3 dir = Vec3(0, 0, -1);
	ray_hit rayHit;

	const int physSkipNum = 1;
	IPhysicalEntity* physSkip[physSkipNum];
	physSkip[0] = GetEntity()->GetPhysics();

	unsigned int flags = rwi_stop_at_pierceable | rwi_colltype_any | rwi_ignore_back_faces;

	float RayMaxDistance = 100.0f;

	hit = gEnv->pPhysicalWorld->RayWorldIntersection(origin, dir*RayMaxDistance, entity_query_flags::ent_all, flags, &rayHit, 1, physSkip, physSkipNum);
	if (hit)
	{
		IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
		debug->Begin("EnemyTargetPosition", false);

		Vec3 lineStart = origin;
		Vec3 lineEnd = rayHit.pt;
		float timeOut = 10.0f;

		debug->AddLine(lineStart, lineEnd, ColorF(1.0f, 1.0f, 0.0f), timeOut);
	}

	return rayHit.pt;
}


void CDummyEntity::Walk()
{
	//if (gEnv->IsEditor())
	//	return;

	//Vec3 characterPos = GetEntity()->GetWorldPos();

	//float distance = (characterPos - targetPos).len();
	Vec3 targetPos = RaycastRandom() + Vec3(0, 0, 0.2f);

	if (IsProcessingRequest())
		CancelCurrentRequest();

	RequestMoveTo(targetPos);

	//if (isCharacterMoving)
	//pPathfinding->CancelCurrentRequest();

	//// only if player do not near
	//if (distance > 3.0f)
	//{
	//	pPathfinding->CancelCurrentRequest();
	//	pPathfinding->RequestMoveTo(targetPos);
	//	walkToTarget = true;
	//}
	//else
	//{
	//	pPathfinding->CancelCurrentRequest();
	//	walkToTarget = false;
	//	//m_pPath->ClearMovementState();
	//}
}

void CDummyEntity::GoForAll()
{
	// We only handle default spawning below for the Launcher
	// Editor has special logic in CEditorGame

	//if (gEnv->IsEditor())
	//	return;

	auto *pEntityIterator = gEnv->pEntitySystem->GetEntityIterator();
	pEntityIterator->MoveFirst();

	auto *pSpawnerClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Dummy");
	//auto extensionId = gEnv->pGameFramework->GetIGameObjectSystem()->GetID("Dummy");

	while (!pEntityIterator->IsEnd())
	{
		IEntity *pEntity = pEntityIterator->Next();

		if (pEntity->GetClass() != pSpawnerClass)
			continue;

		auto* dummy = pEntity->GetComponent<CDummyEntity>();

		dummy->Walk();

	}
}



