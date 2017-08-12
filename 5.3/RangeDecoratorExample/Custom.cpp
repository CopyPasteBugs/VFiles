#include "StdAfx.h"
#include "GamePlugin.h"
#include "Custom.h"

#include <CrySerialization/Decorators/Range.h>
#include <CrySerialization/Decorators/BitFlags.h>


class CHitFXEntityRegistrator
	: public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CCustom>("CCustom", "Defaults", "Particles.bmp");
	}
	virtual void Unregister() override
	{

	}
public:
	CHitFXEntityRegistrator() {}
	~CHitFXEntityRegistrator()
	{
	}
};

static CHitFXEntityRegistrator g_hitFXEntityRegistrator;

CRYREGISTER_CLASS(CCustom)

void CCustom::Initialize()
{
	OnResetState();
	GetEntity()->Activate(true);
}

void CCustom::ProcessEvent(SEntityEvent & event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_UPDATE:
	{
		
	}
	}
}

void CCustom::OnResetState()
{

	GetEntity()->FreeSlot(0);

	switch (mySelectedGeom)
	{
	case mgm_primitive_box:
		GetEntity()->LoadGeometry(0, "Assets\\objects\\primitive_box.cgf");
		break;
	
	case mgm_primitive_cylinder:
		GetEntity()->LoadGeometry(0, "Assets\\objects\\primitive_cylinder.cgf");
		break;

	case mgm_primitive_sphere:
		GetEntity()->LoadGeometry(0, "Assets\\objects\\primitive_sphere.cgf");
		break;

	default:
		break;
	}
	
	Physicalize();
}

void CCustom::Physicalize()
{
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_RIGID;
	physParams.mass = (float)nMass;
	

	GetEntity()->Physicalize(physParams);
}

void CCustom::SerializeProperties(Serialization::IArchive & archive)
{
	archive(Serialization::Range(nMass, 0, 100, 5), "MyMass", "My mass");
	
	archive(mySelectedGeom, "MyGeom", "My geometry models");
	
	if (archive.isInput()) 
	{
		OnResetState();
	}
}

SERIALIZATION_ENUM_BEGIN(MyGeomModels, "My geometry models")
SERIALIZATION_ENUM(mgm_primitive_box, "primitivebox", "primitive_box")
SERIALIZATION_ENUM(mgm_primitive_cylinder, "primitivecylinder", "primitive_cylinder")
SERIALIZATION_ENUM(mgm_primitive_sphere, "primitivesphere", "primitive_sphere")
SERIALIZATION_ENUM_END()
