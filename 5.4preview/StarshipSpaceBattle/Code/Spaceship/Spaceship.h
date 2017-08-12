#pragma once
//#include <CrySchematyc/MathTypes.h>
//#include <CrySchematyc/Reflection/TypeDesc.h>
//#include <CrySchematyc/Env/IEnvRegistrar.h>

#include <ICryMannequin.h>

#include <DefaultComponents/Cameras/CameraComponent.h>
#include <DefaultComponents/Input/InputComponent.h>

class CSpaceship final : public IEntityComponent
{
public:
	enum class EInputFlagType
	{
		Hold = 0,
		Toggle
	};

	typedef uint8 TInputFlags;

	enum class EInputFlag
		: TInputFlags
	{
		MoveLeft = 1 << 0,
		MoveRight = 1 << 1,
		MoveForward = 1 << 2,
		MoveBack = 1 << 3,
		Fire = 1 << 4
	};

public:
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);
	static void ReflectType(Schematyc::CTypeDesc<CSpaceship>& desc);
	static CryGUID& IID()
	{
		static CryGUID id = "{5A69C656-9376-4233-8509-BA5B9BF5A499}"_cry_guid;
		return id;
	}

	//public:
	CSpaceship() = default;
	virtual ~CSpaceship();

	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	virtual void OnShutDown() override;
	// ~IEntityComponent


	void OnReset();
protected:
	Schematyc::GeomFileName m_filePath = "Assets/Objects/Default.cgf";

	IEntity* FindCameraEntity();
	void LoadModel();
	void AcquireComponents();
	void LocateCamBehindShip(IEntity* ent);
	bool value;
	void HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type = EInputFlagType::Hold);
	float GetCamDistance();
	void SpawnBullet();
	void Physicalize();

protected:
	Cry::DefaultComponents::CInputComponent* m_pInputComponent = nullptr;
	//Cry::Audio::DefaultComponents::CTriggerComponent* m_pSoundComponent = nullptr;
	IEntityAudioComponent* soundFire = nullptr;
	CryAudio::ControlId controlId = 0;

	
	TInputFlags inputFlags;
	Vec2 mouseDeltaRotation;
	float baseShipVsCamDistance = 0;
	IEntity* camEntity;
	const int32 geometrySlot = 1;
};
