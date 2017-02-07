#pragma once
#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

class CPlayer;

class CAttack final
	: public IEntityComponent
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CAttack, "Attack", 0xB744C6EBF350418B, 0xBADF3414F33DABB7);

public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return  0; };
	// IEntityComponent

	void Update(SEntityUpdateContext* param);

	void RequestFire(const Vec3& worldStartPos, const Quat& worldStartRotation);

protected:
	CPlayer* pHost;
};