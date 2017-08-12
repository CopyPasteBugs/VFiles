#include "StdAfx.h"
#include "MyFlowNode\MyFlowNode.h"

#include "GamePlugin.h"

REGISTER_FLOW_NODE("entity:MyFlowNode", CFlowNode_MyName);

CFlowNode_MyName::CFlowNode_MyName(SActivationInfo* pActInfo)
{
	// This means from now on you will get ProcessEvent calls with eFE_Update event.
	// To be removed again from this list call the same function with false as the second parameter.
	// Frequency: you will get 1 ProcessEvent(eFE_Updated) call per Game update call.
	pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, true);
};

IFlowNodePtr CFlowNode_MyName::Clone(SActivationInfo *pActInfo)
{
	return new CFlowNode_MyName(pActInfo);
};

void  CFlowNode_MyName::GetMemoryUsage(ICrySizer* pSizer) const
{
	pSizer->AddObject(this, sizeof(*this));
}

void CFlowNode_MyName::GetConfiguration(SFlowNodeConfig& config)
{
	// name - Do not use the underscore character '_' in port names

	static const SInputPortConfig in_config[] = {
		InputPortConfig<int>("someInput", _HELP("useful help text"),0, _UICONFIG("v_min=0, v_max=10")),
		InputPortConfig<int>("someInput2", _HELP("useful help text2"),0, _UICONFIG("enum_int:Relaxed=0,Alert=1,Combat=2,Crouch=3")),
		InputPortConfig<string>("someInput2", _HELP("useful help text2"),0, _UICONFIG("enum_string:antohska,kartosha,dolbaejka")),
		{ 0 }
	};
	static const SOutputPortConfig out_config[] = {
		OutputPortConfig<int>("alertness", _HELP("useful help text")),
		{ 0 }
	};

	config.sDescription = _HELP("A description of this flow node");
	config.pInputPorts = in_config;
	config.pOutputPorts = out_config;
	config.SetCategory(EFLN_APPROVED);
	
}

void CFlowNode_MyName::ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo)
{
	switch (event)
	{
	// *** Call per Game update call.
	case  eFE_Update: 
	{
		static int g = 0;
		
		// Send event to FG output port 
		TFlowInputData output;
		output.Set<int>(g++);
		output.SetUserFlag(true);
		ActivateOutput(pActInfo, eOutputPorts_Alertness, output);
		break;
	}
	// *** If one or more input ports have been activated.
	case eFE_Activate:
	{
		if (IsPortActive(pActInfo, eInputPort_SomeInput0))
		{
			TFlowInputData* inputValue = pActInfo->GetInputPort(eInputPort_SomeInput0);
			int* value = inputValue->GetPtr<int>();

			// TODO : 
			if (value)
			{
				// increment and return
				TFlowInputData output;
				output.Set<int>(*value++);
				output.SetUserFlag(true);
				ActivateOutput(pActInfo, eOutputPorts_Alertness, output);
			}
		} 
		else if (IsPortActive(pActInfo, eInputPort_SomeInput1))
		{
			TFlowInputData* inputValue = pActInfo->GetInputPort(eInputPort_SomeInput1);
			int* value = inputValue->GetPtr<int>();

			// TODO : 
			if (value)
			{
				// multiply by 2 and return
				TFlowInputData output;
				output.Set<int>((*value) * 2);
				output.SetUserFlag(true);
				ActivateOutput(pActInfo, eOutputPorts_Alertness, output);
			}
		}
		else if (IsPortActive(pActInfo, eInputPort_SomeInput2))
		{
			TFlowInputData* inputValue = pActInfo->GetInputPort(eInputPort_SomeInput2);
			string* value = inputValue->GetPtr<string>();
			

			// TODO : 
			if (value)
			{
				// return selected string len
				TFlowInputData output;
				output.Set<int>(value->length());
				output.SetUserFlag(true);
				ActivateOutput(pActInfo, eOutputPorts_Alertness, output);
			}
		}

		break;
	}
	default:
		break;
	};
}