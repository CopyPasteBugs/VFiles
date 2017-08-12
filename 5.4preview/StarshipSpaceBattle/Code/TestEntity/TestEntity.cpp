#include "StdAfx.h"
#include "TestEntity/TestEntity.h"
#include <CryMath/Random.h>



void CTestEntity::Register(Schematyc::CEnvRegistrationScope & componentScope)
{

}

void CTestEntity::ReflectType(Schematyc::CTypeDesc<CTestEntity>& desc)
{
	desc.SetGUID(CTestEntity::IID());
	desc.SetEditorCategory("User");
	desc.SetLabel("TestEntity");
	desc.SetDescription("A component ");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::ClientOnly, IEntityComponent::EFlags::SchematycEditable });
	desc.AddMember(&CTestEntity::scale, 0, "Scale", "Scale", "Mesh Scale", 1.0f); // index 0
	desc.AddMember(&CTestEntity::rotation, 1, "Rotation", "Rotation", "Mesh Rotation", 0.0f); // index 1

}

void CTestEntity::Initialize()
{
	OnReset();
}

void CTestEntity::ProcessEvent(SEntityEvent & event)
{
	//! Sent when property of the component changes
	//! nParam[0] = IEntityComponent pointer or nullptr
	//! nParam[1] = Member id of the changed property, (@see IEntityComponent::GetClassDesc() FindMemberById(nParam[1]))

	if (event.event == ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED)
	{
		CTestEntity* component = reinterpret_cast<CTestEntity*>(event.nParam[0]);
		if (component) 
		{
			const Schematyc::CClassMemberDesc* item = component->GetClassDesc().FindMemberById((int32)event.nParam[1]);
		
			switch (item->GetId())
			{
			case 0:
			{
				// was changed only exposed property with index 0 (scale)
				m_pEntity->SetScale(Vec3(scale, scale, scale));
				break;
			}
			case 1:
			{
				// was changed only exposed property with index 1 (rotation)
				m_pEntity->SetRotation(Quat(Ang3(DEG2RAD(rotation), DEG2RAD(rotation), DEG2RAD(rotation))));
				break;
			}
			default:
			{
				OnReset();
				break;
			}
			}			
		}		
	}
}

void CTestEntity::OnReset()
{
	m_pEntity->SetName("TestEntity");
	m_pEntity->LoadGeometry(0, "Assets\\Objects\\Default\\primitive_box.cgf");


	// Make sure that bullets are always rendered regardless of distance
	// Ratio is 0 - 255, 255 being 100% visibility
	GetEntity()->SetViewDistRatio(255);
}
