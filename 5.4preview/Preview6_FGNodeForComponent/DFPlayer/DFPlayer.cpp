#include "StdAfx.h"

#include "DFCamera.h"
#include "DFEnemy/DFEnemy.h"
#include "DFPickableComponent/DFPickableComponent.h"
#include "DFPlayer.h"
#include "flownodes/EntityFlowNode.h"


void CDFPlayer::Register(Schematyc::CEnvRegistrationScope & componentScope)
{

}

void CDFPlayer::OnFlowgraphActivation(EntityId entityId, IFlowNode::SActivationInfo* pActInfo, const class CEntityFlowNode* pNode)
{
	if (auto* pEntity = gEnv->pEntitySystem->GetEntity(entityId))
	{

	}
}

void CDFPlayer::RegisterFlowNode(_smart_ptr<CEntityFlowNodeFactory>& pFlowNodeFactory)
{
	pFlowNodeFactory = new CEntityFlowNodeFactory("entity:Player");

	pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("Hide", ""));
	pFlowNodeFactory->m_inputs.push_back(InputPortConfig<bool>("UnHide", ""));
	pFlowNodeFactory->m_inputs.push_back(InputPortConfig<string>("LoadGeometry", ""));
	pFlowNodeFactory->m_activateCallback = CDFPlayer::OnFlowgraphActivation;

	pFlowNodeFactory->m_outputs.push_back(OutputPortConfig_Void("OnHide"));
	pFlowNodeFactory->m_outputs.push_back(OutputPortConfig_Void("OnUnHide"));
	pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<EntityId>("OnCollision"));
	pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<string>("CollisionSurfaceName"));
	pFlowNodeFactory->m_outputs.push_back(OutputPortConfig<string>("OnGeometryChanged"));

	pFlowNodeFactory->Close();
}

void CDFPlayer::ReflectType(Schematyc::CTypeDesc<CDFPlayer>& desc)
{
	desc.SetGUID(CDFPlayer::IID());
	desc.SetEditorCategory("DFAssets");
	desc.SetLabel("DFPlayer");
	desc.SetDescription("A main game character");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });
}

