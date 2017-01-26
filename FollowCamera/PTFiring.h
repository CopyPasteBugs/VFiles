#pragma once

#include "Entities/Helpers/ISimpleExtension.h"

class CPlayerTank;

class CTankFiring
	: public CGameObjectExtensionHelper<CTankFiring, ISimpleExtension>
{
public:
	CTankFiring();
	virtual ~CTankFiring();

	// ISimpleExtension
	virtual void PostInit(IGameObject* pGameObject) override;
	virtual void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	// ~ISimpleExtension

	void RequestFire(const Vec3& worldStartPos, const Quat& worldStartRotation);
	IEntity* GetBulletSpawnEntity() { return fireHelper; };

protected:
	void InitFireHelper();

protected:
	CPlayerTank* pPlayer;

	IEntity* fireHelper;

};