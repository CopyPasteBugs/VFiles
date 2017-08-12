#include "StdAfx.h"
#include "Asteroid.h"
#include <CryMath/Random.h>
#include "Bullet/Bullet.h"

void CAsteroidComponent::Register(Schematyc::CEnvRegistrationScope & componentScope)
{

}

void CAsteroidComponent::ReflectType(Schematyc::CTypeDesc<CAsteroidComponent>& desc)
{
	desc.SetGUID(CAsteroidComponent::IID());
	desc.SetEditorCategory("User");
	desc.SetLabel("Asteroid");
	desc.SetDescription("A component ");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::ClientOnly });

	desc.AddMember(&CAsteroidComponent::usedForBackround, 0, "AssetForBackground", "Background", "My Boolean", false);
}

CAsteroidComponent::~CAsteroidComponent()
{
	for (int32 i = 0; i < 8; i++) 
	{
		if (phys[i])
			gEnv->pPhysicalWorld->DestroyPhysicalEntity(phys[i]);
	}
}

void CAsteroidComponent::Initialize()
{
	//GetEntity()->SetFlags(GetEntity()->GetFlags() | ENTITY_FLAG_NO_SAVE);
	
	m_pEntity->LoadGeometry(geometrySlot, "Assets\\Objects\\Asteroid\\Asteroid.cgf");

	auto *pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("Materials\\asteroid");
	m_pEntity->SetMaterial(pMaterial);

	//LoadModelSeparatedParts();

	// Make sure that bullets are always rendered regardless of distance
	// Ratio is 0 - 255, 255 being 100% visibility
	GetEntity()->SetViewDistRatio(255);

	if (usedForBackround)
		InitPhysBackgroud();
	else
		InitPhysWithImpule();
	
	
}

uint64 CAsteroidComponent::GetEventMask() const
{
	uint64 mask = 0;

	if (!usedForBackround)
		mask = BIT64(ENTITY_EVENT_COLLISION) | BIT64(ENTITY_EVENT_UPDATE);

	return mask;
}

void CAsteroidComponent::ProcessEvent(SEntityEvent & event)
{
	// Handle the OnCollision event, in order to have the entity removed on collision
	if (event.event == ENTITY_EVENT_COLLISION)
	{
		EventPhysCollision *physCollision = reinterpret_cast<EventPhysCollision *>(event.nParam[0]);
		if (physCollision != nullptr)
		{
			IPhysicalEntity* pOtherEntityPhysics = m_pEntity->GetPhysics() != physCollision->pEntity[0] ? physCollision->pEntity[0] : physCollision->pEntity[1];
			IEntity* pOtherEntity = gEnv->pEntitySystem->GetEntityFromPhysics(pOtherEntityPhysics);
			//IEntity* pOtherEntity = gEnv->pEntitySystem->GetEntityFromPhysics(physCollision->pEntity[1]);

			if (pOtherEntity != nullptr)
			{
				CBulletComponent* bullet = pOtherEntity->GetComponent<CBulletComponent>();
				if (bullet)
				{
					ChangeStateDestroyed();
				}
			}

		}
		// Queue removal of this entity, unless it has already been done
		//gEnv->pEntitySystem->RemoveEntity(GetEntityId());
	}
	else if (event.event == ENTITY_EVENT_UPDATE)
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		if (lifeTime > 15.0f)
		{
			FreeAllResources();
			gEnv->pEntitySystem->RemoveEntity(GetEntityId());
			lifeTime = 0.0f;
		}
		lifeTime = lifeTime + pCtx->fFrameTime;

	}
}

