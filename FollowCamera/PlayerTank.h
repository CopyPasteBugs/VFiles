#include "Entities\Helpers\ISimpleExtension.h"

#include <CryMath/Cry_Camera.h>

#include <IViewSystem.h>
#include <IActorSystem.h>

#include "PlayerTank\PTView.h"

class CPlayerTank;

class CPlayerTank
	: public CGameObjectExtensionHelper<CPlayerTank, ISimpleExtension>
	, public IActionListener
{
public:
	enum EInputFlagType
	{
		eInputFlagType_Hold = 0,
		eInputFlagType_Toggle
	};

	typedef uint8 TInputFlags;

	enum EInputFlags
		: TInputFlags
	{
		eInputFlag_MoveLeft = 1 << 0,
		eInputFlag_MoveRight = 1 << 1,
		eInputFlag_MoveForward = 1 << 2,
		eInputFlag_MoveBack = 1 << 3
	};

public:
	CPlayerTank();
	virtual ~CPlayerTank();

	// ISimpleActor
	virtual bool Init(IGameObject* pGameObject) override;
	virtual void PostInit(IGameObject* pGameObject) override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	// ~ISimpleActor

	// IActionListener
	virtual void OnAction(const ActionId &action, int activationMode, float value) override;
	// ~IActionListener

	// Input
	const TInputFlags GetInputFlags() const { return m_inputFlags; };
	const Vec2 GetMouseDeltaRotation() const { return m_mouseDeltaRotation; }
	const float GetMouseDeltaWheel() const { return m_mouseDeltaWheel; }
	const Quat &GetLookOrientation() const { return m_lookOrientation; }
	// Input

	// Movement
	Vec3 GetLocalMoveDirection() const;
	bool IsOnGround() const { return m_bOnGround; }
	Vec3 GetGroundNormal() const { return m_groundNormal; }
	virtual void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	// Movement

	// IGameObjectView
	//virtual void UpdateView(SViewParams &viewParams) override;
	//virtual void PostUpdateView(SViewParams &viewParams) override {}
	// ~IGameObjectView

protected:
	void InitializeActionHandler();
	void HandleInputFlagChange(EInputFlags flags, int activationMode, EInputFlagType type = eInputFlagType_Hold);
	bool OnActionF(EntityId entityId, const ActionId & actionId, int activationMode, float value);
	bool OnActionMoveLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveRight(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveForward(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveBack(EntityId entityId, const ActionId& actionId, int activationMode, float value);

	bool OnActionMouseRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMouseRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionShoot(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	bool OnActionMouseWheelUp(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMouseWheelDown(EntityId entityId, const ActionId& actionId, int activationMode, float value);

	bool OnActionJump(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	
	// Movement
	// Get the stats from latest physics thread update
	void GetLatestPhysicsStats(IPhysicalEntity &physicalEntity);
	void UpdateMovementRequest(float frameTime, IPhysicalEntity &physicalEntity);
	// Movement 


	void OnResetState();


protected:
	// Input
	bool m_bOnGround;
	Vec3 m_groundNormal;
	TInputFlags m_inputFlags;
	// Handler for actionmap events that maps actions to callbacks
	TActionHandler<CPlayerTank> m_actionHandler;
	bool requestJump = false;
	// Input

	// View
	Vec2 m_mouseDeltaRotation;
	float m_mouseDeltaWheel;
	// Should translate to head orientation in the future
	Quat m_lookOrientation;
	// View


	// Settings
	float fSpeed = 10.0f;
	float fRotationSpeedYaw = 0.05f;
	float fRotationSpeedPitch = 0.05f;
	float fRotationLimitsMinPitch = -0.84f;
	float fRotationLimitsMaxPitch = 1.5f;
	float fViewOffsetY = -1.5f;
	float fViewOffsetZ = 1.0f;

	CTankView* pView;

	IEntity* pParticleHelper;
};