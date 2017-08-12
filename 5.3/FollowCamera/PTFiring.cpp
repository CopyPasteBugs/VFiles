#include "StdAfx.h"
#include "PTFiring.h"

#include <IViewSystem.h>
#include "Entities/Helpers/ISimpleExtension.h"
#include "PlayerTank.h"
#include "PTView.h"

CTankFiring::CTankFiring()
{

}

CTankFiring::~CTankFiring()
{

}

void CTankFiring::PostInit(IGameObject * pGameObject)
{
	pPlayer = static_cast<CPlayerTank*>(pGameObject->QueryExtension("PlayerTank"));
	// Make sure that this extension is updated regularly via the Update function below
	pGameObject->EnableUpdateSlot(this, 0);
	//InitFireHelper();
}

void CTankFiring::Update(SEntityUpdateContext & ctx, int updateSlot)
{

}

void CTankFiring::RequestFire(const Vec3& worldStartPos, const Quat& worldStartRotation)
{
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("TankBullet");

	spawnParams.vPosition = worldStartPos;
	spawnParams.qRotation = worldStartRotation;

	spawnParams.vScale = Vec3(0.2,0.2,0.2);

	// Spawn the entity, bullet is propelled in CBullet based on the rotation and position here
	gEnv->pEntitySystem->SpawnEntity(spawnParams);
}

//void CTankFiring::InitFireHelper()
//{
//	SEntitySpawnParams spawnParamsHelper;
//	spawnParamsHelper.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
//	spawnParamsHelper.vPosition = Vec3(0, 3, 0);
//	spawnParamsHelper.vScale = Vec3(1, 1, 1);
//	spawnParamsHelper.qRotation = IDENTITY;
//	spawnParamsHelper.nFlags = ENTITY_FLAG_CLIENT_ONLY;
//
//	fireHelper = gEnv->pEntitySystem->SpawnEntity(spawnParamsHelper);
//	CRY_ASSERT(fireHelper != nullptr);
//	GetEntity()->AttachChild(fireHelper);
//	fireHelper->SetName("fireHelper");
//
//}
