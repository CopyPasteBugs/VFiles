#include "StdAfx.h"
#include "Attack.h"
#include "GamePlugin.h"

#include "Player.h"


CRYREGISTER_CLASS(CAttack)

void CAttack::Initialize()
{
	pHost = GetEntity()->GetComponent<CPlayer>();
}

void CAttack::ProcessEvent(SEntityEvent & event)
{
}

void CAttack::Update(SEntityUpdateContext * param)
{
}

void CAttack::RequestFire(const Vec3 & worldStartPos, const Quat & worldStartRotation)
{
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("TankBullet");

	spawnParams.vPosition = worldStartPos;
	spawnParams.qRotation = worldStartRotation;

	spawnParams.vScale = Vec3(0.2, 0.2, 0.2);

	// Spawn the entity, bullet is propelled in CBullet based on the rotation and position here
	gEnv->pEntitySystem->SpawnEntity(spawnParams);
}
