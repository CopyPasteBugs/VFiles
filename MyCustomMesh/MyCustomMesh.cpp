#include "StdAfx.h"
#include "GamePlugin.h"
#include "MyCustomMesh.h"

#include <Cry3DEngine/I3DEngine.h>
#include <CryRenderer\IRenderer.h>
#include <Cry3DEngine\IRenderNode.h>
#include <Cry3DEngine\IIndexedMesh.h>


class CCustomMeshRegistrator
	: public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CCustomMesh>("CustomMesh", "Defaults", "Particles.bmp");
	}
	virtual void Unregister() override
	{

	}
public:
	CCustomMeshRegistrator() {}
	~CCustomMeshRegistrator()
	{
	}
};

static CCustomMeshRegistrator g_customMeshRegistrator;

CRYREGISTER_CLASS(CCustomMesh)

void CCustomMesh::Initialize()
{
	//OnResetState();
	GetEntity()->Activate(true);
	material = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("default.mtl");
	//gEnv->p3DEngine->RegisterEntity(this);
	
}

void CCustomMesh::ProcessEvent(SEntityEvent & event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_XFORM_FINISHED_EDITOR:
	case ENTITY_EVENT_UPDATE:
	{
		break;
	}
	case ENTITY_EVENT_START_LEVEL:
		OnResetState();
		break;
	}
}

void CCustomMesh::OnShutDown()
{

}

void CCustomMesh::OnResetState()
{
	if (!inited)
	{				
		if (!pStaticObject)
		{
			pStaticObject = gEnv->p3DEngine->CreateStatObj();

			IIndexedMesh* idxMesh = pStaticObject->GetIndexedMesh();
			CMesh* mesh = idxMesh->GetMesh();
			int count = 3;
			mesh->SetIndexCount(count);
			mesh->SetVertexCount(count);
			mesh->SetTexCoordsCount(count);
			int gotElements = 0;
			
			//pos = mesh->GetStreamPtr<Vec3>(CMesh::POSITIONS, &gotElements);
			
			pos[0] = Vec3(0, 0, 0);
			pos[1] = Vec3(0, 100, 0);
			pos[2] = Vec3(0, 100, 100);

			mesh->SetSharedStream(CMesh::POSITIONS, &pos[0], count);


			tng[0] = Vec3(0, 1, 0);
			tng[1] = Vec3(0, 1, 0);
			tng[2] = Vec3(0, 1, 0);

			mesh->SetSharedStream(CMesh::TANGENTS, &tng[0], count);

			//uv = mesh->GetStreamPtr<Vec2>(CMesh::TEXCOORDS, &gotElements);

			uv[0] = Vec2(0, 0);
			uv[1] = Vec2(1, 0);
			uv[2] = Vec2(1, 1);

			mesh->SetSharedStream(CMesh::TEXCOORDS, &uv[0], count);

			//ind = mesh->GetStreamPtr<vtx_idx>(CMesh::INDICES, &gotElements);

			ind[0] = 0;
			ind[1] = 1;
			ind[2] = 2;

			mesh->SetSharedStream(CMesh::INDICES, &ind[0], count);

			subset.nNumIndices = count;
			subset.nNumVerts = count;
			//subset.nMatID = 0;
			//subset.FixRanges(&ind[0]);
			subset.nFirstVertId = 0;
			subset.nFirstIndexId = 0;
			
			mesh->m_subsets.clear();
			mesh->m_subsets.push_back(subset);

			face.v[0] = 0;
			face.v[1] = 1;
			face.v[2] = 2;
			face.nSubset = 1;

			mesh->SetSharedStream(CMesh::FACES, &face, 1);

			mesh->m_bbox = AABB(Vec3(-100,-100,-100), Vec3(100,100,100));

			bool ret = mesh->Validate(nullptr);

			// make the static object update
			pStaticObject->SetFlags(STATIC_OBJECT_GENERATED | STATIC_OBJECT_DYNAMIC);
			pStaticObject->Invalidate();
			pStaticObject->SetMaterial(material);

			GetEntity()->SetStatObj(pStaticObject, 0, false);
		}
		
		inited = true;
	}
}

