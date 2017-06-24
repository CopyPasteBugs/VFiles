#include "StdAfx.h"
#include "GamePlugin.h"
#include "Custom.h"

#include <CrySerialization/Decorators/Range.h>

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

	if (archive.isInput()) 
	{
		OnResetState();
	}
}