void CDFPlayer::Initialize()
{
	// Get the input component, wraps access to action mapping so we can easily get callbacks when inputs are triggered
	m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();
	characterController = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();

	soundCoin = m_pEntity->GetOrCreateComponent<IEntityAudioComponent>();
	if (soundCoin)
	{
		soundCoin->SetCurrentEnvironments();
	}

	soundCherry = m_pEntity->GetOrCreateComponent<IEntityAudioComponent>();
	if (soundCherry)
	{
		soundCherry->SetCurrentEnvironments();
	}


	vision = m_pEntity->GetOrCreateComponent<CVisionComponent>();
	vision->params.skipPhys.push_back(GetEntity()->GetPhysicalEntity());

	// Register an action, and the callback that will be sent when it's triggered
	m_pInputComponent->RegisterAction("player", "moveleft", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveLeft, activationMode);  });
	// Bind the 'A' key the "moveleft" action
	m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, EKeyId::eKI_A);

	m_pInputComponent->RegisterAction("player", "moveright", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveRight, activationMode);  });
	m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, EKeyId::eKI_D);

	m_pInputComponent->RegisterAction("player", "moveforward", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveForward, activationMode);  });
	m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, EKeyId::eKI_W);

	m_pInputComponent->RegisterAction("player", "moveback", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveBack, activationMode);  });
	m_pInputComponent->BindAction("player", "moveback", eAID_KeyboardMouse, EKeyId::eKI_S);

	m_pInputComponent->RegisterAction("player", "mouse_rotateyaw", [this](int activationMode, float value) { m_mouseDeltaRotation.x -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotateyaw", eAID_KeyboardMouse, EKeyId::eKI_MouseX);

	m_pInputComponent->RegisterAction("player", "mouse_rotatepitch", [this](int activationMode, float value) { m_mouseDeltaRotation.y -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotatepitch", eAID_KeyboardMouse, EKeyId::eKI_MouseY);

	// mouse wheel
	m_pInputComponent->RegisterAction("player", "mouse_wheelDown", [this](int activationMode, float value) { mouseWheel = value; });
	m_pInputComponent->BindAction("player", "mouse_wheelDown", eAID_KeyboardMouse, EKeyId::eKI_MouseWheelDown);

	m_pInputComponent->RegisterAction("player", "mouse_wheelUp", [this](int activationMode, float value) { mouseWheel = value; });
	m_pInputComponent->BindAction("player", "mouse_wheelUp", eAID_KeyboardMouse, EKeyId::eKI_MouseWheelUp);

	m_pInputComponent->RegisterAction("player", "pause", [this](int activationMode, float value) { bool p = gEnv->pGameFramework->IsGamePaused(); gEnv->pGameFramework->PauseGame(!p, true); });
	m_pInputComponent->BindAction("player", "pause", eAID_KeyboardMouse, EKeyId::eKI_P);

	
	LoadPacModel();
	Revive();
	InitCamera();

}

uint64 CDFPlayer::GetEventMask() const
{
	return BIT64(ENTITY_EVENT_START_GAME) | BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_TIMER);
}

void CDFPlayer::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_START_GAME:
	{
		Revive();
		m_pEntity->SetTimer(m_TimerIDCheckPickables, period);
	}
	break;
	case ENTITY_EVENT_TIMER:
	{
		int TimerId = (int)event.nParam[0];

		if (TimerId == m_TimerIDCheckPickables)
		{
		
			CheckForEntities(2.0f);
			m_pEntity->SetTimer(m_TimerIDCheckPickables, period);
			break;
		}
	}
	break;
	case ENTITY_EVENT_UPDATE:
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		const float moveSpeed = 20.5f;
		Vec3 velocity = ZERO;

		// Check input to calculate local space velocity
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveLeft)
		{
			velocity.x -= moveSpeed * pCtx->fFrameTime;
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveRight)
		{
			velocity.x += moveSpeed * pCtx->fFrameTime;
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveForward)
		{
			velocity.y += moveSpeed * pCtx->fFrameTime;
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveBack)
		{
			velocity.y -= moveSpeed * pCtx->fFrameTime;
		}


		if (camera) 
		{
			if (fabs(mouseWheel) > 0)
			{
				
				camera->Distance += mouseWheel * pCtx->fFrameTime;
				float cd = CLAMP(camera->Distance, 5, 20);
				camera->SetDistance(cd);
				mouseWheel = 0.0f;
			}


			const float rotSpeed = 0.1f;
			camera->YawPitchRoll.x += ((m_mouseDeltaRotation.x * rotSpeed) * pCtx->fFrameTime);
			m_mouseDeltaRotation.x = 0.0f;
		}

		//// Update camera pos
		//{
		//	Matrix34 localTransform = IDENTITY;
		//	//Ang3 ypr = CCamera::CreateAnglesYPR();
		//	localTransform.SetRotation33(CCamera::CreateOrientationYPR(Vec3(DEG2RAD(45.0f), DEG2RAD(0.0f), DEG2RAD(0.01f))));
		//	localTransform.SetTranslation(Vec3(0, -10, 0));
		//	cameraComponent->SetTransformMatrix(localTransform);
		//}


		// Update the player's transformation
		//Matrix34 transformation = m_pEntity->GetWorldTM();
		//transformation.AddTranslation(transformation.TransformVector(velocity));
		
		
		if (camera)
		{
			if (cameraEntity)
			if (camera->forwardCamera != nullptr)
			{
				const Matrix34 forwardHelper = camera->forwardCamera->GetWorldTM();
				Vec3 move = forwardHelper.TransformVector(velocity);
				//transformation.AddTranslation(move);
				
				
				characterController->AddVelocity(move);
				//rigid->SetVelocity(move * 100.f);
			}
		}

		UpdateCharacterContoller(pCtx->fFrameTime);

		//  if pac are walking then speedup the animation 
		if (auto* pCharacter = m_pEntity->GetCharacter(1))
		{
			if (characterController->GetVelocity().GetLength2D() > 0.2f)
				pCharacter->GetISkeletonAnim()->SetLayerPlaybackScale(1, 3.0f);
			else
				pCharacter->GetISkeletonAnim()->SetLayerPlaybackScale(1, 1.0f);
		}


		vision->params.position = GetEntity()->GetWorldPos() + Vec3(0,0,1); // add offset By Z axis to avoid raycasting terrain
		vision->params.viewDirection = GetEntity()->GetForwardDir();

	}
	break;
	//case ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED:
	//{
	//	CDFPlayer* component = (CDFPlayer*)(event.nParam[0]);
	//	const Schematyc::CClassMemberDesc* item = component->GetClassDesc().FindMemberById((int32)event.nParam[1]);
	//	
	//	if (item->GetId() == 'some') 
	//	{
	//		CryLogAlways("catch somevalue changing");
	//	}
	//	
	//}
	//break;
	}
}

