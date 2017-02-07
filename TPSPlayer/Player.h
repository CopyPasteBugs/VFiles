#pragma once
#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <IActionMapManager.h>
#include <CryMath/Cry_Camera.h>
#include <IViewSystem.h>
#include <IActorSystem.h>

class CView;
class CMarker;
class CAttack;
class CInput;

class CPlayer final
	: public IEntityComponent
	, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CPlayer, "Player", 0x4C59A850F4BA449E, 0xBA02F86423196D07);
public:


public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return (BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_START_LEVEL)); }
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	virtual void OnShutDown() override;
	// IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "Player"; };
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// IEntityPropertyGroup

	CInput* GetInput() { return pInput; };
	CView* GetView() { return pView; };
	CAttack* GetAttack() { return pAttack; };
	CMarker* GetMarker() { return pMarker; };

protected:
	void OnResetState();
	void Physicalize();
	void LoadModel();
	void Update(SEntityUpdateContext* ctx);
	void UpdateMovement(SEntityUpdateContext* ctx);
	void GetLatestPhysicsStats(IPhysicalEntity &physicalEntity);
	void UpdateMovementRequest(float frameTime, IPhysicalEntity &physicalEntity);


protected:
	CView* pView = nullptr;
	CMarker* pMarker = nullptr;
	CAttack* pAttack = nullptr;
	CInput* pInput = nullptr;

	bool bIsOnGround = false;
	Vec3 groundNormal;
	float fSpeed = 10.0f;

};