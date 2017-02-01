#include "StdAfx.h"
#include "GamePlugin.h"
#include "HitFXEntity.h"

#include <Cry3DEngine/I3DEngine.h>
#include <CryParticleSystem/IParticlesPfx2.h>

class CHitFXEntityRegistrator
	: public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CHitFXEntity>("HitFXEntity", "Defaults", "Particles.bmp");
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

CRYREGISTER_CLASS(CHitFXEntity)

void CHitFXEntity::Initialize()
{
	OnResetState();
	fKillTime = gEnv->pTimer->GetFrameStartTime().GetSeconds() + 0.5f;
	GetEntity()->Activate(true);
}

void CHitFXEntity::ProcessEvent(SEntityEvent & event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_UPDATE:
		{
			if (fKillTime < gEnv->pTimer->GetFrameStartTime().GetSeconds())
			{
				GetEntity()->FreeSlot(1);
				//gEnv->pParticleManager->DeleteEmitter(GetEntity()->GetParticleEmitter(0));
				gEnv->pEntitySystem->RemoveEntity(GetEntityId());
			}
			break;
		}
	}
}

void CHitFXEntity::OnResetState()
{
	const char* m_particleEffectPath = "particles/hit.pfx";

	if (IParticleEffect* pEffect = gEnv->pParticleManager->FindEffect(m_particleEffectPath))
	{
		GetEntity()->LoadParticleEmitter(1, pEffect);
	}
}

void CHitFXEntity::SerializeProperties(Serialization::IArchive & archive)
{

}