void CDFPlayer::OnShutDown()
{
	if (camera) 
	{
		camera->SetTarget(nullptr);
		//GameUtils::RemoveComponentWithEntity(camera);
	}
}

void CDFPlayer::ProcessEventPickable(IPickableComponent * component)
{
	if (!component) return;

	switch (component->GetType())
	{
	case EPickableType::PICABLE_CHERRY: 
	{
		IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
		debug->Begin("ProcessPickEventCherry", true);
		string s;
		s = s.Format("Got Cherry = %d", component->GetValue());
		debug->AddText(10, 120, 2, ColorF(1.0f), 2, s.c_str());
		component->RemovePickable();

		if (soundCherry)
			soundCherry->ExecuteTrigger(controlIdCherry);

		//gEnv->pAudioSystem->ExecuteTrigger(controlIdCherry);
		
		break;
	}
	case EPickableType::PICABLE_COIN:
	{
		IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
		debug->Begin("ProcessPickEventCoin", true);
		string s;
		s = s.Format("Got Coin = %d", component->GetValue());
		debug->AddText(10, 160, 2, ColorF(1.0f), 2, s.c_str());
		component->RemovePickable();

		if (soundCoin)
			soundCherry->ExecuteTrigger(controlIdCoin);
		// Play sound globally
		//gEnv->pAudioSystem->ExecuteTrigger(controlIdCoin);

		break;
	}
	default:
	{
	
		break;
	}
	}
}

void CDFPlayer::InitCamera()
{
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParams.vPosition = Vec3(0.0f);
	spawnParams.qRotation = IDENTITY;
	spawnParams.vScale = Vec3Constants<float>::fVec3_One;

	if (IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		camera = pEntity->CreateComponent<CDFCamera>();
	}

	camera->SetTarget(GetEntity());
	cameraEntity = camera->GetEntity();
}

void CDFPlayer::Revive()
{
	// Unhide the entity in case hidden by the Editor
	GetEntity()->Hide(false);
	GetEntity()->SetWorldTM(Matrix34::Create(Vec3(1, 1, 1), IDENTITY, GetEntity()->GetWorldPos()));

	m_inputFlags = 0;
	m_mouseDeltaRotation = ZERO;

	if (auto* pCharacter = m_pEntity->GetCharacter(1))
	{
		CryCharAnimationParams animParams;
		animParams.m_fPlaybackSpeed = 1.0f;
		animParams.m_nFlags = CA_LOOP_ANIMATION | CA_FORCE_SKELETON_UPDATE;
		animParams.m_nLayerID = 1;
		animParams.m_fKeyTime = 0.5f;

		pCharacter->SetFlags(ECharRenderFlags::CS_FLAG_UPDATE_ALWAYS);
		

		//string animationName = PathUtil::GetFileName("Assets\\Objects\\pac\\pac_eat.anm"); // ret "pac_eat"
		if (pCharacter->GetISkeletonAnim()->StartAnimation("eat", animParams)) // or just use short name "pac_eat" or "eat"
		{
			pCharacter->GetISkeletonAnim()->SetLayerBlendWeight(animParams.m_nLayerID, 1.0f);
			
		}
	}
}

