#include "StdAfx.h"
#include "MyAreaTrigger.h"

#include "GamePlugin.h"

#include "Entities\Helpers\ISimpleExtension.h"
#include <CryEntitySystem/IEntitySystem.h>
#include <CryRenderer\IRenderer.h>
#include <CryRenderer\IRenderAuxGeom.h>
#include <CryEntitySystem\IEntity.h>

#include "Entities\Helpers\EntityFlowNode.h"

#include "PlayerTank\PlayerTank.h"

class CAreaTriggerRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CMyAreaTriggerEntity>("MyAreaTriggerEntity", "Default", "Trigger.bmp");
		
		CEntityFlowNodeFactory* pFlowNodeFactory = new CEntityFlowNodeFactory("entity:MyAreaTriggerEntity");

		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Active", ""));
		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Enable", ""));
		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Disable", ""));
		pFlowNodeFactory->m_inputs.push_back(InputPortConfig<EntityId>("WatchThisEntityId", ""));


		pFlowNodeFactory->m_activateCallback = CMyAreaTriggerEntity::OnFlowgraphActivation;
		
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<bool>("Disabled"));
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<bool>("Enabled"));
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<bool>("Enter"));
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<bool>("Leave"));
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<EntityId>("GuestEntityId"));
		pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<EntityId>("AreaEntityId"));
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
	, nEntityId(0)
{

}

void CMyAreaTriggerEntity::Initialize()
{
	// Active for Update mask working
	GetEntity()->Activate(true);
	GetEntity()->SetFlags(ENTITY_FLAG_CLIENT_ONLY);
	OnResetState();
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

		EntityId id = (EntityId)event.nParam[0];
		//EntityId areaId = (EntityId)event.nParam[1];
		//EntityId EntityOfArea = (EntityId)event.nParam[2];

		IEntity * pEntity = gEnv->pEntitySystem->GetEntity((EntityId)event.nParam[0]);
		//auto *pEntityClass = pEntity->GetClass();

		//IEntity * pAreaEntity = gEnv->pEntitySystem->GetEntity(areaId);
		//IEntity * pEntityOfAreaEntity = gEnv->pEntitySystem->GetEntity(EntityOfArea);


		guest = pEntity;
		
		// *** if only specific entity check
		IEntityClass *pPlayerPrefabClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Player");
		if (pEntity->GetClass() == pPlayerPrefabClass)
		{
			ActivateFlowNodeOutput(eOutputPorts_Enter, TFlowInputData(true));
			gEnv->pLog->Log("%s: Player Enter!!!", pEntity->GetName());
		}
		
		//if (pEntity)
		//{
		//	uint16 extensionId = gEnv->pGameFramework->GetIGameObjectSystem()->GetID("TankMarker");
		//	IGameObject *pGameObject = gEnv->pGameFramework->GetGameObject((EntityId)event.nParam[0]);
		//	CTankMarker *pMarker = nullptr;
		//	if (pGameObject)
		//	{
		//		pMarker = static_cast<CTankMarker*>(pGameObject->QueryExtension(extensionId));
		//		if (pMarker == nullptr)
		//		{
		//			ActivateFlowNodeOutput(eOutputPorts_Enter, TFlowInputData(true));
		//		}
		//	}
		//}


		ActivateFlowNodeOutput(eOutputPorts_GuestEntityId, TFlowInputData(id));
		ActivateFlowNodeOutput(eOutputPorts_AreaEntityId, TFlowInputData(GetEntityId()));
		gEnv->pLog->Log("entity enter name= %s id= %d class= %s:", pEntity->GetName(),pEntity->GetId(), pEntity->GetClass()->GetName());
		
		//if (pAreaEntity)
		//	gEnv->pLog->Log("%s: - areaId", pAreaEntity->GetName());
		//
		//if (pEntityOfAreaEntity)
		//	gEnv->pLog->Log("%s: - entity Of Area", pEntityOfAreaEntity->GetName());

		break;
	}
	case ENTITY_EVENT_LEAVEAREA:
	{
		if (!m_bActive)
			return;

		EntityId id = (EntityId)event.nParam[0];
		IEntity * pEntity = gEnv->pEntitySystem->GetEntity(id);

		// *** if only specific entity check
		IEntityClass *pPlayerPrefabClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Player");
		if (pEntity->GetClass() == pPlayerPrefabClass)
		{
			ActivateFlowNodeOutput(eOutputPorts_Leave, TFlowInputData(true));
			gEnv->pLog->Log("%s: Player Enter!!!", pEntity->GetName());
		}



		ActivateFlowNodeOutput(eOutputPorts_GuestEntityId, TFlowInputData(id));
		ActivateFlowNodeOutput(eOutputPorts_AreaEntityId, TFlowInputData(GetEntityId()));
		gEnv->pLog->Log("entity leave name= %s id= %d class= %s:", pEntity->GetName(), pEntity->GetId(), pEntity->GetClass()->GetName());

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
	//debug->Begin("MyAreaTrigger", true);
	//AABB bb;
	//GetEntity()->GetWorldBounds(bb);
	//debug->AddAABB(bb.min, bb.max, ColorF(1, 1, 1), 0);

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

	if (IsPortActive(pActInfo, eInputPorts_Disable))
	{
		pTriggerEntity->m_bActive = false;

		pTriggerEntity->ActivateFlowNodeOutput(eOutputPorts_Disabled, TFlowInputData());
	}

	if (IsPortActive(pActInfo, eInputPorts_WatchThisEntityId))
	{
		pTriggerEntity->nEntityId = GetPortEntityId(pActInfo, eInputPorts_WatchThisEntityId);
	}
}

void CMyAreaTriggerEntity::OnResetState()
{
	
	//IEntityTriggerComponent *pTriggerProxy = (IEntityTriggerComponent*)(GetEntity()->GetProxy(ENTITY_PROXY_TRIGGER));
	IEntityTriggerComponent *pTriggerProxy = GetEntity()->GetOrCreateComponent<IEntityTriggerComponent>();

	float radius = m_fSize / 2.0f;
	AABB boundingBox = AABB(Vec3(-radius, -radius, -radius), Vec3(radius, radius, radius));

	//if (!pTriggerProxy) 
	//{
	//	pTriggerProxy = (IEntityTriggerComponent*)(GetEntity()->CreateProxy(ENTITY_PROXY_TRIGGER));
	//}

	if (pTriggerProxy)
	{
		pTriggerProxy->SetTriggerBounds(boundingBox);
	}
	else
	{
		gEnv->pLog->Log("%s: Warning: MyAreaTrigger Has Bad Params", GetEntity()->GetName());
	}

	//SEntityPhysicalizeParams::AreaDefinition ad;
	//ad.areaType = SEntityPhysicalizeParams::AreaDefinition::EAreaType::AREA_BOX;
	//ad.center = GetEntity()->GetWorldPos();
	//ad.boxmin = boundingBox.min;
	//ad.boxmax = boundingBox.max;
	//ad.axis = Vec3(0,0,0);

	//SEntityPhysicalizeParams physParams;
	//physParams.type = PE_AREA;
	//physParams.pAreaDef = &ad;
	//physParams.nSlot = -1;
	//physParams.mass = 0.0f;
	//

	//GetEntity()->Physicalize(physParams);


	
}
