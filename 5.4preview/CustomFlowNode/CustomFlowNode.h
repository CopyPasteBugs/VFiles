#pragma once
#include <CryFlowGraph\IFlowBaseNode.h>

class CFlowNode_MyName final : public CFlowBaseNode<eNCT_Instanced>
{
public:
	enum EInputPorts
	{
		eInputPort_SomeInput0 = 0,
		eInputPort_SomeInput1,
		eInputPort_SomeInput2,
		eInputPort_Num
	};

	enum EOutputPorts
	{
		eOutputPorts_Alertness = 0,
		eOutputPorts_Num
	};
public:
	CFlowNode_MyName(SActivationInfo* pActInfo);

	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo) override;
	virtual void GetMemoryUsage(ICrySizer* s) const override;
	virtual void GetConfiguration(SFlowNodeConfig& config) override;
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) override;
};