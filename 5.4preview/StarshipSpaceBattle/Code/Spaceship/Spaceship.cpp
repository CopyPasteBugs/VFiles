#include "StdAfx.h"
#include "Spaceship.h"
#include "Bullet\Bullet.h"

#include <CryGame/IGameFramework.h>
#include <CryAction/IMaterialEffects.h>


#include <CryAudio/IAudioInterfacesCommonData.h>

void CSpaceship::ReflectType(Schematyc::CTypeDesc<CSpaceship>& desc)
{
	desc.SetGUID(CSpaceship::IID());
	desc.SetEditorCategory("User");
	desc.SetLabel("Spaceship");
	desc.SetDescription("A component ");
	//desc.SetName(Schematyc::CTypeName("Name123"));
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });

	desc.AddMember(&CSpaceship::value, 'myBo', "MyBool", "My Boolean", "Whether or not we should be true", false);
	
}

void CSpaceship::Register(Schematyc::CEnvRegistrationScope & componentScope)
{

}

CSpaceship::~CSpaceship()
{

}

void CSpaceship::Initialize()
{
	OnReset();
}

uint64 CSpaceship::GetEventMask() const
{
	return BIT64(ENTITY_EVENT_START_GAME) | BIT64(ENTITY_EVENT_UPDATE); //BIT64(ENTITY_EVENT_RESET)
}

void CSpaceship::ProcessEvent(SEntityEvent & event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_RESET:
	{
		//OnReset();
	}
	break;
	case ENTITY_EVENT_START_GAME:
	{
		camEntity = FindCameraEntity();
		if (camEntity)
		{
			LocateCamBehindShip(camEntity);
			baseShipVsCamDistance = GetCamDistance();
			
		}
		//const char* s = ent->GetName();
		
		// Revive the entity when gameplay starts

	}
	break;
	case ENTITY_EVENT_UPDATE:
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		const float moveSpeed = 20.5f;
		Vec3 velocity = ZERO;

		// Check input to calculate local space velocity
		if (inputFlags & (TInputFlags)EInputFlag::MoveLeft)
		{
			velocity.x -= moveSpeed * pCtx->fFrameTime;
		}
		if (inputFlags & (TInputFlags)EInputFlag::MoveRight)
		{
			velocity.x += moveSpeed * pCtx->fFrameTime;
		}
		if (inputFlags & (TInputFlags)EInputFlag::MoveForward)
		{
			float actualDistance = GetCamDistance() - baseShipVsCamDistance;
			if (actualDistance < 5.0f)
			{
				velocity.y += moveSpeed * pCtx->fFrameTime;
			}
		}
		if (inputFlags & (TInputFlags)EInputFlag::MoveBack)
		{
			float actualDistance = GetCamDistance() - baseShipVsCamDistance;
			if (actualDistance > -3.0f)
			{
				velocity.y -= moveSpeed * pCtx->fFrameTime;
			}
		}
		if (inputFlags & (TInputFlags)EInputFlag::Fire)
		{
			static float ft = 0.0f;
			
			if (ft > 0.1f) {
				SpawnBullet();
				ft = 0.0f;

				if (soundFire)
					soundFire->ExecuteTrigger(controlId);
			}

			ft += pCtx->fFrameTime;
		}

		// Update the player's transformation
		Matrix34 transformation = m_pEntity->GetWorldTM();
		transformation.AddTranslation(transformation.TransformVector(velocity));
		m_pEntity->SetWorldTM(transformation);

	}
	break;
	}
}

void CSpaceship::OnShutDown()
{
	m_pEntity->RemoveComponent(m_pInputComponent);
	m_pEntity->RemoveComponent(soundFire);
	m_pInputComponent = nullptr;
	soundFire = nullptr;
}

void CSpaceship::OnReset()
{
	LoadModel();
	AcquireComponents();
	Physicalize();

	soundFire = m_pEntity->GetOrCreateComponent<IEntityAudioComponent>();
	if (soundFire) 
	{
		soundFire->SetCurrentEnvironments();
		controlId = CryAudio::StringToId_CompileTime("fire1");
		//controlId = CryAudio::StringToId_RunTime("fire1"); // trigger name in ACE
		soundFire->ExecuteTrigger(controlId);
	}
}

IEntity * CSpaceship::FindCameraEntity()
{
	if (!gEnv->IsEditor())
	{
		// Find first camera in level
		auto *pEntityIterator = gEnv->pEntitySystem->GetEntityIterator();
		pEntityIterator->MoveFirst();

		while (!pEntityIterator->IsEnd())
		{
			IEntity *pEntity = pEntityIterator->Next();

			if (auto* pComponent = pEntity->GetComponent<Cry::DefaultComponents::CCameraComponent>())
			{
				return pComponent->GetEntity();
			}
		}
	}

	return nullptr;
}

void CSpaceship::LoadModel()
{
	GetEntity()->LoadGeometry(geometrySlot, "Assets\\objects\\Spaceship\\spaceship.cgf");

}

