#pragma once

////////////////////////////////////////////////////////
// Physicalized bullet shot from weaponry, expires on collision with another object
////////////////////////////////////////////////////////
class CHit final : public IEntityComponent
{
public:
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);
	static void ReflectType(Schematyc::CTypeDesc<CHit>& desc);
	static CryGUID& IID()
	{
		static CryGUID id = "{D8E8B3BA-3C00-4DB1-B57C-6E6E5E9A0C5D}"_cry_guid;
		return id;
	}
public:
	virtual ~CHit();

	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	// ~IEntityComponent

protected:
	float fKillTime = 0.0f;
};