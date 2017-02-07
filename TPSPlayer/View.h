#pragma once
#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <IViewSystem.h>

class CPlayer;

class CView final
	: public IEntityComponent
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CView, "View", 0xCB6CA62EE2A04D69, 0xB8C8F327529F8F46);
public:
	typedef struct SCameraParameters
	{
		float minFollow;
		float maxFollow;
		float pitch;
		float yaw;
		float follow;
		float curFollow;
		float followVel;
		Vec3 curPosition;
		Vec3 newPosition;
		Vec3 curVelocity;

		float curYaw;
		float curPitch;

		float curYawVel;
		float curPitchVel;

		float shakeMagnitude;
		float shakeSpeed;
		float shakeTime;
		float shakeDamping;

		int wheel;
		float lastCollisionTime;
	} SCameraParameters;

	SCameraParameters camParams;
public:
	// IEntityComponent
	virtual void Initialize() override;
	virtual	void ProcessEvent(SEntityEvent &event) override;
	virtual uint64 GetEventMask() const override { return 0; };
	virtual void OnShutDown() override;
	// IEntityComponent

	void Update(float dt);
	void CaptureView(IEntity* viewHostEntity);

	void StartPositionCamera(float follow, float minFollow, float maxFollow, float pitch, float yaw);
	void ShakeCamera(float magnitude, float speed, float damping);
	void SetCameraTarget(IEntity* target) { eCameraTarget = target; };
	IEntity* GetCameraTarget() { return eCameraTarget; };

	Quat GetViewRotation() { return eCameraView->GetWorldRotation(); };
	Quat GetViewRotationPlane()
	{
		return GetCameraRootRotation();
	};

	Vec3 GetViewFoward() { return GetViewRotation() * FORWARD_DIRECTION; }; // world rot
	Quat GetCameraRootRotation() { return eCameraRoot->GetWorldRotation(); }; // on plane rotation
	Vec3 GetCameraRootForward() { return eCameraRoot->GetWorldRotation() * FORWARD_DIRECTION; }; // on plane dir

	void OnResetState();

protected:
	void InitCameraHelperEntities();
	void FreeCameraHelperEntities();
	void OnViewRotate(float dt);
	void OnViewMove(float dt);
	void SpringFollow(float dt);
	void SpringPosition(float dt);
	void SpringRotation(float dt);
	float TestViewForObstacle(float curFollowDistance, bool& hasObstacle);

protected:
	CPlayer* pHost;
	IView* pView;
	unsigned int viewId;
	SViewParams viewParams;
	float fFov;
	Vec3 position;
	Quat rotation;
	IEntity* pViewHostEntity;

	// Helpers entities
	IEntity* eCameraRoot;
	IEntity* eCameraShake;
	IEntity* eCameraAngle;
	IEntity* eCameraView;
	IEntity* eCameraTarget;

	// Settings
	float fRotationSpeedYaw = 0.05f;
	float fRotationSpeedPitch = 0.05f;
	float fRotationLimitsMinPitch = -0.84f;
	float fRotationLimitsMaxPitch = 1.3f;

	float fShakeOffsetX = 3.0f;
	float fShakeOffsetY = 1.0f;

};