void CAsteroidComponent::InitPhysBackgroud()
{
	{
		SEntityPhysicalizeParams physParams;
		physParams.type = PE_RIGID;
		physParams.mass = mass;
		physParams.nSlot = geometrySlot;
		physParams.nFlagsOR = geom_colltype0 | pef_ignore_areas | pef_ignore_ocean;

		m_pEntity->Physicalize(physParams);
	}
	// Set 0 gravity
	{
		pe_simulation_params simParams = {};
		simParams.mass = 500.0f;
		simParams.gravity = Vec3(0.0f, 0.0f, 0.0f);
		m_pEntity->GetPhysics()->SetParams(&simParams, 0);
	}
	// Ignore 
	{
		
		pe_params_collision_class pcc;
		pcc.collisionClassAND.ignore = geom_colltype0; // ingnore collitiosn with collider type 0
		pcc.collisionClassOR.ignore = geom_colltype0;
		m_pEntity->GetPhysics()->SetParams(&pcc, 0);
	}
	// Apply angular impulse only 
	if (auto *pPhysics = GetEntity()->GetPhysics())
	{
		pe_action_impulse impulseAction;
		impulseAction.point = GetEntity()->GetWorldPos();
		impulseAction.angImpulse = cry_random_componentwise(Vec3(-360, -360, -360), Vec3(360, 360, 360)) * cry_random(0.5f, 3.0f);
		impulseAction.point = GetEntity()->GetWorldPos();
		pPhysics->Action(&impulseAction);
	}

}

void CAsteroidComponent::InitPhysWithImpule()
{
	//Now create the physical representation of the entity
	{
		SEntityPhysicalizeParams physParams;
		physParams.type = PE_RIGID;
		physParams.mass = mass;
		physParams.nSlot = geometrySlot;
		physParams.nFlagsOR = pef_ignore_areas | pef_ignore_ocean | pef_log_collisions | pef_monitor_collisions;
		m_pEntity->Physicalize(physParams);
	}
	// Zero gravity
	{
		pe_simulation_params simParams = {};
		simParams.mass = 500.0f;
		simParams.gravity = Vec3(0.0f, 0.0f, 0.0f);
		m_pEntity->GetPhysics()->SetParams(&simParams, 0);

		pe_params_collision_class pcc;
		pcc.collisionClassOR.type = geom_colltype1; // entity's phys has collider type 1 
		pcc.collisionClassAND.type = geom_colltype1;
		pcc.collisionClassAND.ignore = geom_colltype0;
		pcc.collisionClassOR.ignore = geom_colltype0;

		m_pEntity->GetPhysics()->SetParams(&pcc, 0);
	}
	// Apply an impulse so that the bullet flies forward
	if (auto *pPhysics = GetEntity()->GetPhysics())
	{
		pe_action_impulse impulseAction;
		const float initialVelocity = 3000.1f;

		////impulseAction.impulse = -GetEntity()->GetWorldRotation().GetColumn1() * initialVelocity;
		//
		impulseAction.point = GetEntity()->GetWorldPos();
		impulseAction.impulse = (Vec3(0, -1, 0).normalize()) * initialVelocity;
		impulseAction.angImpulse = cry_random_componentwise(Vec3(-360, -360, -360), Vec3(360, 360, 360)) * cry_random(0.5f, 3.0f);
		pPhysics->Action(&impulseAction);

		//pe_action_set_velocity vel;
		//vel.v = -GetEntity()->GetWorldRotation().GetColumn1() * 10.0f;
		//pPhysics->Action(&vel, 0);
	}
}

