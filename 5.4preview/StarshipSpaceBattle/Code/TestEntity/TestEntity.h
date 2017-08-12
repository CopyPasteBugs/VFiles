#pragma once

class CTestEntity final : public IEntityComponent
{
public:
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);
	static void ReflectType(Schematyc::CTypeDesc<CTestEntity>& desc);
	static CryGUID& IID()
	{
		static CryGUID id = "{5F7B9CA7-2069-486F-B9E7-8B3DB72A8BB8}"_cry_guid;
		return id;
	}

public:
	virtual ~CTestEntity() {}

	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED); }
	virtual void ProcessEvent(SEntityEvent& event) override;
	// ~IEntityComponent

protected:
	void OnReset();
protected:
	float scale = 1.0f;
	float rotation = 0.0f;
};