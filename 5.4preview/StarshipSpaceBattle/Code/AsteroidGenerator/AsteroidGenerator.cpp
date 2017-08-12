#include "StdAfx.h"
#include "AsteroidGenerator.h"
#include "Asteroid/Asteroid.h"
#include <CryMath/Random.h>

void CAsteroidGenerator::Register(Schematyc::CEnvRegistrationScope & componentScope)
{
}

void CAsteroidGenerator::ReflectType(Schematyc::CTypeDesc<CAsteroidGenerator>& desc)
{
	desc.SetGUID(CAsteroidGenerator::IID());
	desc.SetEditorCategory("User");
	desc.SetLabel("AsteroidGenerator");
	desc.SetDescription("Asteroid generator component");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });

	//desc.AddMember(&CAsteroidGenerator::asteroidMax, 0, "asteroidMax", "asteroidMax", "description 1", 20);
	//desc.AddMember(&CAsteroidGenerator::emittPeriod, 1, "emittPeriod", "emittPeriod", "description 2", 0.5f);
	//desc.AddMember(&CAsteroidGenerator::emittRect, 2, "emittRect", "emittRect", "description 3", Vec2(20.0f,0.0f));
}

void CAsteroidGenerator::Initialize()
{
	emittOrigin = GetEntity()->GetPos();
}

void CAsteroidGenerator::ProcessEvent(SEntityEvent & event)
{
	if (event.event == ENTITY_EVENT_UPDATE)
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		static float elapsed = 0.0f;

		if (elapsed > emittPeriod) 
		{
			elapsed = 0.0f;
			RepositionGeneratorOrigin();
			EmittAsteroid();

		}

		elapsed += pCtx->fFrameTime;

	}
}

void CAsteroidGenerator::EmittAsteroid()
{
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();

	spawnParams.vPosition = GetEntity()->GetPos();
	spawnParams.qRotation = GetEntity()->GetRotation();
	spawnParams.vScale = cry_random(Vec3(0.5, 0.5, 0.5), Vec3(2.0f, 2.0f, 2.0f));

	// Spawn the entity
	if (IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		// See Bullet.cpp, bullet is propelled in  the rotation and position the entity was spawned with
		pEntity->CreateComponent<CAsteroidComponent>();
	}
}

void CAsteroidGenerator::RepositionGeneratorOrigin()
{
	//Vec3 min = emittOrigin - Vec3(emittRect.x, 0, emittRect.y);
	//Vec3 max = emittOrigin + Vec3(emittRect.x, 0, emittRect.y);

	//Vec3 newpos = cry_random_componentwise(min, max);

	float halfSize = emittRect.x * 0.5f;
	float horizontOffset = cry_random(-halfSize, halfSize);

	GetEntity()->SetPos(Vec3(emittOrigin.x + horizontOffset,emittOrigin.y, emittOrigin.z));
	GetEntity()->SetRotation(IDENTITY);
}

