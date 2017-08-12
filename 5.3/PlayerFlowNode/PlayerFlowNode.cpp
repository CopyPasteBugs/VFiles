#include "StdAfx.h"
#include "PlayerFlowNode\PlayerFlowNode.h"

#include "GamePlugin.h"
#include "TPSPlayer\Player.h"
#include "TPSPlayer\Input.h"

REGISTER_FLOW_NODE("entity:PlayerFlowNode", CFlowNode_Player);

CFlowNode_Player::CFlowNode_Player(SActivationInfo* pActInfo)
{
	// This means from now on you will get ProcessEvent calls with eFE_Update event.
	// To be removed again from this list call the same function with false as the second parameter.
	// Frequency: you will get 1 ProcessEvent(eFE_Updated) call per Game update call.
	pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, true);
	fEventTime = 0;
	player = nullptr;
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
		InputPortConfig_Void("EnvironmentEvent", _HELP("Activate if one of the level's enteractive object got a Player near"),0, ""),
		InputPortConfig<EntityId>("EnvironmentEntityId", _HELP("Send to Player the object's Id, for iternal usage"),0, ""),
		InputPortConfig<string>("EnvironmentObjectUsability", _HELP("What this object doing, explanation for player"),0, ""),
		{ 0 }
	};
	static const SOutputPortConfig out_config[] = {
		OutputPortConfig<bool>("PlayerPressE", _HELP("Activated only if Player wants to interact with object"),""),
		OutputPortConfig<EntityId>("PlayerEntityId", _HELP("Store actual player's EntityId"),""),
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
	// *** Call per Game update call.
	case  eFE_Update:
	{

		if (player)
		{
			if (fEventTime < gEnv->pTimer->GetFrameStartTime().GetSeconds())
			{
				// Send void
				TFlowInputData out;
				out.Set<EntityId>(player->GetEntityId());
				out.SetUserFlag(true);

				ActivateOutput(pActInfo, eOutputPorts_PlayerEntityId, out);
				
				
				if (player->GetInput()->GetUseState())
				{
					ActivateOutput(pActInfo, eOutputPorts_PlayerPressE, TFlowInputData(true));
				}

				fEventTime = 1.0f + gEnv->pTimer->GetFrameStartTime().GetSeconds();
			}
		}
		break;
	}

	// *** If one or more input ports have been activated.
	case eFE_Activate:
	{
		if (IsPortActive(pActInfo, eInputPort_EnvironmentEvent))
		{
			if (player->GetInput()->GetUseState())
			{
				ActivateOutput(pActInfo, eOutputPorts_PlayerPressE, TFlowInputData(true));
				//ActivateOutput(pActInfo, eOutputPorts_PlayerEntityId, TFlowInputData(player->GetEntityId()));
			}			
		}

		if (IsPortActive(pActInfo, eInputPort_EnvironmentEntityId))
		{
			TFlowInputData* inputValue = pActInfo->GetInputPort(eInputPort_EnvironmentEntityId);
			EntityId* value = inputValue->GetPtr<EntityId>();

			

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
	// *** Sent once after level has been loaded.
	case eFE_Initialize:
	{
		// This is singletone FlowNode so we try to find our sigle Player(on whole level) to work with it.
		FindPlayerOnLevel();

		// Also push to player's fgNode some initial values
		//ActivateOutput(pActInfo, eOutputPorts_PlayerPressE, TFlowInputData(false));
		//if (player)
		//	ActivateOutput(pActInfo, eOutputPorts_PlayerEntityId, TFlowInputData(player->GetEntityId()));

		pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, true);

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

	auto *pSpawnerClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Player");

	while (!pEntityIterator->IsEnd())
	{
		IEntity *pEntity = pEntityIterator->Next();

		if (pEntity->GetClass() != pSpawnerClass)
			continue;

		player = pEntity->GetComponent<CPlayer>();

		break;
	}
}
