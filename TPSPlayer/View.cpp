#include "StdAfx.h"
#include "View.h"
#include "GamePlugin.h"

#include "Player.h"
#include "Input.h"
#include "Attack.h"
#include "Marker.h"

CRYREGISTER_CLASS(CView)

void CView::Initialize()
{

	OnResetState();
}

void CView::ProcessEvent(SEntityEvent & event)
{

}

void CView::OnShutDown()
{
	if (pView) 
	{
		IViewSystem* viewSys = gEnv->pGameFramework->GetIViewSystem();
		pView->SetActive(false);
		viewSys->RemoveView(viewSys->GetViewId(pView));
		pView = nullptr;
	}

	FreeCameraHelperEntities();
}

void CView::Update(float dt)
{	
	OnViewRotate(dt);
	OnViewMove(dt);

	//viewParams.blend = false;
	//viewParams.rotation = eCameraView->GetWorldRotation();
	//viewParams.position = eCameraView->GetWorldPos();
	//viewParams.fov = DEG2RAD(fFov);
	//pView->SetCurrentParams(viewParams);
}

void CView::CaptureView(IEntity * viewHostEntity)
{
	pViewHostEntity = viewHostEntity;
	if (pView)
		pView->LinkTo(viewHostEntity);
}

void CView::StartPositionCamera(float follow, float minFollow, float maxFollow, float pitch, float yaw)
{
	memset(&camParams, 0, sizeof(camParams));

	camParams.curFollow = follow;
	camParams.minFollow = minFollow;
	camParams.maxFollow = maxFollow;
	camParams.pitch = pitch;
	camParams.yaw = yaw;
}

void CView::ShakeCamera(float magnitude, float speed, float damping)
{
	camParams.shakeMagnitude = magnitude;
	camParams.shakeSpeed = speed;
	camParams.shakeDamping = damping;
}

void CView::OnResetState()
{

	pHost = GetEntity()->GetComponent<CPlayer>();

	// *** IView
	if (!pView)
	{
		IViewSystem* viewSys = gEnv->pGameFramework->GetIViewSystem();
		pView = viewSys->CreateView();
		viewId = viewSys->GetViewId(pView);
		viewSys->SetActiveView(viewId);
	}
	// *** IView


	InitCameraHelperEntities();
	SetCameraTarget(GetEntity());
	StartPositionCamera(2.0f, 1.0f, 5.0f, -45.0f, 0.0f);
	camParams.curPosition = GetEntity()->GetWorldPos() + Vec3(0, 0, 10);



	viewParams.rotation = eCameraView->GetWorldRotation();
	viewParams.position = eCameraView->GetWorldPos();
	viewParams.fov = DEG2RAD(fFov);
	pView->SetCurrentParams(viewParams);

	// bind view to this entity
	CaptureView(eCameraView);

}

