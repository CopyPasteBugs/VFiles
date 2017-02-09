#pragma once
#include "StdAfx.h"

#include "GamePlugin.h"

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <Cry3DEngine\IIndexedMesh.h>

class CCustomMesh final
	: public IEntityComponent
	, public IRenderNode
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

	// *** IRenderNode (override all pure virtual methods)
	virtual const char* GetName() const override { return "CustomMesh"; };
	virtual const char* GetEntityClassName() const override { return "CustomMeshClass"; };

	virtual Vec3 GetPos(bool bWorldOnly = true) const override
	{
		if (bWorldOnly)
			return GetEntity()->GetWorldPos();
		else
			return GetEntity()->GetPos();
	};
	virtual const AABB GetBBox() const override { return AABB(Vec3(-100, -100, -100), Vec3(100, 100, 100)); };
	virtual void       FillBBox(AABB& aabb) { aabb = GetBBox(); }
	virtual void       SetBBox(const AABB& WSBBox) override { };

	//! Changes the world coordinates position of this node by delta.
	//! Don't forget to call this base function when overriding it.
	virtual void OffsetPosition(const Vec3& delta) {};

	//! Renders node geometry
	virtual void Render(const struct SRendParams& EntDrawParams, const SRenderingPassInfo& passInfo) override;

	//! Get physical entity.
	virtual struct IPhysicalEntity* GetPhysics() const override { return GetEntity()->GetPhysics(); };
	virtual void                    SetPhysics(IPhysicalEntity* pPhys) {};

	//! Set override material for this instance.
	virtual void SetMaterial(IMaterial* pMat) override { material = pMat; };

	//! Queries override material of this instance.
	virtual IMaterial* GetMaterial(Vec3* pHitPos = NULL) const override { return material; };
	virtual IMaterial* GetMaterialOverride() override { return material; };

	virtual float      GetMaxViewDist() override { return 255.0f; };
	virtual EERType    GetRenderNodeType() override { return EERType::eERType_GeomCache; };
	virtual void       GetMemoryUsage(ICrySizer* pSizer) const override { pSizer->AddObject(this, sizeof(*this));  };

	// *** IRenderNode


	void OnResetState();
protected:
	bool inited = false;
	IRenderNode* pNode = nullptr;
	IStatObj* pStaticObject = nullptr;
	Matrix34 matrix;

	IMaterial* material;

	Vec3 pos[3];
	Vec2 uv[3];
	Vec3 tng[3];
	vtx_idx ind[3];
	SMeshSubset subset;
	SMeshFace face;



};