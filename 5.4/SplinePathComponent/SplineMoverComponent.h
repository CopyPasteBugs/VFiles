class CSplineComponent;

class CSplineMoverComponent : public IEntityComponent
{
public:
	static CryGUID& IID()
	{
		static CryGUID id = "{3914D4B9-1AFF-4DFB-9487-4AC8049BECAE}"_cry_guid;
		return id;
	}

	static void ReflectType(Schematyc::CTypeDesc<CSplineMoverComponent>& desc)
	{
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("SplineMoverComponent");
		desc.SetDescription("CSplineMoverComponent");
		desc.SetIcon("icons:General/Core.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::ClientOnly, IEntityComponent::EFlags::Attach });

		desc.AddMember(&CSplineMoverComponent::m_EnableMovement, 'move', "Move", "Move", nullptr, false);
		desc.AddMember(&CSplineMoverComponent::m_Speed, 'spee', "Speed", "Speed", nullptr, 1.0f);
		desc.AddMember(&CSplineMoverComponent::m_LookaheadOverallSteps, 'step', "OverallSteps", "OverallSteps", nullptr, 100.0f);
		desc.AddMember(&CSplineMoverComponent::m_resetButton, 'rese', "Reset", "Reset", "Reset", Serialization::FunctorActionButton<std::function<void()>>());
	}

	CSplineMoverComponent() = default;
	virtual ~CSplineMoverComponent();

	// IEntityComponent
	virtual void Initialize();
	virtual void ProcessEvent(SEntityEvent& event);
	virtual uint64 GetEventMask() const;

public:
	void Update(float fFrameTime);
	void Reset();
	CSplineComponent* GetSplinePath();
	CSplineComponent* m_SplinePath;
	Vec3 m_Pos = Vec3(0.0f);
	Vec3 m_Lookahead = Vec3(0.0f);
	float m_LookaheadStep = 0.0f;
	float m_Delta = 0.0f;
	float m_Speed = 1.0f;

	bool m_EnableMovement = false;
	Serialization::FunctorActionButton<std::function<void()>> m_resetButton;

	float m_Move = -1.0f;
	Schematyc::Range<20, 100> m_LookaheadOverallSteps = 100.0f;
};