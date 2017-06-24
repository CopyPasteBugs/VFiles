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
	GetEntity()->LoadGeometry(0, "Assets\\objects\\sphere.cgf");
	Physicalize();
	nBitMask = 3;
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
	
	archive(mySelectedEnum, "MyEnum", "My enum");

	
	if (archive.isInput()) 
	{
		OnResetState();
	}
}

SERIALIZATION_ENUM_BEGIN(MyEnum, "My enum flags")
SERIALIZATION_ENUM(me_Something, "something", "Something")
SERIALIZATION_ENUM(me_Anything, "anything", "Anything")
SERIALIZATION_ENUM(me_Somewhere, "somewhere", "Somewhere")
SERIALIZATION_ENUM_END()
