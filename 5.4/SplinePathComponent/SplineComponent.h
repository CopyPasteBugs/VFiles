#pragma once
#include <vector>

enum class ESplineType : uint32
{
	LINEAR,
	BEZIER,
	CATMULLROM,
};

static void ReflectType(Schematyc::CTypeDesc<ESplineType>& desc)
{
	desc.SetGUID("{75435FBE-6C81-4E8D-8F0D-9567A4D0C4FD}"_cry_guid);
	desc.SetLabel("Spline Type");
	desc.SetDescription("Determines what type spline is");
	desc.SetDefaultValue(ESplineType::LINEAR);
	desc.AddConstant(ESplineType::LINEAR, "Linear", "Linear");
	desc.AddConstant(ESplineType::BEZIER, "Bezier", "Bezier");
	desc.AddConstant(ESplineType::CATMULLROM, "CatmullRom", "CatmullRom");
}

struct SSplineSampleOptions
{
	inline bool operator==(const SSplineSampleOptions &rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

	static void ReflectType(Schematyc::CTypeDesc<SSplineSampleOptions>& desc)
	{

		desc.SetGUID("{7F367C98-6802-426E-9A08-DF2161969475}"_cry_guid);
		desc.AddMember(&SSplineSampleOptions::m_useSampledSpline, 'samp', "OptimizeSpline", "Optimize Spline", "This is optimization: it's splits Spline into short lines", true);
		desc.AddMember(&SSplineSampleOptions::m_SampleQuality, 'rate', "SampleQuality", "SampleQuality", "Indicates how much the spline will be splitted on short lines", 1.f);
	}

	bool m_useSampledSpline = true;
	Schematyc::Range<0, 64> m_SampleQuality = 24;
};

struct SSplineMovementOptions
{
	inline bool operator==(const SSplineMovementOptions &rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

	static void ReflectType(Schematyc::CTypeDesc<SSplineMovementOptions>& desc)
	{

		desc.SetGUID("{669C86E8-9A5D-4198-98D9-2E390E5A53D7}"_cry_guid);
		desc.AddMember(&SSplineMovementOptions::m_speed, 'spee', "Speed", "Speed", nullptr, 1.0f);
		desc.AddMember(&SSplineMovementOptions::m_fixedSpeed, 'cspe', "ConstSpeed", "Fixed Speed", nullptr, false);
	}

	Schematyc::Range<0, 100> m_speed = 1.f;
	bool m_fixedSpeed = false;
};

class CSplineComponent : 
	public IEntityComponent 
#ifndef RELEASE
	,public IEntityComponentPreviewer
#endif
{
public:
	static CryGUID& IID()
	{
		static CryGUID id = "{C772305F-DD62-45E9-A6D4-0D63A8F28F20}"_cry_guid;
		return id;
	}

	static void ReflectType(Schematyc::CTypeDesc<CSplineComponent>& desc)
	{
		desc.SetGUID(IID());
		desc.SetEditorCategory("GameComponents");
		desc.SetLabel("SplineComponent");
		desc.SetDescription("CSplineComponent");
		desc.SetIcon("icons:General/Core.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::ClientOnly });

		desc.AddMember(&CSplineComponent::m_splineType, 'spty', "SplineType", "Spline type", nullptr, ESplineType::LINEAR);
		desc.AddMember(&CSplineComponent::m_regularRebind, 'regr', "UpdateSplinesPoints", "Update splines points", nullptr, false);
		desc.AddMember(&CSplineComponent::m_useCyclePath, 'cycl', "SetCyclePath", "Set cycle path", nullptr, false);
		desc.AddMember(&CSplineComponent::m_splineSampleOptions, 'sopt', "SampleOptions", "SampleOptions", "", SSplineSampleOptions());
		desc.AddMember(&CSplineComponent::m_splineMovementOptions, 'mopt', "MovementOtions", "MovementOptions", nullptr, SSplineMovementOptions());
		desc.AddMember(&CSplineComponent::m_moving, 'move', "EnableMoving", "Enable moving", nullptr, false);
		desc.AddMember(&CSplineComponent::m_rebindButton, 'rebb', "Rebind", "Rebind", "Rebind the linked entites manually", Serialization::FunctorActionButton<std::function<void()>>());
		desc.AddMember(&CSplineComponent::m_resetMovementButton, 'rese', "ResetMovement", "ResetMovement", "ResetMovement to start of the Spline", Serialization::FunctorActionButton<std::function<void()>>());
	}

	CSplineComponent() = default;	
	virtual ~CSplineComponent();

	// IEntityComponent
	virtual void Initialize();
	virtual void ProcessEvent(SEntityEvent& event);
	virtual uint64 GetEventMask() const;

#ifndef RELEASE
	virtual IEntityComponentPreviewer* GetPreviewer() final { return this; }
	// ~IEntityComponent
	// IEntityComponentPreviewer
	virtual void SerializeProperties(Serialization::IArchive& archive) final {}
	virtual void Render(const IEntity& entity, const IEntityComponent& component, SEntityPreviewContext &context) const final;
	// ~IEntityComponentPreviewer
#endif

public:
	void AddPoint(const Vec3 newPoint);
	void AddPoint(const Vec3 newValue, unsigned index);
	void SetPoint(const Vec3 newValue, unsigned index);
	void SetPoints(const std::vector<Vec3>& points) { m_points = points; }

	void RemovePoint() { m_points.pop_back(); };
	void RemovePoint(unsigned index) { m_points.erase(m_points.begin() + index); };
	void Clear() { m_points.clear(); };

	Vec3 GetLerp(const Vec3& p, const Vec3& q, float t) const;
	Vec3 GetLerp(const std::vector<Vec3> points, float t) const;
	Vec3 GetBezier(const std::vector<Vec3> points, float t) const;
	Vec3 GetCatmullRom(const std::vector<Vec3> points, float t) const;
	Vec3 GetPoint(float t) const;
	Vec3 GetPointSampled(float t) const;
	float GetLength(float precision = 0.01f);
	float GetLengthSampled(float precision = 0.05f);
	void Rebind();
	void Resample();

	void ResetMovement();
	bool IsFinished() const { return m_traveled >= 1.0f; }
	void Move(float fFrameTime);
	void EnableMovement(bool state) { m_moving = state; };

	std::vector<Vec3> m_points;
	ESplineType m_splineType = ESplineType::LINEAR;
	bool m_regularRebind = false;
	bool m_useCyclePath = false;

	Serialization::FunctorActionButton<std::function<void()>> m_rebindButton;
	Serialization::FunctorActionButton<std::function<void()>> m_resetMovementButton;

	std::vector<Vec3> m_pointsSampled;
	SSplineSampleOptions m_splineSampleOptions;
	SSplineMovementOptions m_splineMovementOptions;

	float m_length = 0.f;
	float m_elapsed = 0.0f;
	float m_traveled = 0.0f;
	bool m_moving = false;
};