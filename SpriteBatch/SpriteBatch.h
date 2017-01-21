#pragma once


struct SEntityScriptProperties;
class CGameEntityNodeFactory;


class CSpriteBatchEntity : public CGameObjectExtensionHelper<CSpriteBatchEntity, ISimpleExtension>
{
public:
	CSpriteBatchEntity();
	virtual ~CSpriteBatchEntity();

	//ISimpleExtension
	virtual bool Init(IGameObject* pGameObject) override;
	virtual void PostInit(IGameObject* pGameObject) override;
	virtual void PostUpdate(float frameTime) override;
	virtual void Release() override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	//~ISimpleExtension

	static bool RegisterProperties(SEntityScriptProperties& tables, CGameEntityNodeFactory* pNodeFactory);

private:
	static void OnFlowgraphActivation(EntityId entityId, IFlowNode::SActivationInfo* pActInfo, const class CFlowGameEntityNode* pNode);
	void Reset();
	void GetTexture(const char *path, ITexture **pDiffuse);

	enum EFlowgraphInputPorts
	{
		eInputPorts_LoadSprite,
	};

	enum EFlowgraphOutputPorts
	{
		eOutputPorts_Done,
	};

	string m_geometryPath;
	ITexture *pDiffuseTexture;
	int width;
	int height;
	int id;
};