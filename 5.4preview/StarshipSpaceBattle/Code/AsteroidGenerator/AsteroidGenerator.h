#pragma once

class CAsteroidGenerator final : public IEntityComponent
{
public:
	virtual ~CAsteroidGenerator() {}

	static void Register(Schematyc::CEnvRegistrationScope& componentScope);
	static void ReflectType(Schematyc::CTypeDesc<CAsteroidGenerator>& desc);
	static CryGUID& IID()
	{
		static CryGUID id = "{A7F35CE6-A215-408F-94AE-F805EAF0C769}"_cry_guid;
		return id;
	}

	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override { return BIT(ENTITY_EVENT_UPDATE); }
	virtual void ProcessEvent(SEntityEvent& event) override;
	// ~IEntityComponent
protected:
	void EmittAsteroid();
	void RepositionGeneratorOrigin();
protected:
	int32 asteroidMax = 20;
	float emittPeriod = 0.5f;
	Vec2 emittRect = Vec2(80,0);
	Vec3 emittOrigin;
};