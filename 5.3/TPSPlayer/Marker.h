#pragma once
#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

class CPlayer;

class CMarker final
	: public IEntityComponent
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CMarker, "Marker", 0x394CB2C991A04096, 0xBFA9C5D0D2E88BD6);

public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return 0; };
	// IEntityComponent

	void Update(SEntityUpdateContext* param);

protected:
	CPlayer* pHost;

};