void CView::InitCameraHelperEntities()
{
	// Root Camera not child of target (it's world entity)

	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParams.vPosition = GetEntity()->GetWorldPos();
	spawnParams.vScale = Vec3(1, 1, 1);
	spawnParams.qRotation = IDENTITY;
	spawnParams.nFlags = ENTITY_FLAG_CLIENT_ONLY;

	eCameraRoot = gEnv->pEntitySystem->SpawnEntity(spawnParams);
	CRY_ASSERT(eCameraRoot != nullptr);
	eCameraRoot->SetName("eCameraRoot");

	// Shake child of root
	SEntitySpawnParams spawnParamsShake;
	spawnParamsShake.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParamsShake.vPosition = Vec3(0, 0, 0);
	spawnParamsShake.vScale = Vec3(1, 1, 1);
	spawnParamsShake.qRotation = IDENTITY;
	spawnParamsShake.nFlags = ENTITY_FLAG_CLIENT_ONLY;

	eCameraShake = gEnv->pEntitySystem->SpawnEntity(spawnParams);
	CRY_ASSERT(eCameraShake != nullptr);
	eCameraRoot->AttachChild(eCameraShake);
	eCameraShake->SetName("eCameraShake");

	// Angle child of Shake
	SEntitySpawnParams spawnParamsAngle;
	spawnParamsAngle.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParamsAngle.vPosition = Vec3(0, 0, 0);
	spawnParamsAngle.vScale = Vec3(1, 1, 1);
	spawnParamsAngle.qRotation = IDENTITY;
	spawnParamsAngle.nFlags = ENTITY_FLAG_CLIENT_ONLY;
	eCameraAngle = gEnv->pEntitySystem->SpawnEntity(spawnParamsAngle);
	CRY_ASSERT(eCameraAngle != nullptr);
	eCameraShake->AttachChild(eCameraAngle);
	eCameraAngle->SetName("eCameraAngle");

	//View child of Angle (Camera's postion and orientation)
	SEntitySpawnParams spawnParamsView;
	spawnParamsView.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParamsView.vPosition = Vec3(0, 0, 0);
	spawnParamsView.vScale = Vec3(1, 1, 1);
	spawnParamsView.qRotation = IDENTITY;
	spawnParamsView.nFlags = ENTITY_FLAG_CLIENT_ONLY;
	eCameraView = gEnv->pEntitySystem->SpawnEntity(spawnParamsView);
	CRY_ASSERT(eCameraView != nullptr);
	eCameraAngle->AttachChild(eCameraView);
	eCameraView->SetName("eCameraView");
}

void CView::FreeCameraHelperEntities()
{
	eCameraView = nullptr;
	eCameraAngle = nullptr;
	eCameraShake = nullptr;
	eCameraRoot = nullptr;
}

void CView::OnViewRotate(float dt)
{
	if (pHost == nullptr) return;

	// Get new adjust for rotations
	Vec2 mouseDeltaRotation = pHost->GetInput()->GetDeltaRotation();

	camParams.yaw += mouseDeltaRotation.x * fRotationSpeedYaw * dt;
	camParams.pitch += mouseDeltaRotation.y * fRotationSpeedPitch * dt;
	//camParams.pitch = CLAMP(camParams.pitch, fRotationLimitsMinPitch, fRotationLimitsMaxPitch);
	camParams.pitch = CLAMP(camParams.pitch, DEG2RAD(-90), DEG2RAD(90));

	// Get adjust for follow distance
	camParams.wheel = (int)pHost->GetInput()->GetDeltaWheel();
}

void CView::OnViewMove(float dt)
{
	Quat worldRotation = eCameraView->GetWorldRotation();
	worldRotation.v.z = 0.0f;

	// Test for view obstacles
	static bool resultTest = false;
	//camParams.curFollow = TestViewForObstacle(camParams.curFollow, resultTest);
	//if (resultTest)
	//{
	//	camParams.wheel = -120;
	//	camParams.follow = camParams.curFollow;
	//	camParams.followVel = 0;
	//}

	// Adjust follow distance with mouse wheel 
	if (camParams.wheel != 0)
	{
		camParams.follow -= (camParams.wheel * dt);
		camParams.wheel = 0;
	}
	// Clamp follow distance
	camParams.follow = CLAMP(camParams.follow, camParams.minFollow, camParams.maxFollow);

	//// Shake Camera(noise offsets)
	//camParams.shakeTime = camParams.shakeTime + dt * camParams.shakeSpeed;
	//float magnitudeForce = sinf(camParams.shakeTime) * camParams.shakeMagnitude;
	//Vec3 shakePos = Vec3(sinf(camParams.shakeTime*fShakeOffsetX)*magnitudeForce, cos(camParams.shakeTime*fShakeOffsetY)*magnitudeForce, 0.0f);
	//camParams.shakeMagnitude -= camParams.shakeDamping * dt;
	//if (camParams.shakeMagnitude < 0.0f) camParams.shakeMagnitude = 0.0f;

	//eCameraShake->SetPos(shakePos);

	// Set distance between cam and target
	SpringFollow(dt);
	// Calc position
	SpringPosition(dt);
	// Calc Rotation
	SpringRotation(dt);

	// Now use new spring positions
	Ang3 yprRoot = Ang3(camParams.yaw, 0.0f, 0.0f);
	Quat rootOrientation = Quat(CCamera::CreateOrientationYPR(yprRoot));

	// Set Root-helper exact in same position as the target entity and then rotate root around UP-axis
	eCameraRoot->SetPosRotScale(camParams.curPosition, rootOrientation, Vec3(1, 1, 1));

	// Set angle for angle-Helper (under / above movements)
	Ang3 yprAngle = Ang3(0.0, camParams.pitch, 0.0);
	Quat angleOrientation = Quat(CCamera::CreateOrientationYPR(yprAngle));
	eCameraAngle->SetRotation(angleOrientation);

	// Shift View-helper(only forward/backward movements) on follow distate in eCameraAngle-space 
	//eCameraView->SetPos(Vec3(0, -camParams.curFollow, 0));
	eCameraView->SetPosRotScale(Vec3(0, -camParams.curFollow, 0), IDENTITY, Vec3(1, 1, 1));

	resultTest = false;
}

