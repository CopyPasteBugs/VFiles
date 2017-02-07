#pragma once
#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <IActionMapManager.h>
#include <CryMath/Cry_Camera.h>
#include <IViewSystem.h>
#include <IActorSystem.h>

class CPlayer;

class CInput final
	: public IEntityComponent
	, public IInputEventListener
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CInput, "Input", 0xC64BA3E574364AB, 0x9383E9B961E37001);
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
		eInputFlag_MoveLeft = BIT(0),
		eInputFlag_MoveRight = BIT(1),
		eInputFlag_MoveForward = BIT(2),
		eInputFlag_MoveBack = BIT(3)
	};

public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return 0; };
	virtual void OnShutDown() override;
	// IEntityComponent

	// IInputEventListener
	virtual bool OnInputEvent(const SInputEvent& event);
	// IInputEventListener

	void Update(SEntityUpdateContext* param);

	const Vec2 GetDeltaRotation() const { return mouseDeltaRotation; };
	const float GetDeltaWheel() const { return mouseDeltaWheel; };
	const TInputFlags GetMovementFlags() const { return m_inputFlags; };
	const Vec3 GetLocalMoveDirection() const;
	const bool GetJumpState() const { return requestJump; };
	void SetJumpState(bool jumpState) { requestJump = jumpState; };
	void ClearJumpState() { SetJumpState(false); };
	const bool GetUseState() const { return bPlayerPressE; };
	void ClearUseState() { bPlayerPressE = false; };

	void OnResetState();

protected:
	void InitializeActionHandler();

	void HandleInputFlagChange(EInputFlags flags, int activationMode, EInputFlagType type = eInputFlagType_Hold);
	bool OnUse(EntityId entityId, const ActionId & actionId, int activationMode, float value);
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
	bool OnActionFindWay(EntityId entityId, const ActionId & actionId, int activationMode, float value);
	bool OnExit(EntityId entityId, const ActionId & actionId, int activationMode, float value);

protected:
	TActionHandler<CInput> m_actionHandler;
	TInputFlags m_inputFlags;
	std::map<EKeyId, ActionId> keyMap;
	CPlayer* pHost = nullptr;
	int forTest = 0;
	bool requestJump = false;
	Vec2 mouseDeltaRotation;
	float mouseDeltaWheel = 0.0f;
	bool bPlayerPressE = false;

};