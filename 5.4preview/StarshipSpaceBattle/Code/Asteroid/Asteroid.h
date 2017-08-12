#pragma once

////////////////////////////////////////////////////////
// Physicalized bullet shot from weaponry, expires on collision with another object
////////////////////////////////////////////////////////
class CAsteroidComponent final : public IEntityComponent
{
public:
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);
	static void ReflectType(Schematyc::CTypeDesc<CAsteroidComponent>& desc);
	static CryGUID& IID()
	{
		static CryGUID id = "{C557B06D-921F-4FFF-97A5-3906431102E2}"_cry_guid;
		return id;
	}
public:
	virtual ~CAsteroidComponent();

	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	// ~IEntityComponent

	void InitPhysBackgroud();
	void InitPhysWithImpule();

	void LoadSplittedModel();
	void LoadModelSeparatedParts();
	void FreeAllResources();
	void ChangeStateDestroyed();

protected:
	float lifeTime = 0;
	bool usedForBackround = false;
	const int32 geometrySlot = 1;
	const int32 geometrySlotSplitted = 2;
	const float mass = 500.0f;
	Matrix34 submat[8];
	IPhysicalEntity* phys[8] = {0};
	IEntity* child[8];
	IStatObj* loadedAsteroidParts[8];
	bool isDestroyed = false;
};