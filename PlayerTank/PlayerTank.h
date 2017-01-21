#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

class CPlayerTankInput;
class CPlayerTankMovement;
class CPlayerTankView;

class CPlayerTank
	: public IEntityComponent
	, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CPlayerTank, "PlayerTank", 0x40637E94B9894648, 0x9C7264F510AC9C25);
public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual void OnShutDown() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return (BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_START_LEVEL)); };
	virtual struct IEntityPropertyGroup* GetPropertyGroup() override { return this; };
	// IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "PlayerTank"; };
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// IEntityPropertyGroup


	CPlayerTankInput* GetInput();
	CPlayerTankMovement* GetMovement();
	CPlayerTankView* GetView();

	float GetSpeed() { return fSpeed; };

	IGameObject* pGameObject;

protected:

	void OnResetState();
	
	float fSpeed;
	CPlayerTankInput* pInput;
	CPlayerTankMovement* pMovement;
	CPlayerTankView* pView;

};