#include "StdAfx.h"
#include "Hit.h"
#include <CryMath/Random.h>

void CHit::Register(Schematyc::CEnvRegistrationScope & componentScope)
{

}

void CHit::ReflectType(Schematyc::CTypeDesc<CHit>& desc)
{
	desc.SetGUID(CHit::IID());
	desc.SetEditorCategory("User");
	desc.SetLabel("Hit");
	desc.SetDescription("A component ");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::ClientOnly });

}

CHit::~CHit()
{

}

void CHit::Initialize()
{
	fKillTime = gEnv->pTimer->GetFrameStartTime().GetSeconds() + 1.0f;

	const char* m_particleEffectPath = "Assets/hit1.pfx";

	if (IParticleEffect* pEffect = gEnv->pParticleManager->FindEffect(m_particleEffectPath))
	{
		m_pEntity->LoadParticleEmitter(1, pEffect);
	}

	// Make sure that bullets are always rendered regardless of distance
	// Ratio is 0 - 255, 255 being 100% visibility
	GetEntity()->SetViewDistRatio(255);
	
}

uint64 CHit::GetEventMask() const
{
	return BIT64(ENTITY_EVENT_UPDATE);
}

void CHit::ProcessEvent(SEntityEvent & event)
{

	if (event.event == ENTITY_EVENT_UPDATE)
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		if (fKillTime < gEnv->pTimer->GetFrameStartTime().GetSeconds())
		{
			GetEntity()->FreeSlot(1);
			//gEnv->pParticleManager->DeleteEmitter(GetEntity()->GetParticleEmitter(0));
			gEnv->pEntitySystem->RemoveEntity(GetEntityId());
		}

	}
}