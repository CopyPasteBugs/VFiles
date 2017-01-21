#include "StdAfx.h"
#include "PlayerTank.h"

#include "GamePlugin.h"
#include "PTInput.h"
#include "PTMovement.h"
#include "PTView.h"

#include "Player/ISimpleActor.h"

class CPlayerTankRegistrator
	: public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CPlayerTank>("PlayerTank", "UserStuff");
	}
	virtual void Unregister() override
	{

	}

public:
	CPlayerTankRegistrator() {}
	~CPlayerTankRegistrator()
	{

	}
};

static CPlayerTankRegistrator g_playerTankRegistrator;

CRYREGISTER_CLASS(CPlayerTank)

void CPlayerTank::Initialize()
{
	// Create GameObject for Entity
	pGameObject = gEnv->pGameFramework->GetIGameObjectSystem()->CreateGameObjectForEntity(GetEntityId());
	
	// Add Components
	pInput = GetEntity()->GetOrCreateComponent<CPlayerTankInput>();
	pMovement = GetEntity()->GetOrCreateComponent<CPlayerTankMovement>();
	pView = GetEntity()->GetOrCreateComponent<CPlayerTankView>();
	
	// Active for Update mask working
	GetEntity()->Activate(true);

	GetEntity()->SetUpdatePolicy(EEntityUpdatePolicy::ENTITY_UPDATE_ALWAYS);
	GetEntity()->SetFlags(EEntityFlags::ENTITY_FLAG_CASTSHADOW);
	GetEntity()->SetViewDistRatio(255);

	// Reset
	OnResetState();	
}

void CPlayerTank::OnShutDown()
{
	GetEntity()->RemoveComponent(pView);
	GetEntity()->RemoveComponent(pMovement);
	GetEntity()->RemoveComponent(pInput);
	pInput = nullptr;
	pView = nullptr;
	pMovement = nullptr;
}

void CPlayerTank::ProcessEvent(SEntityEvent & event)
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
			OnResetState();
			break;
		default:
			break;
		}
	}
	case ENTITY_EVENT_UPDATE:
	{
		SEntityUpdateContext* param = (SEntityUpdateContext*)event.nParam[0];
		if (pMovement) 
			pMovement->Update(*param);


		break;
	}
	case ENTITY_EVENT_START_LEVEL:
	{
		if (!gEnv->IsEditor())
		{
			OnResetState();
		}
		break;
	}
	default:
		break;
	}
}

void CPlayerTank::SerializeProperties(Serialization::IArchive & archive)
{
	archive(fSpeed, "Speed", "Speed");

	if (archive.isInput())
	{
		OnResetState();
	}
}

CPlayerTankInput * CPlayerTank::GetInput()
{
	return pInput;
}

CPlayerTankMovement * CPlayerTank::GetMovement()
{
	return pMovement;
}

CPlayerTankView * CPlayerTank::GetView()
{
	return pView;
}

void CPlayerTank::OnResetState()
{
	fSpeed = 2.0;
	GetEntity()->LoadGeometry(0, "Assets/Objects/Default/primitive_sphere_small.cgf");

	//SEntityPhysicalizeParams physParams;
	//physParams.type = PE_LIVING;
	//physParams.nSlot = 0;
	//physParams.mass = 10.0f;

	//pe_player_dimensions playerDimensions;
	//// Prefer usage of a cylinder instead of capsule
	//playerDimensions.bUseCapsule = 0;
	//// Specify the size of our cylinder
	//playerDimensions.sizeCollider = Vec3(0.7f, 0.7f, 1.0f);
	//// Keep pivot at the player's feet (defined in player geometry) 
	//playerDimensions.heightPivot = 0.0f;
	//// Offset collider upwards
	//playerDimensions.heightCollider = 1.0f;
	//playerDimensions.groundContactEps = 0.004f;
	//physParams.pPlayerDimensions = &playerDimensions;


	//pe_player_dynamics playerDynamics;
	//playerDynamics.kAirControl = 0.0f;
	//playerDynamics.kAirResistance = 1.0;
	//playerDynamics.mass = physParams.mass;
	//physParams.pPlayerDynamics = &playerDynamics;

	//GetEntity()->Physicalize(physParams);
}
