#include "StdAfx.h"
#include "MyAreaTrigger.h"

#include "GamePlugin.h"

#include "Entities\Helpers\ISimpleExtension.h"
#include <CryEntitySystem/IEntitySystem.h>
#include <CryRenderer\IRenderer.h>
#include <CryRenderer\IRenderAuxGeom.h>

#include "Entities\Helpers\EntityFlowNode.h"

class CAreaTriggerRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CMyAreaTriggerEntity>("MyAreaTriggerEntity", "Default", "Trigger.bmp");
		
		CEntityFlowNodeFactory* pFlowNodeFactory = new CEntityFlowNodeFactory("entity:MyAreaTriggerEntity");


		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Active", ""));
		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Enable", ""));
		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Disable", ""));
		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Manual", ""));


		pFlowNodeFactory->m_activateCallback = CMyAreaTriggerEntity::OnFlowgraphActivation;
		
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<bool>("Disabled"));
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<bool>("Enabled"));
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<bool>("Enter"));
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<bool>("Leave"));

		pFlowNodeFactory->Close();
	}
	virtual void Unregister() override
	{

	}

public:
	CAreaTriggerRegistrator() {}
	~CAreaTriggerRegistrator()
	{

	}
};

static CAreaTriggerRegistrator g_areaRegistrator;
CRYREGISTER_CLASS(CMyAreaTriggerEntity);

CMyAreaTriggerEntity::CMyAreaTriggerEntity() 
	: m_bActive(true)
	, m_fSize(1.0f)
{

}

void CMyAreaTriggerEntity::Initialize()
{
	// Active for Update mask working
	GetEntity()->Activate(true);
	GetEntity()->SetFlags(ENTITY_PROXY_TRIGGER);
}

void CMyAreaTriggerEntity::OnShutDown()
{
}

void CMyAreaTriggerEntity::ProcessEvent(SEntityEvent& event)
{
	if (gEnv->IsDedicated())
		return;

	switch (event.event)
	{
		// Editor specific, physicalize on reset, property change or transform change
	case ENTITY_EVENT_RESET:
	case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
	case ENTITY_EVENT_XFORM_FINISHED_EDITOR:
		OnResetState();
		break;
	case ENTITY_EVENT_ENTERAREA:
	{
		if (!m_bActive)
			return;

		IEntity * pEntity = gEnv->pEntitySystem->GetEntity((EntityId)event.nParam[0]);		
		guest = pEntity;
		ActivateFlowNodeOutput(eOutputPorts_Enter, TFlowInputData((EntityId)event.nParam[0]));
		gEnv->pLog->Log("%s: - entity entered to MyAreaTrigger", pEntity->GetName());	
		break;
	}
	case ENTITY_EVENT_LEAVEAREA:
	{
		if (!m_bActive)
			return;

		IEntity * pEntity = gEnv->pEntitySystem->GetEntity((EntityId)event.nParam[0]);

		TFlowInputData data;
		data.Set<EntityId>((EntityId)event.nParam[0]);
		data.SetUserFlag(true);
		ActivateFlowNodeOutput(eOutputPorts_Leave, data);
		gEnv->pLog->Log("%s: - entity leave MyAreaTrigger", pEntity->GetName());

		break;
	}
	case ENTITY_EVENT_UPDATE:
	{
		SEntityUpdateContext* param = (SEntityUpdateContext*)event.nParam[0];
		Update(*param);
		break;
	}
	case ENTITY_EVENT_START_LEVEL:
	{
		//Reset for game.exe mode because ENTITY_EVENT_RESET not work in game.exe 
		if (!gEnv->IsEditor())
		{
			OnResetState();
		}
	}
	}
}

void CMyAreaTriggerEntity::Update(SEntityUpdateContext & ctx)
{
	//static IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
	//debug->Begin("MyAreaTrigger", false);
	//AABB bb;
	//GetEntity()->GetWorldBounds(bb);
	//debug->AddAABB(bb.min, bb.max, ColorF(1, 1, 1), 1);

	AABB aabb;
	GetEntity()->GetLocalBounds(aabb);

	OBB obb(OBB::CreateOBBfromAABB(Matrix33(GetEntity()->GetWorldTM()), aabb));

	

	//Overlap::Lineseg_OBB(lineseg, GetEntity()->GetWorldPos(), obb)

	gEnv->pRenderer->GetIRenderAuxGeom()->DrawOBB(obb, GetEntity()->GetWorldTM(), false, ColorB(255, 255, 0), EBoundingBoxDrawStyle::eBBD_Faceted);

}

void CMyAreaTriggerEntity::ActivateFlowNodeOutput(const int portIndex, const TFlowInputData & inputData)
{
	SEntityEvent evnt;
	evnt.event = ENTITY_EVENT_ACTIVATE_FLOW_NODE_OUTPUT;
	evnt.nParam[0] = portIndex;
	evnt.nParam[1] = (INT_PTR)&inputData;
	GetEntity()->SendEvent(evnt);
}

void CMyAreaTriggerEntity::OnFlowgraphActivation(EntityId entityId, IFlowNode::SActivationInfo * pActInfo, const CEntityFlowNode * pNode)
{
	auto* pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	auto* pTriggerEntity = pEntity->GetComponent<CMyAreaTriggerEntity>();

	if (IsPortActive(pActInfo, eInputPorts_Enable))
	{
		pTriggerEntity->m_bActive = GetPortBool(pActInfo, eInputPorts_Enable);

		if (pTriggerEntity->m_bActive)
		{
			pTriggerEntity->ActivateFlowNodeOutput(eOutputPorts_Enabled, TFlowInputData());
		}
		else
		{
			pTriggerEntity->ActivateFlowNodeOutput(eOutputPorts_Disabled, TFlowInputData());
		}
	}
	else if (IsPortActive(pActInfo, eInputPorts_Disable))
	{
		pTriggerEntity->m_bActive = false;

		pTriggerEntity->ActivateFlowNodeOutput(eOutputPorts_Disabled, TFlowInputData());
	}
}

void CMyAreaTriggerEntity::OnResetState()
{
	IEntity &entity = *GetEntity();

	//IEntityTriggerComponent *pTriggerProxy = (IEntityTriggerComponent*)(GetEntity()->GetProxy(ENTITY_PROXY_TRIGGER));
	IEntityTriggerComponent *pTriggerProxy = GetEntity()->GetOrCreateComponent<IEntityTriggerComponent>();


	if (!pTriggerProxy)
	{
		//GetEntity()->CreateProxy(ENTITY_PROXY_TRIGGER);
		pTriggerProxy = GetEntity()->GetOrCreateComponent<IEntityTriggerComponent>();
	}

	if (pTriggerProxy)
	{
		float radius = m_fSize / 2.0f;
		AABB boundingBox = AABB(Vec3(-radius, -radius, -radius), Vec3(radius, radius, radius));
		pTriggerProxy->SetTriggerBounds(boundingBox);

	}
	else
	{
		gEnv->pLog->Log("%s: Warning: MyAreaTrigger Has Bad Params", GetEntity()->GetName());
	}
}
