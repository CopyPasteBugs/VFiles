#pragma once

#include "Entities\Helpers\DesignerEntityComponent.h"

#include <CrySerialization/Decorators/Resources.h>

class CMyGeomEntity final 
	: public CDesignerEntityComponent<IGeometryEntityComponent>
	, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_CLASS(CMyGeomEntity, IGeometryEntityComponent, "MyGeomEntity", 0x5704E240673E49C5, 0xB3BD33AC6649274A);

	virtual ~CMyGeomEntity() {}

public:
	enum EInputPorts
	{
		eInputPort_Hide = 0,
		eInputPort_UnHide,
		eInputPort_Geometry
	};

	enum EOutputPorts
	{
		eOutputPort_OnHide = 0,
		eOutputPort_OnUnHide,
		eOutputPort_OnCollision,
		eOutputPort_CollisionSurfaceName,
		eOutputPort_OnGeometryChanged
	};

	enum EPhysicalizationType
	{
		ePhysicalizationType_None = 0,
		ePhysicalizationType_Static,
		ePhysicalizationType_Rigid
	};

public:
	// ISimpleExtension
	virtual void Initialize() final;

	virtual void ProcessEvent(SEntityEvent& event) final;
	virtual uint64 GetEventMask() const final { return CDesignerEntityComponent::GetEventMask() | BIT64(ENTITY_EVENT_COLLISION) | BIT64(ENTITY_EVENT_HIDE) | BIT64(ENTITY_EVENT_UNHIDE) | BIT64(ENTITY_EVENT_RESET); }

	virtual void OnResetState() final;

	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	// ~ISimpleExtension

	// IGeometryEntityComponent
	virtual void SetGeometry(const char* szFilePath) override;
	// IGeometryEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "Geometry Properties"; }

	virtual void SerializeProperties(Serialization::IArchive& archive) override
	{
		archive(Serialization::ModelFilename(m_model), "Geometry", "Geometry");
		archive(m_physicalizationType, "Physicalize", "Physicalize");
		archive(m_bReceiveCollisionEvents, "ReceiveCollisionEvents", "ReceiveCollisionEvents");
		archive(m_mass, "Mass", "Mass");

		if (archive.openBlock("Animations", "Animations"))
		{
			
			archive(Serialization::GeneralFilename(m_animation), "Animation", "Animation");
			archive(m_animationSpeed, "Speed", "Speed");
			archive(m_bLoopAnimation, "Loop", "Loop");

			archive.closeBlock();
		}

		if (archive.isInput())
		{
			OnResetState();
		}
	}
	// ~IEntityPropertyGroup

public:
	static void OnFlowgraphActivation(EntityId entityId, IFlowNode::SActivationInfo* pActInfo, const class CEntityFlowNode* pNode);

protected:
	IPhysicalEntity* m_pPhysEnt;

	string m_model;

	EPhysicalizationType m_physicalizationType = ePhysicalizationType_Static;
	float m_mass = 1.f;

	string m_animation;
	float m_animationSpeed = 1.f;
	bool m_bLoopAnimation = true;

	bool m_bReceiveCollisionEvents = false;
};
