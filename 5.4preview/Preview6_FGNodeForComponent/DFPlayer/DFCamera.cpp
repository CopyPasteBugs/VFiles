#include "StdAfx.h"
#include "DFCamera.h"


void CDFCamera::Register(Schematyc::CEnvRegistrationScope & componentScope)
{

}

void CDFCamera::ReflectType(Schematyc::CTypeDesc<CDFCamera>& desc)
{
	desc.SetGUID(CDFCamera::IID());
	desc.SetEditorCategory("DFAssets");
	desc.SetLabel("DFCamera");
	desc.SetDescription("A main game camera");
	desc.SetIcon("icons:General/Camera.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::ClientOnly });

	//desc.AddMember(&CDFCamera::Distance, 0, "Distance", "Distance", nullptr, 50.0f);
	//desc.AddMember(&CDFCamera::YawPitch, 1, "YawPitch", "YawPitch", nullptr, Vec2(0.0f, 45.0f));

}

void CDFCamera::Initialize()
{
	// Create the camera component, will automatically update the viewport every frame
	cameraComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCameraComponent>();
	//GetEntity()->Hide(false);

	//blendCamera = SpawnEntity(GetEntity()->GetWorldPos());

	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParams.vPosition = GetEntity()->GetWorldPos();
	spawnParams.qRotation = IDENTITY;
	spawnParams.vScale = Vec3Constants<float>::fVec3_One;
	blendCamera = gEnv->pEntitySystem->SpawnEntity(spawnParams);


	//forwardCamera = SpawnEntity();
	SEntitySpawnParams spawnParams2;
	spawnParams2.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParams2.vPosition = Vec3(0.0f);
	spawnParams2.qRotation = IDENTITY;
	spawnParams2.vScale = Vec3Constants<float>::fVec3_One;
	forwardCamera = gEnv->pEntitySystem->SpawnEntity(spawnParams2);


}

void CDFCamera::ProcessEvent(SEntityEvent & event)
{
	if (event.event == ENTITY_EVENT_UPDATE)
	{
		// early out if no target for camera logic
		if (!Target) return;

		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		// calculate rotation and pos onbase yaw and pitch + distance values
		RotationMatrix = CCamera::CreateOrientationYPR(YawPitchRoll);
		Vec3 cameraForward = GetEntity()->GetWorldTM().GetColumn1();
		Vec3 pos = Target->GetWorldPos() - (cameraForward * Distance);
		Rotation = Quat(RotationMatrix);

		// set ditry rotation and pos(target trasforms) to blend-helper entity
		blendCamera->SetPosRotScale(pos, Rotation, Vec3(1, 1, 1));

		// plane rotation's (root helper) use only yaw value
		planeRotationMatrix = CCamera::CreateOrientationYPR(Vec3(YawPitchRoll.x, 0, 0));
		planeRotation = Quat(planeRotationMatrix);
		forwardCamera->SetPosRotScale(Target->GetWorldPos(), planeRotation, Vec3(1, 1, 1));

		// now, lerp active camera to helper entity 
		Vec3 bpos = GetEntity()->GetWorldPos();
		Quat brot = GetEntity()->GetWorldRotation();

		const float blendPositionSpeed = 40.0f;
		const float blendRotationSpeed = 20.0f;

		bpos.SetLerp(bpos, pos, blendPositionSpeed * pCtx->fFrameTime);
		brot.SetNlerp(brot, Rotation, blendRotationSpeed * pCtx->fFrameTime);

		GetEntity()->SetPosRotScale(bpos, brot, Vec3(1, 1, 1));
	}
}

void CDFCamera::OnShutDown()
{

}

Vec3 CDFCamera::GetPlaneForward()
{
	Vec3 ret = FORWARD_DIRECTION;
	
	if (forwardCamera) 
		ret = forwardCamera->GetWorldTM().GetColumn1();

	return ret;
}
