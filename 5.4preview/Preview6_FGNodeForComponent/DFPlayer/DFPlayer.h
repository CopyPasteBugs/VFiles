#pragma once

#include <DefaultComponents/Cameras/CameraComponent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Physics/RigidBodyComponent.h>
#include <DefaultComponents/Physics/CapsulePrimitiveComponent.h>
#include <DefaultComponents/Geometry/StaticMeshComponent.h>
#include <DefaultComponents/Audio/TriggerComponent.h>
#include "DFPickableComponent/DFPickableComponent.h"

#include "DFVisionComponents/DFIVisionComponent.h"
#include "DFVisionComponents/DFVisionComponent.h"

//forwards
class CDFCamera;
class CEntityFlowNodeFactory;

class CDFPlayer final : public IEntityComponent, public IPickableCallback
{
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
		MoveBack = 1 << 3
	};
public:
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);
	static void ReflectType(Schematyc::CTypeDesc<CDFPlayer>& desc);
	static CryGUID& IID()
	{
		static CryGUID id = "{2AEA13F1-23E7-4EA5-8734-B2F3F39AC367}"_cry_guid;
		return id;
	}
	static void OnFlowgraphActivation(EntityId entityId, IFlowNode::SActivationInfo* pActInfo, const class CEntityFlowNode* pNode);
	static void RegisterFlowNode(_smart_ptr<CEntityFlowNodeFactory>& pFlowNodeFactory);
public:
	CDFPlayer() = default;
	virtual ~CDFPlayer() {}

	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	virtual void OnShutDown() override;
	// IEntityComponent

	//	IPickableCallback
	virtual void ProcessEventPickable(IPickableComponent* component);;
	// IPickableCallback

	void InitCamera();
	void Revive();

	void CheckForEntities(float radius);
	void LoadPacModel();
	void UpdateCharacterContoller(float frameTime);

protected:
	void HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type = EInputFlagType::Hold);

protected:
	Cry::DefaultComponents::CInputComponent* m_pInputComponent = nullptr;
	Cry::DefaultComponents::CCharacterControllerComponent* characterController = nullptr;
	CVisionComponent* vision = nullptr;

	//Cry::DefaultComponents::CRigidBodyComponent* rigid = nullptr;
	//Cry::DefaultComponents::CCapsulePrimitiveComponent* capsule = nullptr;
	IEntityAudioComponent* soundCoin = nullptr;
	IEntityAudioComponent* soundCherry = nullptr;
	CryAudio::ControlId controlIdCoin = CryAudio::StringToId_CompileTime("coin");;
	CryAudio::ControlId controlIdCherry = CryAudio::StringToId_CompileTime("cherry");;

	TInputFlags m_inputFlags;
	Vec2 m_mouseDeltaRotation = Vec2(0);
	float mouseWheel = 0.0f;
	int32 modelSlot = 1;
	
	// Camera stuff
	IEntity* cameraEntity = nullptr;
	CDFCamera* camera = nullptr;

	// Timer stuff
	int32 period = 100;
	const int32 m_TimerIDCheckPickables = 1;


	float m_mass = 90;
	Vec3 m_velocity = Vec3(0.0f);
};

int AnimCallback(ICharacterInstance* inst, void* p);