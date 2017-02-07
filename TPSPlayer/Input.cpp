#include "StdAfx.h"
#include "Input.h"
#include "GamePlugin.h"
#include "Player.h"
#include "View.h"
#include "Marker.h"
#include "Attack.h"

CRYREGISTER_CLASS(CInput)

void CInput::Initialize()
{
	OnResetState();
}

void CInput::ProcessEvent(SEntityEvent & event)
{

}

void CInput::OnShutDown()
{
	if (gEnv->pInput)
		gEnv->pInput->RemoveEventListener(this);
}

bool CInput::OnInputEvent(const SInputEvent & event)
{
	if (event.keyId != EKeyId::eKI_SYS_Commit)
	{
		int activationMode = event.state;
		float value = event.value;

		m_actionHandler.Dispatch(this, GetEntityId(), keyMap[event.keyId], activationMode, value);
		return true;
	}
	return false;
}

void CInput::Update(SEntityUpdateContext * param)
{

}

const Vec3 CInput::GetLocalMoveDirection() const
{
	Vec3 moveDirection = ZERO;

	uint32 inputFlags = (uint32)GetMovementFlags();

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

void CInput::OnResetState()
{
	m_actionHandler.Clear();

	keyMap[EKeyId::eKI_A] = ActionId("moveleft");
	keyMap[EKeyId::eKI_D] = ActionId("moveright");
	keyMap[EKeyId::eKI_W] = ActionId("moveforward");
	keyMap[EKeyId::eKI_S] = ActionId("moveback");
	keyMap[EKeyId::eKI_MouseX] = ActionId("mouse_rotateyaw");
	keyMap[EKeyId::eKI_MouseY] = ActionId("mouse_rotatepitch");
	keyMap[EKeyId::eKI_MouseWheelUp] = ActionId("wheelup");
	keyMap[EKeyId::eKI_MouseWheelDown] = ActionId("wheeldown");
	keyMap[EKeyId::eKI_Mouse1] = ActionId("shoot");
	keyMap[EKeyId::eKI_Space] = ActionId("jump");
	keyMap[EKeyId::eKI_Escape] = ActionId("exit");
	keyMap[EKeyId::eKI_F] = ActionId("findway");
	keyMap[EKeyId::eKI_R] = ActionId("use2");

	InitializeActionHandler();

	if (gEnv->pInput)
		gEnv->pInput->AddEventListener(this);

	pHost = GetEntity()->GetComponent<CPlayer>();
}

void CInput::InitializeActionHandler()
{
	m_actionHandler.AddHandler(ActionId("moveleft"), &CInput::OnActionMoveLeft);
	m_actionHandler.AddHandler(ActionId("moveright"), &CInput::OnActionMoveRight);
	m_actionHandler.AddHandler(ActionId("moveforward"), &CInput::OnActionMoveForward);
	m_actionHandler.AddHandler(ActionId("moveback"), &CInput::OnActionMoveBack);

	m_actionHandler.AddHandler(ActionId("mouse_rotateyaw"), &CInput::OnActionMouseRotateYaw);
	m_actionHandler.AddHandler(ActionId("mouse_rotatepitch"), &CInput::OnActionMouseRotatePitch);

	m_actionHandler.AddHandler(ActionId("wheelup"), &CInput::OnActionMouseWheelUp);
	m_actionHandler.AddHandler(ActionId("wheeldown"), &CInput::OnActionMouseWheelDown);

	m_actionHandler.AddHandler(ActionId("shoot"), &CInput::OnActionShoot);
	m_actionHandler.AddHandler(ActionId("jump"), &CInput::OnActionJump);
	m_actionHandler.AddHandler(ActionId("findway"), &CInput::OnActionFindWay);
	m_actionHandler.AddHandler(ActionId("exit"), &CInput::OnExit);
	m_actionHandler.AddHandler(ActionId("use2"), &CInput::OnUse);

}

void CInput::HandleInputFlagChange(EInputFlags flags, int activationMode, EInputFlagType type)
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

bool CInput::OnUse(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	if (activationMode == eIS_Pressed)
		bPlayerPressE = true;

	if (activationMode == eIS_Released)
		bPlayerPressE = false;

	return true;
}

bool CInput::OnActionF(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	return true;
}

bool CInput::OnActionMoveLeft(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	HandleInputFlagChange(eInputFlag_MoveLeft, activationMode);
	return true;
}

bool CInput::OnActionMoveRight(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	HandleInputFlagChange(eInputFlag_MoveRight, activationMode);
	return true;
}

bool CInput::OnActionMoveForward(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	HandleInputFlagChange(eInputFlag_MoveForward, activationMode);
	return true;
}

bool CInput::OnActionMoveBack(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	HandleInputFlagChange(eInputFlag_MoveBack, activationMode);
	return true;
}

bool CInput::OnActionMouseRotateYaw(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	mouseDeltaRotation.x = -value;
	return true;
}

bool CInput::OnActionMouseRotatePitch(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	mouseDeltaRotation.y = -value;
	return true;
}

bool CInput::OnActionShoot(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	// Only fire on press, not release
	if (activationMode == eIS_Pressed)
	{
		if (pHost)
		{
			//auto *pBarrelOutAttachment = pCharacter->GetIAttachmentManager()->GetInterfaceByName("BulletSpawnPlace");
			//QuatTS bulletOrigin = pBarrelOutAttachment->GetAttWorldAbsolute();;
			Vec3 dir = pHost->GetView()->GetCameraRootForward();
			Vec3 startPos = GetEntity()->GetWorldPos() + dir;
			pHost->GetAttack()->RequestFire(startPos, pHost->GetView()->GetCameraRootRotation());
		}
	}
	return true;
}

bool CInput::OnActionMouseWheelUp(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	mouseDeltaWheel = value;
	return true;
}

bool CInput::OnActionMouseWheelDown(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	mouseDeltaWheel = value;
	return true;
}

bool CInput::OnActionJump(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	if (activationMode == eIS_Pressed) 
		requestJump = true;
	
	return false;
}

bool CInput::OnActionFindWay(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	return true;
}

bool CInput::OnExit(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	gEnv->pSystem->Quit();
	return true;
}
