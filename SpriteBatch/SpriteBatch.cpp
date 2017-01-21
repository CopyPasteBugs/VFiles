#include "StdAfx.h"
#include "SpriteBatch.h"
#include "game/GameFactory.h"
#include "flownodes/FlowGameEntityNode.h"
#include "Game/GameFactory.h"


CSpriteBatchEntity::CSpriteBatchEntity() : 
	pDiffuseTexture(nullptr)
{
}

CSpriteBatchEntity::~CSpriteBatchEntity()
{
	if (pDiffuseTexture) 
	{
		pDiffuseTexture->Release();
		pDiffuseTexture = nullptr;
	}
}

bool CSpriteBatchEntity::Init(IGameObject * pGameObject)
{
	SetGameObject(pGameObject);
	return true;
}

void CSpriteBatchEntity::PostInit(IGameObject * pGameObject)
{
	Reset();
	pGameObject->EnablePostUpdates(this);
}

void CSpriteBatchEntity::Release()
{
}

void CSpriteBatchEntity::ProcessEvent(SEntityEvent & event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
	case ENTITY_EVENT_RESET:
	{
		Reset();
	}
	break;
	}
}

bool CSpriteBatchEntity::RegisterProperties(SEntityScriptProperties & tables, CGameEntityNodeFactory * pNodeFactory)
{
	if (tables.pPropertiesTable)
	{
		tables.pPropertiesTable->SetValue("TextureMap", "Assets/textures/ball.dds");
	}

	if (tables.pEditorTable)
	{
		tables.pEditorTable->SetValue("Icon", "prompt.bmp");
		tables.pEditorTable->SetValue("IconOnTop", true);
	}

	if (pNodeFactory)
	{
		std::vector<SInputPortConfig> inputs;
		inputs.push_back(InputPortConfig<string>("LoadTexture", "Load sprite"));
		pNodeFactory->AddInputs(inputs, OnFlowgraphActivation);

		std::vector<SOutputPortConfig> outputs;
		outputs.push_back(OutputPortConfig<bool>("Done"));
		pNodeFactory->AddOutputs(outputs);
	}

	return true;
}

void CSpriteBatchEntity::OnFlowgraphActivation(EntityId entityId, IFlowNode::SActivationInfo * pActInfo, const CFlowGameEntityNode * pNode)
{
	if (CSpriteBatchEntity* pSpriteEntity = QueryExtension(entityId))
	{
		if (IsPortActive(pActInfo, eInputPorts_LoadSprite))
		{
			//pSpriteEntity->GetEntity()->LoadGeometry(0, GetPortString(pActInfo, eInputPorts_LoadSprite));
			ActivateOutputPort(entityId, eOutputPorts_Done, TFlowInputData(true));
		}
	}
}

void CSpriteBatchEntity::Reset()
{
	SAFE_RELEASE(pDiffuseTexture);

	SmartScriptTable propertiesTable;
	GetEntity()->GetScriptTable()->GetValue("Properties", propertiesTable);

	const char* texturePath = "Assets/textures/ball.dds";
	GetTexture(texturePath, &pDiffuseTexture);
	//GetEntity()->LoadGeometry(0, geometryPath);

	width = (int)pDiffuseTexture->GetWidth();
	height = (int)pDiffuseTexture->GetHeight();
	id = pDiffuseTexture->GetTextureID();
}

void CSpriteBatchEntity::GetTexture(const char *path, ITexture **pDiffuse)
{
	stack_string textureMap = path;

	char diffuseCubemap[ICryPak::g_nMaxPath];
	_snprintf(diffuseCubemap, sizeof(diffuseCubemap), "%s%s%s.%s", PathUtil::AddSlash(PathUtil::GetPathWithoutFilename(textureMap)).c_str(),
		PathUtil::GetFileName(textureMap).c_str(), "", PathUtil::GetExt(textureMap));

	// '\\' in filename causing texture duplication
	stack_string diffuseCubemapUnix = PathUtil::ToUnixPath(diffuseCubemap);

	*pDiffuse = gEnv->pRenderer->EF_LoadTexture(diffuseCubemapUnix, FT_DONT_STREAM);
}

void CSpriteBatchEntity::PostUpdate(float frameTime)
{
	if (pDiffuseTexture != nullptr)
	{
		int x;
		int y;

		for (int i = 0; i < 10000; i++)
		{
			x = rand() % 800;
			y = rand() % 600;

			gEnv->pRenderer->Push2dImage(x, y, 32, 32, id, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}
