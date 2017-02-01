#include "StdAfx.h"
#include "PTInput.h"

#include "GamePlugin.h"
#include "PlayerTank.h"

CRYREGISTER_CLASS(CPlayerTankInput)

void CPlayerTankInput::Initialize()
{
	// Active for Update mask working
	GetEntity()->Activate(true);

	// Get main component of host Entity for internal using
	pTank = static_cast<CPlayerTank *>(GetEntity()->GetComponent<CPlayerTank>());

	// NOTE: Since CRYENGINE 5.3, the game is responsible to initialize the action maps
	IActionMapManager *pActionMapManager = gEnv->pGameFramework->GetIActionMapManager();
	pActionMapManager->InitActionMaps("Libs/config/defaultprofile.xml");
	pActionMapManager->Enable(true);
	pActionMapManager->EnableActionMap("player", true);
	
	if (IActionMap *pActionMap = pActionMapManager->GetActionMap("player"))
	{
		pActionMap->SetActionListener(GetEntityId());
	}

	pTank->pGameObject->CaptureActions(this);

	// Populate the action handler callbacks so that we get action map events
	InitializeActionHandler();
}

void CPlayerTankInput::OnShutDown()
{
	pTank->pGameObject->ReleaseActions(this);
	pTank = nullptr;
}

void CPlayerTankInput::ProcessEvent(SEntityEvent & event)
{

}

void CPlayerTankInput::OnAction(const ActionId & action, int activationMode, float value)
{
	m_actionHandler.Dispatch(this, GetEntityId(), action, activationMode, value);
}

void CPlayerTankInput::InitializeActionHandler()
{
	m_actionHandler.AddHandler(ActionId("tankactionf"), &CPlayerTankInput::OnActionF);

	m_actionHandler.AddHandler(ActionId("moveleft"), &CPlayerTankInput::OnActionMoveLeft);
	m_actionHandler.AddHandler(ActionId("moveright"), &CPlayerTankInput::OnActionMoveRight);
	m_actionHandler.AddHandler(ActionId("moveforward"), &CPlayerTankInput::OnActionMoveForward);
	m_actionHandler.AddHandler(ActionId("moveback"), &CPlayerTankInput::OnActionMoveBack);

	m_actionHandler.AddHandler(ActionId("mouse_rotateyaw"), &CPlayerTankInput::OnActionMouseRotateYaw);
	m_actionHandler.AddHandler(ActionId("mouse_rotatepitch"), &CPlayerTankInput::OnActionMouseRotatePitch);
}

bool CPlayerTankInput::OnActionF(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{

	return true;
}

bool CPlayerTankInput::OnActionMoveLeft(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	m_moveDirection.x = -value;
	return true;
}

bool CPlayerTankInput::OnActionMoveRight(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	m_moveDirection.x = value;
	return true;
}

bool CPlayerTankInput::OnActionMoveForward(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	m_moveDirection.y = value;
	return true;
}

bool CPlayerTankInput::OnActionMoveBack(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	m_moveDirection.y = -value;
	return true;
}

bool CPlayerTankInput::OnActionMouseRotateYaw(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	m_mouseDeltaRotation.x -= value;
	return true;
}

bool CPlayerTankInput::OnActionMouseRotatePitch(EntityId entityId, const ActionId & actionId, int activationMode, float value)
{
	m_mouseDeltaRotation.y -= value;
	return true;
}

