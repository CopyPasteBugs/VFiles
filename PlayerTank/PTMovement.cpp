#include "StdAfx.h"
#include "PTMovement.h"

#include "PlayerTank.h"
#include "PTInput.h"

CRYREGISTER_CLASS(CPlayerTankMovement)

void CPlayerTankMovement::Initialize()
{
	// Active for Update mask working
	GetEntity()->Activate(true);

	// Get main component of host Entity for internal using
	pTank = static_cast<CPlayerTank *>(GetEntity()->GetComponent<CPlayerTank>());
}

void CPlayerTankMovement::OnShutDown()
{
	pTank = nullptr;
}

void CPlayerTankMovement::ProcessEvent(SEntityEvent & event)
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
		//Update(*param);
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

void CPlayerTankMovement::Update(SEntityUpdateContext & ctx)
{
	Matrix34 playerTransform = GetEntity()->GetWorldTM();

	// Start with calculating movement direction
	Vec3 moveDirection = pTank->GetInput()->GetMovementDirection();
	moveDirection *= pTank->GetSpeed();

	// Add move direction to player position
	playerTransform.AddTranslation(GetEntity()->GetWorldRotation() * moveDirection * ctx.fFrameTime);

	// Update view rotation based on input
	Vec2 mouseDeltaRotation = pTank->GetInput()->GetAndResetMouseDeltaRotation();
	if (!mouseDeltaRotation.IsZero())
	{
		Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(playerTransform));

		ypr.x += mouseDeltaRotation.x * ctx.fFrameTime * 0.05f;
		ypr.y += mouseDeltaRotation.y * ctx.fFrameTime * 0.05f;
		ypr.y = clamp_tpl(ypr.y, -(float)g_PI * 0.5f, (float)g_PI * 0.5f);
		ypr.z = 0;

		playerTransform.SetRotation33(CCamera::CreateOrientationYPR(ypr));
	}

	// Now set the new player transform on the entity
	GetEntity()->SetWorldTM(playerTransform);
}
