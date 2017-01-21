#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <IGameObject.h>
#include <IViewSystem.h>

class CPlayerTank;

class CPlayerTankView
	: public IEntityComponent
	, public IGameObjectView
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CPlayerTankView, "CPlayerTankView", 0xAE3C2611DEB84CCA, 0x9328384C3154A5A7);
public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual void OnShutDown() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return (BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_START_LEVEL));; }
	// IEntityComponent

	//void Update(SEntityUpdateContext &ctx);

	// IGameObjectView
	virtual void UpdateView(SViewParams &viewParams) override;
	virtual void PostUpdateView(SViewParams &viewParams) override {}
	// ~IGameObjectView


protected:
	CPlayerTank* pTank;
};