void CView::SpringFollow(float dt)
{
	float deltaFollow = camParams.follow - camParams.curFollow;
	float af = 9 * deltaFollow - 6 * camParams.followVel;
	camParams.followVel = camParams.followVel + dt * af;
	camParams.curFollow = camParams.curFollow + dt * camParams.followVel;
}

void CView::SpringPosition(float dt)
{
	Vec3 dist = eCameraTarget->GetWorldPos() - camParams.curPosition;
	Vec3 C1(16.0f, 16.0f, 16.0f);
	Vec3 C2(8.0f, 8.0f, 8.0f);

	Vec3 A = dist.CompMul(C1) - camParams.curVelocity.CompMul(C2);

	camParams.curVelocity = camParams.curVelocity + A.CompMul(Vec3(dt, dt, dt));
	camParams.curPosition = camParams.curPosition + camParams.curVelocity.CompMul(Vec3(dt, dt, dt));
}

void CView::SpringRotation(float dt)
{
	float dY = camParams.yaw - camParams.curYaw;
	float dP = camParams.pitch - camParams.curPitch;

	float aY = dY * 60.0f - camParams.curYawVel * 30.0f;
	float aP = dP * 60.0f - camParams.curPitchVel * 30.0f;

	camParams.curYawVel = camParams.curYawVel + aY * dt;
	camParams.curPitchVel = camParams.curPitchVel + aP * dt;

	camParams.curYaw = camParams.curYaw + camParams.curYawVel * dt;
	camParams.curPitch = camParams.curPitch + camParams.curPitchVel * dt;
}

float CView::TestViewForObstacle(float curFollowDistance, bool & hasObstacle)
{
	hasObstacle = false;
	Vec3 viewForward = eCameraView->GetWorldRotation() * -FORWARD_DIRECTION;
	Vec3 direction = viewForward * camParams.curFollow;

	int hit = 0;
	Vec3 origin = eCameraTarget->GetWorldPos();
	ray_hit rayHit;

	const int physSkipNum = 1;
	IPhysicalEntity* physSkip[physSkipNum];
	physSkip[0] = GetEntity()->GetPhysics();

	unsigned int flags = rwi_stop_at_pierceable | rwi_colltype_any | rwi_ignore_back_faces;

	hit = gEnv->pPhysicalWorld->RayWorldIntersection(origin, direction, entity_query_flags::ent_static, flags, &rayHit, 1, physSkip, physSkipNum);
	if (hit)
	{
		hasObstacle = true;

		// debug hit
		{
			//static IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
			//debug->Begin("ObstacleTestFromTargetToCamera", false);
			//Vec3 lineStart = origin;
			////Vec3 lineEnd = origin + (direction.normalized() * 10.0f);
			//Vec3 lineEnd = rayHit.pt;
			//float timeOut = 5.0f;
			//debug->AddLine(lineStart, lineEnd, ColorF(1.0f, 0.0f, 0.0f), timeOut);
		}

		// return shortest distance
		return min(rayHit.dist, curFollowDistance);
	}

	// No hit with obstacles return same value
	return curFollowDistance;
}
