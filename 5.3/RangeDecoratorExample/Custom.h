#pragma once
#include "StdAfx.h"

#include "GamePlugin.h"

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

enum MyGeomModels 
{	
	mgm_primitive_box = BIT(1),
	mgm_primitive_cylinder = BIT(2),
	mgm_primitive_sphere = BIT(3)
};

class CCustom
	: public IEntityComponent
	, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CCustom, "CustomEntity", 0x6E67315952564F7A, 0xA4CBA2A368EC01F9);

public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_UPDATE); }
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	// IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "CustomEntity"; };
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	
	// IEntityPropertyGroup

	void OnResetState();

protected:
	void Physicalize();
	int nMass;
	int nBitMask;
	MyGeomModels mySelectedGeom;
};