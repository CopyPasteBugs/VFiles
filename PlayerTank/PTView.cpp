#include "StdAfx.h"
#include "PTView.h"

#include "PlayerTank.h"
#include "PTMovement.h"
#include "PTInput.h"

#include "Entities/Helpers/ISimpleExtension.h"

CRYREGISTER_CLASS(CPlayerTankView)

void CPlayerTankView::Initialize()
{
	// Get main component of host Entity for internal using
	pTank = static_cast<CPlayerTank *>(GetEntity()->GetComponent<CPlayerTank>());
	pTank->pGameObject->CaptureView(this);
}

void CPlayerTankView::OnShutDown()
{
	pTank->pGameObject->ReleaseView(this);
}

void CPlayerTankView::ProcessEvent(SEntityEvent & event)
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
		}
	}
	default:
		break;
	}
}

void CPlayerTankView::UpdateView(SViewParams & viewParams)
{
	viewParams.position = GetEntity()->GetWorldPos();
	viewParams.rotation = GetEntity()->GetWorldRotation();
}

