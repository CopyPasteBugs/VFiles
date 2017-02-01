#include "StdAfx.h"
#include "PlayerFlowNode\PlayerFlowNode.h"

#include "GamePlugin.h"
#include "PlayerTank\PlayerTank.h"

class CPlayerFlowNodeRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{


	}
	virtual void Unregister() override
	{

	}

public:
	CPlayerFlowNodeRegistrator() {}
	~CPlayerFlowNodeRegistrator()
	{

	}
};

CPlayerFlowNodeRegistrator g_playerFlowNodeRegistrator;

REGISTER_FLOW_NODE("entity:PlayerFlowNode", CFlowNode_Player);

CFlowNode_Player::CFlowNode_Player(SActivationInfo* pActInfo)
{
	// This means from now on you will get ProcessEvent calls with eFE_Update event.
	// To be removed again from this list call the same function with false as the second parameter.
	// Frequency: you will get 1 ProcessEvent(eFE_Updated) call per Game update call.
	pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, true);
};

//IFlowNodePtr CFlowNode_Player::Clone(SActivationInfo *pActInfo)
//{
//	return new CFlowNode_Player(pActInfo);
//};

void  CFlowNode_Player::GetMemoryUsage(ICrySizer* pSizer) const
{
	pSizer->AddObject(this, sizeof(*this));
}

void CFlowNode_Player::GetConfiguration(SFlowNodeConfig& config)
{
	// name - Do not use the underscore character '_' in port names

	static const SInputPortConfig in_config[] = {
		InputPortConfig<bool>("EnvironmentEvent", _HELP("Activate if one of the level's enteractive object got a Player near"),0, ""),
		InputPortConfig<EntityId>("EnvironmentEntityId", _HELP("Send to Player the object's Id, for iternal usage"),0, ""),
		InputPortConfig<string>("EnvironmentObjectUsability", _HELP("What this object doing, explanation for player"),0, ""),
		{ 0 }
	};
	static const SOutputPortConfig out_config[] = {
		OutputPortConfig<int>("PlayerPressE", _HELP("Activated only if Player wants to interact with object")),
		{ 0 }
	};

	config.sDescription = _HELP("for levels objects with abilities to Interact with player");
	config.pInputPorts = in_config;
	config.pOutputPorts = out_config;
	config.SetCategory(EFLN_APPROVED);

}

void CFlowNode_Player::ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo)
{
	switch (event)
	{
	// *** Sent once after level has been loaded.
	case eFE_Initialize:
	{
		// This is singletone FlowNode so we try to find our sigle Player(on whole level) to work with it.
		FindPlayerOnLevel();
		break;
	}
	// *** Call per Game update call.
	case  eFE_Update:
	{
		break;
	}
	// *** If one or more input ports have been activated.
	case eFE_Activate:
	{
		if (IsPortActive(pActInfo, eInputPort_EnvironmentEvent))
		{
			TFlowInputData* inputValue = pActInfo->GetInputPort(eInputPort_EnvironmentEvent);
			bool* value = inputValue->GetPtr<bool>();

			// TODO : 
			if (value)
			{
				if (player->GetUseFlag())
				{
					//player->SetUseFlag(false);
					ActivateOutput(pActInfo, eOutputPorts_PlayerPressE, TFlowInputData(true));
				}
			}
		}

		if (IsPortActive(pActInfo, eInputPort_EnvironmentEntityId))
		{
			TFlowInputData* inputValue = pActInfo->GetInputPort(eInputPort_EnvironmentEntityId);
			EntityId* value = inputValue->GetPtr<EntityId>();

			// TODO : 
			if (value)
			{

			}
		}

		if (IsPortActive(pActInfo, eInputPort_EnvironmentObjectUsability))
		{
			TFlowInputData* inputValue = pActInfo->GetInputPort(eInputPort_EnvironmentObjectUsability);
			string* value = inputValue->GetPtr<string>();


			// TODO : 
			if (value)
			{

			}
		}

		break;
	}
	default:
		break;
	};
}

void CFlowNode_Player::FindPlayerOnLevel()
{
	auto *pEntityIterator = gEnv->pEntitySystem->GetEntityIterator();
	pEntityIterator->MoveFirst();

	auto *pSpawnerClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("PlayerTank");

	while (!pEntityIterator->IsEnd())
	{
		IEntity *pEntity = pEntityIterator->Next();

		if (pEntity->GetClass() != pSpawnerClass)
			continue;

		player = pEntity->GetComponent<CPlayerTank>();

		break;
	}
}
