#pragma once
#include <CryFlowGraph\IFlowBaseNode.h>

class CPlayer;

class CFlowNode_Player final : public CFlowBaseNode<eNCT_Singleton>
{
public:
	enum EInputPorts
	{
		eInputPort_EnvironmentEvent= 0,
		eInputPort_EnvironmentEntityId,
		eInputPort_EnvironmentObjectUsability,
		eInputPort_Num
	};

	enum EOutputPorts
	{
		eOutputPorts_PlayerPressE = 0,
		eOutputPorts_PlayerEntityId,
		eOutputPorts_Num
	};
public:
	CFlowNode_Player(SActivationInfo* pActInfo);
	// *** We don't need Clone() bacause this flownode have only one instance on whole level
	//virtual IFlowNodePtr Clone(SActivationInfo *pActInfo) override;
	virtual void GetMemoryUsage(ICrySizer* s) const override;
	virtual void GetConfiguration(SFlowNodeConfig& config) override;
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) override;

	void FindPlayerOnLevel();

	CPlayer* player;

	float fEventTime;
	

};