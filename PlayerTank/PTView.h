#pragma once

#include "Entities/Helpers/ISimpleExtension.h"

class CPlayerTank;

////////////////////////////////////////////////////////
// Player extension to manage the local client's view / camera
////////////////////////////////////////////////////////
class CTankView
	: public CGameObjectExtensionHelper<CTankView, ISimpleExtension>
	, public IGameObjectView
{
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
	CTankView();
	virtual ~CTankView();

	// ISimpleExtension
	virtual void PostInit(IGameObject* pGameObject) override;
	virtual void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	// ~ISimpleExtension

	// IGameObjectView
	virtual void UpdateView(SViewParams &viewParams) override;
	virtual void PostUpdateView(SViewParams &viewParams) override {}
	// ~IGameObjectView

	void InitCameraHelperEntities();
	void FreeCameraHelperEntities();

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

	void OnViewRotate(float dt);
	void OnViewMove(float dt);

	void SpringFollow(float dt);
	void SpringPosition(float dt);
	void SpringRotation(float dt);

	float TestViewForObstacle(float curFollowDistance, bool& hasObstacle);

protected:
	CPlayerTank* pPlayer;

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