void CSpaceship::AcquireComponents()
{
	// Get the input component, wraps access to action mapping so we can easily get callbacks when inputs are triggered
	m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();
	// Register an action, and the callback that will be sent when it's triggered
	m_pInputComponent->RegisterAction("player", "moveleft", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveLeft, activationMode);  });
	// Bind the 'A' key the "moveleft" action
	m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, EKeyId::eKI_A);

	m_pInputComponent->RegisterAction("player", "moveright", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveRight, activationMode);  });
	m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, EKeyId::eKI_D);

	m_pInputComponent->RegisterAction("player", "moveforward", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveForward, activationMode);  });
	m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, EKeyId::eKI_W);

	m_pInputComponent->RegisterAction("player", "moveback", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveBack, activationMode);  });
	m_pInputComponent->BindAction("player", "moveback", eAID_KeyboardMouse, EKeyId::eKI_S);

	m_pInputComponent->RegisterAction("player", "fire", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::Fire, activationMode);  });
	m_pInputComponent->BindAction("player", "fire", eAID_KeyboardMouse, EKeyId::eKI_Space);


	m_pInputComponent->RegisterAction("player", "mouse_rotateyaw", [this](int activationMode, float value) { mouseDeltaRotation.x -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotateyaw", eAID_KeyboardMouse, EKeyId::eKI_MouseX);

	m_pInputComponent->RegisterAction("player", "mouse_rotatepitch", [this](int activationMode, float value) { mouseDeltaRotation.y -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotatepitch", eAID_KeyboardMouse, EKeyId::eKI_MouseY);


	//m_pSoundComponent = m_pEntity->GetOrCreateComponent<Cry::Audio::DefaultComponents::CTriggerComponent>();
	


}

void CSpaceship::LocateCamBehindShip(IEntity * ent)
{
	Vec3 p = this->GetEntity()->GetWorldPos();

	ent->SetPos(p + Vec3(0, -20, 10));
}

void CSpaceship::HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type)
{
	switch (type)
	{
	case EInputFlagType::Hold:
	{
		if (activationMode == eIS_Released)
		{
			inputFlags &= ~flags;
		}
		else
		{
			inputFlags |= flags;
		}
	}
	break;
	case EInputFlagType::Toggle:
	{
		if (activationMode == eIS_Released)
		{
			// Toggle the bit(s)
			inputFlags ^= flags;
		}
	}
	break;
	}
}

float CSpaceship::GetCamDistance()
{
	if (camEntity) 
	{
		Vec3 shipPos = GetEntity()->GetPos();
		Vec3 camPos = camEntity->GetPos();
		return (shipPos - camPos).len();
	}

	return 0.0;
}

void CSpaceship::SpawnBullet()
{

	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();

	spawnParams.vPosition = GetEntity()->GetPos() + Vec3(0, 3, 0);
	spawnParams.qRotation = GetEntity()->GetRotation();
	spawnParams.vScale = Vec3(4,4,4);
	spawnParams.sName = "Bullet";

	// Spawn the entity
	if (IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		// See Bullet.cpp, bullet is propelled in  the rotation and position the entity was spawned with
		pEntity->CreateComponentClass<CBulletComponent>();
	}
}

void CSpaceship::Physicalize()
{
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_LIVING;
	physParams.mass = 90;
	physParams.nSlot = geometrySlot;
	physParams.nFlagsOR = geom_colltype3 | geom_colltype_ray;

	pe_player_dimensions playerDimensions;
	// Prefer usage of a cylinder instead of capsule
	playerDimensions.bUseCapsule = 0;

	// Specify the size of our cylinder
	playerDimensions.sizeCollider = Vec3(1.f, 1.f, 1.f);
	// Keep pivot at the player's feet (defined in player geometry) 
	playerDimensions.heightPivot = 0.1f;
	// Offset collider upwards
	playerDimensions.heightCollider = 0.f;
	playerDimensions.groundContactEps = 0.004f;

	physParams.pPlayerDimensions = &playerDimensions;

	pe_player_dynamics playerDynamics;
	playerDynamics.kAirControl = 0.f;
	playerDynamics.mass = physParams.mass;
	physParams.pPlayerDynamics = &playerDynamics;

	GetEntity()->Physicalize(physParams);
	

	//SEntityPhysicalizeParams physParams;
	//physParams.type = PE_RIGID;
	//physParams.mass = 90;
	//physParams.nSlot = 1;

	//GetEntity()->Physicalize(physParams);
	//GetEntity()->EnablePhysics(true);

	//pe_params_collision_class pcc;
	//pcc.collisionClassOR.type = GAME_COLLITION_CLASSES::GCC_SPACESHIP;
	//pcc.collisionClassOR.ignore = GAME_COLLITION_CLASSES::GCC_BULLET;
	//GetEntity()->GetPhysicalEntity()->SetParams(&pcc, 0);

	//ISurfaceTypeManager *pSurfaceTypeManager = Get3DEngine()->GetMaterialManager()->GetSurfaceTypeManager();

	

	// Create virt phys plane in world for spaceship entity only
	{
		SEntitySpawnParams spawnParams;
		spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
		spawnParams.vPosition = GetEntity()->GetPos() - Vec3(0, 0, 2);	// posing plane slighter down than cur pos of spaceship
		spawnParams.qRotation = GetEntity()->GetRotation();
		spawnParams.vScale = Vec3Constants<float>::fVec3_One;

		// Spawn default the entity (with static phys + box geometry)
		if (IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
		{
			SEntityPhysicalizeParams physParams;
			physParams.type = PE_STATIC;
			physParams.mass = 1000;
			physParams.nSlot = 1;
			pEntity->Physicalize(physParams);
			
			primitives::box box;
			box.center = Vec3(0, 0, 0);
			box.size = Vec3(50, 50, 1);
			box.Basis = Matrix33::CreateIdentity();

			IGeometry *pcaps = gEnv->pPhysicalWorld->GetGeomManager()->CreatePrimitive(primitives::box::type, &box);
			phys_geometry *pGeom = gEnv->pPhysicalWorld->GetGeomManager()->RegisterGeometry(pcaps);
			pcaps->Release();

			pe_geomparams gp;
			gp.flags = geom_colltype1 | geom_colltype_ray;
			gp.flagsCollider = geom_colltype2;
			gp.mass = physParams.mass;
			
			pEntity->GetPhysics()->AddGeometry(pGeom, &gp);
			pGeom->nRefCount--;				
		}
	}

	//GetEntity()->GetPhysicalEntity()->SetParams(&gp, 0);
}

