#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <IGameObject.h>

class CPlayerTank;

class CPlayerTankInput
	: public IEntityComponent
	, public IActionListener
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CPlayerTankInput, "CPlayerTankInput", 0x8A2B673FF28942D5, 0xA3EACF03A212C9BA);
public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual void OnShutDown() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return (BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_START_LEVEL)); }
	// IEntityComponent

	// IActionListener
	virtual void OnAction(const ActionId& action, int activationMode, float value);
	// IActionListener

	const Vec2 &GetMovementDirection() const { return m_moveDirection; }

	Vec2 GetAndResetMouseDeltaRotation()
	{
		auto deltaRotation = m_mouseDeltaRotation;

		m_mouseDeltaRotation = ZERO;
		return deltaRotation;
	}

protected:
	void InitializeActionHandler();

	bool OnActionF(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveRight(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveForward(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveBack(EntityId entityId, const ActionId& actionId, int activationMode, float value);

	bool OnActionMouseRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMouseRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value);
protected:
	CPlayerTank* pTank;

	Vec3 m_moveDirection;
	Vec2 m_mouseDeltaRotation;

	// Handler for actionmap events that maps actions to callbacks
	TActionHandler<CPlayerTankInput> m_actionHandler;
};