void CDFPlayer::CheckForEntities(float radius)
{
	SEntityProximityQuery query;
	query.pEntityClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	gEnv->pEntitySystem->QueryProximity(query);

	// now count how many entities have %some% component
	int32 count = 0;
	for (int32 i = 0; i < query.nCount; i++)
	{		
		IEntity* ent = query.pEntities[i];
		IPickableComponent* component = ent->GetComponent<IPickableComponent>();
		if (component)
		{
			count++;
			component->Pick(this);		
		}
	}

	//IPersistantDebug* debug = gEnv->pGameFramework->GetIPersistantDebug();
	//Vec3 pos = GetEntity()->GetPos();
	//Vec3 wsMax = Vec3(pos.x + radius, pos.y + radius, pos.z + radius);
	//Vec3 wsMin = Vec3(pos.x - radius, pos.y - radius, pos.z - radius);
	//query.box = AABB(wsMin, wsMax);
	//debug->Begin("AABB", true);
	//debug->AddAABB(wsMin, wsMax, ColorF(1, 1, 0), 1);

	//debug->Begin("CheckForEntities", true);
	//string s;
	//s = s.Format("enities with QueryProximity = %d", count);
	//debug->AddText(10, 100, 2, ColorF(1.0f), 2, s.c_str());

}

void CDFPlayer::LoadPacModel()
{

	int slot = m_pEntity->LoadCharacter(1, "Assets\\Objects\\pac\\pac.cga");
	//int slot = GetEntity()->LoadGeometry(modelSlot, "Assets\\Objects\\Default\\primitive_cylinder.cgf");
	uint32 flags = ENTITY_SLOT_RENDER | ENTITY_SLOT_IGNORE_VISAREAS | ENTITY_SLOT_CAST_SHADOW;
	m_pEntity->SetSlotFlags(slot, flags);

	//uint32 NoShadows = m_pEntity->GetSlotFlags(slot);
	//NoShadows &= ~ENTITY_SLOT_CAST_SHADOW;
	//m_pEntity->SetSlotFlags(slot, NoShadows);

	auto *pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("Assets\\Objects\\pac\\pac");
	m_pEntity->SetMaterial(pMaterial);

	//SShaderItem shaderItem = pMaterial->GetShaderItem();
	

	//ICharacterInstance* pCharacter = m_pEntity->GetCharacter(1);
	//pCharacter->GetISkeletonPose()->SetPostProcessCallback(&AnimCallback, this);
}

void CDFPlayer::UpdateCharacterContoller(float frameTime)
{
	
	if (characterController->GetVelocity().GetLength() > 0.2f)
	{
		Vec3 forward = characterController->GetVelocity().GetNormalized();
		forward.y = 0;
		Quat newRotation = Quat::CreateRotationVDir(forward);

		Quat oldRotation = m_pEntity->GetRotation();
		Quat finalQ = Quat::CreateNlerp(oldRotation, newRotation, 0.2f);
		finalQ.Normalize();

		// Send updated transform to the entity, only orientation changes
		m_pEntity->SetPosRotScale(m_pEntity->GetWorldPos(), finalQ, GetEntity()->GetScale());
		//m_pEntity->SetRotation(finalQ);
	}
}

void CDFPlayer::HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type)
{
	switch (type)
	{
	case EInputFlagType::Hold:
	{
		if (activationMode == eIS_Released)
		{
			m_inputFlags &= ~flags;
		}
		else
		{
			m_inputFlags |= flags;
		}
	}
	break;
	case EInputFlagType::Toggle:
	{
		if (activationMode == eIS_Released)
		{
			m_inputFlags ^= flags;
		}
	}
	break;
	}
}

int AnimCallback(ICharacterInstance * inst, void * p)
{
	CryLogAlways("123");
	return 0;
}
