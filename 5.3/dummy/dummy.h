#pragma once
#include "StdAfx.h"

#include "GamePlugin.h"

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <CryAISystem/IAISystem.h>
#include <CryAISystem/IMovementSystem.h>
#include <CryAISystem/MovementRequest.h>
#include <IActorSystem.h>

//#include "Player/ISimpleActor.h"

class CDummyEntity
	: public IEntityComponent
	, public IEntityPropertyGroup
	, public IMovementActorAdapter
	, public IAIPathAgent
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CDummyEntity, "Dummy", 0xBA8B086AA7BF486C, 0xAD0FEFAD7E84F591);

	class CPathObstacles
		: public IPathObstacles
	{
		// IPathObstacles
		virtual bool IsPathIntersectingObstacles(const NavigationMeshID meshID, const Vec3& start, const Vec3& end, float radius) const { return false; }
		virtual bool IsPointInsideObstacles(const Vec3& position) const { return false; }
		virtual bool IsLineSegmentIntersectingObstaclesOrCloseToThem(const Lineseg& linesegToTest, float maxDistanceToConsiderClose) const override { return false; }
		// ~IPathObstacles
	};

public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return (BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_START_LEVEL)); }
	// IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "Dymmy"; };
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// IEntityPropertyGroup


	// IMovementActorAdapter
	virtual void                  OnMovementPlanProduced() override {}
	virtual NavigationAgentTypeID GetNavigationAgentTypeID() const override { return m_navigationAgentTypeId; }
	virtual Vec3                  GetPhysicsPosition() const override { return GetEntity()->GetWorldPos(); }
	virtual Vec3                  GetVelocity() const override;
	virtual Vec3                  GetMoveDirection() const override { return GetVelocity().GetNormalized(); }
	virtual Vec3                  GetAnimationBodyDirection() const override { return m_requestedTargetBodyDirection; }
	virtual EActorTargetPhase     GetActorPhase() const override { return eATP_None; }
	virtual void SetMovementOutputValue(const PathFollowResult& result) override;
	virtual void                  SetBodyTargetDirection(const Vec3& direction) override { m_requestedTargetBodyDirection = direction; }
	virtual void                  ResetMovementContext() override {}
	virtual void                  ClearMovementState() override;
	virtual void                  ResetBodyTarget() override {}
	virtual void                  ResetActorTargetRequest() override {}
	virtual bool                  IsMoving() const override { return !GetVelocity().IsZero(0.01f); }

	virtual void                  RequestExactPosition(const SAIActorTargetRequest* request, const bool lowerPrecision) override {}

	virtual bool                  IsClosestToUseTheSmartObject(const OffMeshLink_SmartObject& smartObjectLink) const override { return false; }
	virtual bool                  PrepareNavigateSmartObject(CSmartObject* pSmartObject, OffMeshLink_SmartObject* pSmartObjectLink) override { return false; }
	virtual void                  InvalidateSmartObjectLink(CSmartObject* pSmartObject, OffMeshLink_SmartObject* pSmartObjectLink) override {}

	virtual void                  SetInCover(const bool inCover) override {}
	virtual void                  UpdateCoverLocations() override {}
	virtual void                  InstallInLowCover(const bool inCover) override {}
	virtual void                  SetupCoverInformation() override {}
	virtual bool                  IsInCover() const override { return false; }

	virtual bool                  GetDesignedPath(SShape& pathShape) const override { return false; }
	virtual void                  CancelRequestedPath() override {}
	virtual void                  ConfigurePathfollower(const MovementStyle& style) override {}

	virtual void                  SetActorPath(const MovementStyle& style, const INavPath& navPath) override {}
	virtual void                  SetActorStyle(const MovementStyle& style, const INavPath& navPath) override {}
	virtual void                  SetStance(const MovementStyle::Stance stance) override {}

	virtual std::shared_ptr<Vec3> CreateLookTarget() override { return nullptr; }
	virtual void                  SetLookTimeOffset(float lookTimeOffset) override {}
	virtual void UpdateLooking(float updateTime, std::shared_ptr<Vec3> lookTarget, const bool targetReachable, const float pathDistanceToEnd, const Vec3& followTargetPosition, const MovementStyle& style) override {}
	// IMovementActorAdapter

	// IAIPathAgent
	virtual IEntity *GetPathAgentEntity() const override { return GetEntity(); }
	virtual const char *GetPathAgentName() const override { return GetEntity()->GetClass()->GetName(); }

	virtual unsigned short GetPathAgentType() const override { return AIOBJECT_ACTOR; } //AIOBJECT_DUMMY //AIOBJECT_ACTOR

	virtual float GetPathAgentPassRadius() const override { return 1.f; }
	virtual Vec3 GetPathAgentPos() const override { return GetPathAgentEntity()->GetWorldPos(); }
	virtual Vec3 GetPathAgentVelocity() const override { return GetVelocity(); }

	virtual const AgentMovementAbility& GetPathAgentMovementAbility() const override { return m_movementAbility; }

	virtual void GetPathAgentNavigationBlockers(NavigationBlockers& blockers, const PathfindRequest* pRequest) override {}

	virtual unsigned int GetPathAgentLastNavNode() const override { return 0; }
	virtual void SetPathAgentLastNavNode(unsigned int lastNavNode) override {}

	virtual void SetPathToFollow(const char* pathName) override {}
	virtual void         SetPathAttributeToFollow(bool bSpline) override {}

	virtual void SetPFBlockerRadius(int blockerType, float radius) override {}

	virtual ETriState CanTargetPointBeReached(CTargetPointRequest& request) override
	{
		request.SetResult(eTS_false);
		return eTS_false;
	}

	virtual bool UseTargetPointRequest(const CTargetPointRequest& request) override { return false; }

	virtual bool GetValidPositionNearby(const Vec3& proposedPosition, Vec3& adjustedPosition) const override { return false; }
	virtual bool GetTeleportPosition(Vec3& teleportPos) const override { return false; }

	virtual IPathFollower *GetPathFollower() const override { return m_pPathFollower.get(); }

	virtual bool IsPointValidForAgent(const Vec3& pos, uint32 flags) const override { return true; }
	// IAIPathAgent

	// PathFindingExt
	void RequestMoveTo(const Vec3 &position);
	void CancelCurrentRequest();
	bool IsProcessingRequest() { return m_movementRequestId != 0; }
	void ResetPF();
	void InitAI();
	// PathFindingExt

	// MovementExt
	void Physicalize();
	void RequestMove(const Vec3 &direction);
	void SetVelocity(const Vec3 &velocity);
	Vec3 GetVelocity2() const; 
	void Update(SEntityUpdateContext &ctx, int updateSlot);
	void GetLatestPhysicsStats(IPhysicalEntity &physicalEntity);
	// MovementExt 

	// Custom
	Vec3 RaycastRandom();
	void Walk();
	void GoForAll();
	// Custom

protected:
	void MovementRequestCallback(const MovementRequestResult &result) {}

	INavPath *GetINavPath() { return m_pFoundPath; };
	Movement::PathfinderState GetPathfinderState() { return m_state; }
	void RequestPathTo(MNMPathRequest& request);

	void OnMNMPathResult(const MNM::QueuedPathID& requestId, MNMPathRequestResult& result);

protected:
	void Reset();


	string geometry = "";
	float mass = 10;

	NavigationAgentTypeID m_navigationAgentTypeId;
	MovementActorCallbacks m_callbacks;
	MovementRequestID m_movementRequestId;
	uint32 m_pathFinderRequestId;
	std::shared_ptr<IPathFollower> m_pPathFollower;
	CPathObstacles m_pathObstacles;
	Movement::PathfinderState m_state;
	INavPath *m_pFoundPath;
	AgentMovementAbility m_movementAbility;
	Vec3 m_requestedTargetBodyDirection;


	bool m_bOnGround;
	Vec3 m_groundNormal;
};