void CAsteroidComponent::LoadSplittedModel()
{
	m_pEntity->LoadGeometry(geometrySlotSplitted, "Assets\\Objects\\AsteroidSplitted\\AsteroidSplitted.cgf");
	/*IStatObj* obj = gEnv->p3DEngine->LoadStatObj("Assets\\Objects\\AsteroidSplitted\\AsteroidSplitted.cgf");
	obj->SetFlags(STATIC_OBJECT_HIDDEN);
	obj->PhysicalizeSubobjects()
	IStatObj* sub = gEnv->p3DEngine->CreateStatObj();
	obj->CopySubObject(0, sub, 1);
	m_pEntity->SetStatObj(sub, geometrySlotSplitted, false);
	*/

	pe_params_structural_joint j;
	
	

	auto *pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("Materials\\AsteroidSplitted");
	m_pEntity->SetMaterial(pMaterial);
	//sub->SetMaterial(pMaterial);
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_ARTICULATED;
	physParams.mass = mass;
	physParams.nSlot = 0;
	physParams.nFlagsOR = pef_ignore_areas | pef_ignore_ocean;
	m_pEntity->Physicalize(physParams);

	pe_simulation_params simParams = {};
	simParams.mass = 500.0f;
	simParams.gravity = Vec3(0.0f, 0.0f, 0.0f);
	m_pEntity->GetPhysics()->SetParams(&simParams, 0);

	int numSubObject = m_pEntity->GetStatObj(geometrySlotSplitted)->GetSubObjectCount(); // 7 parts
	float partMass = physParams.mass / numSubObject;
	IStatObj* compound = m_pEntity->GetStatObj(geometrySlotSplitted);
	//compound->SetFlags(EStaticObjectFlags::STATIC_OBJECT_GENERATED |  STATIC_OBJECT_COMPOUND);
	compound->PhysicalizeSubobjects(m_pEntity->GetPhysicalEntity(), &m_pEntity->GetWorldTM(), partMass);
	

	//pe_action_move_parts pp;
	


	for (int i = 0; i < numSubObject; i++)
	{
		//IStatObj::SSubObject* subObject = compound->GetSubObject(i);
		//pe_params_pos pos;
		//pos.scale = 1.0f;
		//pos.pos = m_pEntity->GetWorldPos() + subObject->localTM.GetTranslation();
		//phys[i] = gEnv->pPhysicalWorld->CreatePhysicalEntity(pe_type::PE_RIGID, &pos);
		//submat[i] = subObject->tm;
		

		//IStatObj::SSubObject* so = m_pEntity->GetStatObj(geometrySlotSplitted)->GetSubObject(i);
		//IPhysicalEntity* subPartPhys = so->pStatObj->GetPhysEntity();
		//if (subPartPhys)
		//{
		//	//1. physicalize each part ? 
		//	//2. make explosion (localy, only for splitted aster) or add impulses for each parts of aster?

		//	//subPartPhys->SetParams(&params);
		//}

	}
}

void CAsteroidComponent::LoadModelSeparatedParts()
{
	auto *pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("Materials\\AsteroidSplitted");
	m_pEntity->SetMaterial(pMaterial);

	// get original speed
	IPhysicalEntity* phys = m_pEntity->GetPhysicalEntity();
	pe_status_dynamics ps;
	phys->GetStatus(&ps);
	Vec3 vel = ps.v;

	// spawn few pices
	for (int32 i = 0; i < 7; i++) 
	{
		string s;
		s = s.Format("%d", i);

		SEntitySpawnParams spawnParams;
		spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
		spawnParams.vPosition = m_pEntity->GetWorldPos();
		spawnParams.qRotation = m_pEntity->GetWorldRotation();
		spawnParams.vScale = m_pEntity->GetScale();
		child[i] = gEnv->pEntitySystem->SpawnEntity(spawnParams);

		//m_pEntity->AttachChild(child);
		child[i]->LoadGeometry(1, "Assets\\Objects\\AsteroidSplitted\\AsteroidSplitted" + s +".cgf");
		child[i]->SetMaterial(pMaterial);

		// physicalize part 
		SEntityPhysicalizeParams physParams;
		physParams.type = PE_RIGID;
		physParams.mass = mass / 10;
		physParams.nSlot = 1; // All
		physParams.nFlagsOR = pef_ignore_areas | pef_ignore_ocean;
		child[i]->Physicalize(physParams);

		// set env properties
		pe_simulation_params simParams = {};
		simParams.mass = 500.0f;
		simParams.gravity = Vec3(0.0f, 0.0f, 0.0f);
		child[i]->GetPhysics()->SetParams(&simParams, 0);

		// set original speed
		pe_action_set_velocity sv;
		sv.v = vel;
		child[i]->GetPhysics()->Action(&sv);
	}
}

void CAsteroidComponent::FreeAllResources()
{
	for (int32 i = 0; i < 7; i++)
	{
		if (child[i])
			gEnv->pEntitySystem->RemoveEntity(child[i]->GetId());
	}
	
}

void CAsteroidComponent::ChangeStateDestroyed()
{
	if (isDestroyed) return;

	LoadModelSeparatedParts();
	m_pEntity->Invisible(true); // still call update event
	lifeTime = 10.0;
	
	isDestroyed = true;
	//gEnv->pEntitySystem->RemoveEntity(GetEntityId());
}
