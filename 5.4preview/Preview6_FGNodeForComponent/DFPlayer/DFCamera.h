#pragma once

#include <DefaultComponents/Cameras/CameraComponent.h>

class CDFCamera final : public IEntityComponent
{
public:
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);
	static void ReflectType(Schematyc::CTypeDesc<CDFCamera>& desc);
	static CryGUID& IID()
	{
		static CryGUID id = "{1F294565-D722-479B-B0DB-57BFA35CE627}"_cry_guid;
		return id;
	}

public:
	virtual ~CDFCamera() {}
	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_UPDATE); }
	virtual void ProcessEvent(SEntityEvent& event) override;
	virtual void OnShutDown() override;
	// ~IEntityComponent

	void SetYawPitchRoll(Vec3 orient_) { YawPitchRoll = orient_; };
	void SetDistance(float distance_) { Distance = distance_; };
	void SetTarget(IEntity* target_) { Target = target_; };
	Vec3 GetPlaneForward();
	
	Vec3 YawPitchRoll = Vec3(DEG2RAD(0.0f), DEG2RAD(-45.0f), 0.0); // target observe angle 
	float Distance = 15.0f;
	Matrix33 RotationMatrix;
	Quat Rotation;
	IEntity* Target = nullptr;
	IEntity* blendCamera = nullptr;

	IEntity* forwardCamera = nullptr;
	Matrix33 planeRotationMatrix;
	Quat planeRotation;


protected:
	Cry::DefaultComponents::CCameraComponent* cameraComponent = nullptr;

};