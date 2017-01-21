#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <IGameObject.h>

class CPlayerTank;

class CPlayerTankMovement
	: public IEntityComponent
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CPlayerTankMovement, "CPlayerTankMovement", 0xFDC7F4BEE01444CE, 0x8DB4C7D2EF8B6737);
public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual void OnShutDown() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return (BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_START_LEVEL)); }
	// IEntityComponent

	void Update(SEntityUpdateContext &ctx);


protected:
	CPlayerTank* pTank;
};