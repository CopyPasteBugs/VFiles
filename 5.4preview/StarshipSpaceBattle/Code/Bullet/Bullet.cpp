#include "StdAfx.h"
#include "Bullet.h"
#include "Asteroid/Asteroid.h"
#include "Hit/Hit.h"

#include <CryMath/Random.h>

void CBulletComponent::ProcessEvent(SEntityEvent & event)
{
	// Handle the OnCollision event, in order to have the entity removed on collision
	if (event.event == ENTITY_EVENT_COLLISION)
	{
		EventPhysCollision *physCollision = reinterpret_cast<EventPhysCollision *>(event.nParam[0]);

		IPhysicalEntity* pOtherEntityPhysics = m_pEntity->GetPhysics() != physCollision->pEntity[0] ? physCollision->pEntity[0] : physCollision->pEntity[1];
		IEntity* pOtherEntity = gEnv->pEntitySystem->GetEntityFromPhysics(pOtherEntityPhysics);

		//IEntity* pOtherEntity = gEnv->pEntitySystem->GetEntityFromPhysics(physCollision->pEntity[1]);

		if (pOtherEntity != nullptr)
		{
			CAsteroidComponent* component = pOtherEntity->GetComponent<CAsteroidComponent>();
			if (component)
			{
				component->ChangeStateDestroyed();
			}
		}

		SpawnHitEffect(physCollision);
		// Queue removal of this entity, unless it has already been done
		gEnv->pEntitySystem->RemoveEntity(GetEntityId());
	}
	else if (event.event == ENTITY_EVENT_UPDATE)
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		if (lifeTime > 5.0f)
		{
			gEnv->pEntitySystem->RemoveEntity(GetEntityId());
			lifeTime = 0.0f;
		}
		lifeTime = lifeTime + pCtx->fFrameTime;

	}
}

void CBulletComponent::SpawnHitEffect(EventPhysCollision * col)
{
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParams.vPosition = GetEntity()->GetPos();
	spawnParams.qRotation = Quat::CreateRotationV0V1(Vec3(0,0,1),col->n);;
	spawnParams.vScale = Vec3Constants<float>::fVec3_One;

	// Spawn the CHit entity
	if (IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		pEntity->CreateComponent<CHit>();
	}
}
