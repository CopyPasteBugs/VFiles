#pragma once

#include "Entities/Helpers/ISimpleExtension.h"

#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

class CGameEntityNodeFactory;

class CMyAreaTriggerEntity final
	: public IEntityComponent
	, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CMyAreaTriggerEntity, "MyAreaTriggerEntity", 0xDE425219F8674496, 0x8040F25C6FF6C9C3);

	CMyAreaTriggerEntity();
	virtual ~CMyAreaTriggerEntity() {};
public:
	enum EFlowgraphInputPorts
	{
		eInputPorts_Enable = 0,
		eInputPorts_Disable,
		eInputPorts_Manual
	};

	enum EFlowgraphOutputPorts
	{
		eOutputPorts_Disabled = 0,
		eOutputPorts_Enabled,
		eOutputPorts_Enter,
		eOutputPorts_Leave,
		eOutputPorts_GuestEntityId,
		eOutputPorts_AreaEntityId,
	};

public:

	// IEntityComponent
	virtual void Initialize() override;
	virtual void OnShutDown() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_ENTERAREA) | BIT64(ENTITY_EVENT_LEAVEAREA) | BIT64(ENTITY_EVENT_START_LEVEL)
													| BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_EDITOR_PROPERTY_CHANGED) | BIT64(ENTITY_EVENT_XFORM_FINISHED_EDITOR) | BIT64(ENTITY_EVENT_ACTIVATE_FLOW_NODE_OUTPUT); }
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	// IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "MyAreTriggerEntity Properties"; }
	virtual void SerializeProperties(Serialization::IArchive& archive) override
	{
		archive(m_bActive, "Active", "Active");
		archive(m_fSize, "Size", "Size");

		if (archive.isInput())
		{
			OnResetState();
		}
	}
	// ~IEntityPropertyGroup

	void ActivateFlowNodeOutput(const int portIndex, const TFlowInputData& inputData);

	static void OnFlowgraphActivation(EntityId entityId, IFlowNode::SActivationInfo* pActInfo, const class CEntityFlowNode* pNode);
	void CMyAreaTriggerEntity::Update(SEntityUpdateContext & ctx);

	IEntity* guest;
protected:
	void OnResetState();

protected:
	bool m_bActive;
	float m_fSize;
};
