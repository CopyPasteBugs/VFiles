#pragma once
#include "StdAfx.h"

#include "GamePlugin.h"

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <Cry3DEngine\IIndexedMesh.h>

class CCustomMesh final
	: public IEntityComponent
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CCustomMesh, "CustomMesh", 0x7C08F4ADDA1D45CC, 0x9DFC5AC3BAF2BDB6);

public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_START_LEVEL); }
	//virtual EEntityProxy GetProxyType() const override { return ; };
	virtual void OnShutDown() override;
	// IEntityComponent

	//// IEntityPropertyGroup
	//virtual const char* GetLabel() const override { return "CustomMesh"; };
	//virtual void SerializeProperties(Serialization::IArchive& archive) override;
	//// IEntityPropertyGroup

	void OnResetState();
protected:
	bool inited = false;
	//IRenderNode* pNode = nullptr;
	_smart_ptr<IStatObj> pStaticObject = nullptr;
	_smart_ptr<IMaterial> material = nullptr;
	Matrix34 matrix;
	Vec3 pos[3];
	Vec2 uv[3];
	Vec3 tng[3];
	vtx_idx ind[3];
	SMeshSubset subset;
	SMeshFace face;

};