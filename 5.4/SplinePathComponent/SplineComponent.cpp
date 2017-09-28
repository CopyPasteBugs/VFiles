#include "StdAfx.h"
#include "SplineComponent.h"

CSplineComponent::~CSplineComponent()
{
}

void CSplineComponent::Initialize()
{
	m_rebindButton = Serialization::ActionButton(std::function<void()>([this]() { Rebind(); }));
	m_resetMovementButton = Serialization::ActionButton(std::function<void()>([this]() { ResetMovement(); }));

	m_pEntity->SetUpdatePolicy(EEntityUpdatePolicy::ENTITY_UPDATE_VISIBLE);
}

void CSplineComponent::ProcessEvent(SEntityEvent & event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_START_GAME:
	{
		Rebind();
		Resample();
		m_length = GetLength();

		if (m_regularRebind)
			m_pEntity->SetTimer(1, 100);
	}
	break;
	case ENTITY_EVENT_TIMER:
	{
		//int TimerId = (int)event.nParam[0];	
		if (m_regularRebind)
		{
			m_pEntity->SetTimer(1, 100); // Update timer
			Rebind();
		}
	}
	break;
	case ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED:
	{
		Rebind();
		m_length = GetLength();

		if (m_splineType == ESplineType::LINEAR)
			if (m_splineSampleOptions.m_useSampledSpline)
				m_splineSampleOptions.m_useSampledSpline = false;

		if (m_regularRebind)
			m_pEntity->SetTimer(1, 100);

		if (m_splineSampleOptions.m_useSampledSpline)
			Resample();
	
	}
	break;
	case ENTITY_EVENT_UPDATE:
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		if (m_moving)
		{
			if (IsFinished())
				ResetMovement();

			Move(pCtx->fFrameTime);
		}
	}
	}
}

uint64 CSplineComponent::GetEventMask() const
{
	return BIT64(ENTITY_EVENT_START_GAME) | BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_TIMER) | BIT64(ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED);
}
#ifndef RELEASE
void CSplineComponent::Render(const IEntity & entity, const IEntityComponent & component, SEntityPreviewContext & context) const
{
	if (context.bSelected)
	{
		Matrix34 transform = GetEntity()->GetWorldTM();

		if (m_points.size() < 1) return;

		IRenderAuxGeom* render = gEnv->pRenderer->GetIRenderAuxGeom();
		Vec3 p = GetPoint(0.0f);
		//ColorB c = context.debugDrawInfo.color;
		ColorB c = ColorB(255, 255, 0);
		Vec3 q;

		for (float t = 0.01f; t <= 1.0f; t = t + 0.01f)
		{
			if (m_splineSampleOptions.m_useSampledSpline && (m_splineType != ESplineType::LINEAR))
				q = GetPointSampled(t);
			else
				q = GetPoint(t);
			
			render->DrawLine(p, c, q, c, 2.0f);
			p = q;
		}

		//for (std::vector<Vec3>::const_iterator i = m_points.begin(); i != m_points.end(); ++i)
		//	render->DrawPoint(*i, c);

		//AABB aabb;
		//m_pEntity->GetWorldBounds(aabb);
		//render->DrawAABB(aabb, false, context.debugDrawInfo.color, EBoundingBoxDrawStyle::eBBD_Faceted);
	}
}
#endif

void CSplineComponent::AddPoint(const Vec3 newPoint)
{
	m_points.push_back(newPoint);
}

void CSplineComponent::AddPoint(const Vec3 newValue, unsigned index)
{
	if (index > m_points.size())
		index = m_points.size();

	if (m_points.size())
	{
		m_points.insert(m_points.begin() + index, newValue);
	}
	else if (m_points.empty())
	{
		m_points.push_back(newValue);
	}
}

void CSplineComponent::SetPoint(const Vec3 newValue, unsigned index)
{
	if (index < m_points.size())
	{
		if (m_points.size())
			m_points[index] = newValue;
		else if (m_points.empty())
			m_points.push_back(newValue);
	}
}

Vec3 CSplineComponent::GetLerp(const Vec3 & p, const Vec3 & q, float t) const
{
	return Vec3::CreateLerp(p,q,t);
}

Vec3 CSplineComponent::GetLerp(const std::vector<Vec3> points, float t) const
{
	if (points.size() < 2)
		return ZERO;
	else
	{
		if (t >= 1.f)
			return points.back();

		int baseIdx = CLAMP((int)(t * (points.size() - 1)), 0, (int)(points.size() - 2));
		t = fmodf(t * (points.size() - 1), 1.f);
		return GetLerp(points[baseIdx], points[baseIdx + 1], t);
	}
}

Vec3 CSplineComponent::GetBezier(const std::vector<Vec3> points, float t) const
{
	if (points.size() == 2)
	{
		return GetLerp(points[0], points[1], t);
	}
	else
	{
		std::vector<Vec3> interpolatedPoints;
		for (unsigned i = 1; i < points.size(); i++)
		{
			interpolatedPoints.push_back(GetLerp(points[i - 1], points[i], t));
		}
		return GetBezier(interpolatedPoints, t);
	}
}

Vec3 CatmullRom(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t, float t2, float t3)
{
	return Vec3(0.5f * ((2.0f * p1) + (-p0 + p2) * t +
		(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3));
}

Vec3 CSplineComponent::GetCatmullRom(const std::vector<Vec3> points, float t) const
{
	if (points.size() < 4)
		return ZERO;
	else
	{
		if (t >= 1.f)
			return points[points.size() - 2];

		int baseIdx = static_cast<int>(t * (points.size() - 3));
		t = fmodf(t * (points.size() - 3), 1.f);
		float t2 = t * t;
		float t3 = t2 * t;
		return CatmullRom(points[baseIdx], points[baseIdx + 1], points[baseIdx + 2], points[baseIdx + 3], t, t2, t3);
	}
}

Vec3 CSplineComponent::GetPoint(float t) const
{
	if (m_points.size() < 2)
		return m_points.size() == 1 ? m_points[0] : ZERO;

	if (t > 1.f)
		t = 1.f;
	else if (t < 0.f)
		t = 0.f;


	switch (m_splineType)
	{
	case ESplineType::LINEAR:
		return GetLerp(m_points, t);
	case ESplineType::BEZIER:
		return GetBezier(m_points, t);
	case ESplineType::CATMULLROM:
	{
		std::vector<Vec3> extendedPoints;
		if (m_points.size() > 1)
		{
			// Non-cyclic case: duplicate start and end
			float epsilon = (m_points.front() - m_points.back()).GetLength();

			if (epsilon < 0.01f)
			{
				extendedPoints.push_back(m_points.front());

				for each(const Vec3& v in m_points)
					extendedPoints.push_back(v);

				extendedPoints.push_back(m_points.back());
			}
			// Cyclic case: smooth the tangents
			else
			{
				extendedPoints.push_back(m_points[m_points.size() - 2]);

				for each(const Vec3& v in m_points)
					extendedPoints.push_back(v);

				extendedPoints.push_back(m_points[1]);
			}
		}
		return GetCatmullRom(extendedPoints, t);
	}
	}

	return ZERO;
}

Vec3 CSplineComponent::GetPointSampled(float t) const
{
	if (m_pointsSampled.size() < 2)
		return m_pointsSampled.size() == 1 ? m_pointsSampled[0] : ZERO;

	if (t > 1.f)
		t = 1.f;
	else if (t < 0.f)
		t = 0.f;

	// Use sampled spline just interpolate baked nearest points
	return GetLerp(m_pointsSampled, t);
}

float CSplineComponent::GetLength(float precision)
{
	if (m_points.size() <= 0)
		return 0.0f;

	float length = 0.f;

	Vec3 a = m_points[0];
	for (float t = 0.000f; t <= 1.000f; t += precision)
	{
		Vec3 b = GetPoint(t);
		length += (a - b).GetLength();
		a = b;
	}

	return length;
}

float CSplineComponent::GetLengthSampled(float precision)
{
	if (m_pointsSampled.size() <= 0)
		return 0.0f;

	float length = 0.f;

	Vec3 a = m_pointsSampled[0];
	for (float t = 0.000f; t <= 1.000f; t += precision)
	{
		Vec3 b = GetPointSampled(t);
		length += (a - b).GetLength();
		a = b;
	}

	return length;
}

void CSplineComponent::Rebind()
{
	IEntityLink* link = m_pEntity->GetEntityLinks();

	//AABB aabb = AABB(Vec3(-1.0f),Vec3(1.0f));
	//aabb.Reset();
	//Vec3 origin = m_pEntity->GetWorldPos();
	//Matrix34 tranform = m_pEntity->GetWorldTM().GetInverted();

	if (link)
	{
		Clear();

		while (link)
		{
			IEntity* linkedEntity = gEnv->pEntitySystem->GetEntity(link->entityId);
			if (linkedEntity)
			{
				Vec3 pos = linkedEntity->GetWorldPos();
				AddPoint(pos);
				//aabb.Add(tranform.TransformPoint(pos)); // inverted matrix for local transforms
			}

			link = link->next;
		}
	
		if (m_useCyclePath)
		{
			if (m_points.size() > 1)
				AddPoint(m_points[0]);
		}
		//m_pEntity->SetLocalBounds(aabb, true);
	}
}

void CSplineComponent::Resample()
{
	m_pointsSampled.clear();	
	m_pointsSampled.reserve((int)m_splineSampleOptions.m_SampleQuality.value);
	float step = 1.0f / m_splineSampleOptions.m_SampleQuality.value;

	m_pointsSampled.push_back(GetPoint(0.0f));
	for (float t = step; t <= 1.0f; t = t + step) 
	{
		Vec3 p = GetPoint(t);
		m_pointsSampled.push_back(p);
	}
	m_pointsSampled.push_back(GetPoint(1.0f));
}

void CSplineComponent::ResetMovement()
{
	m_traveled = 0.f;
	m_elapsed = 0.f;
}

void CSplineComponent::Move(float fFrameTime)
{
	if (m_traveled >= 1.0f || m_length <= 0.0f)
		return;

	m_elapsed += fFrameTime;

	float distanceCovered = m_elapsed * m_splineMovementOptions.m_speed;
	m_traveled = distanceCovered / m_length;

	Vec3 pos;
	if (m_splineSampleOptions.m_useSampledSpline && (m_splineType != ESplineType::LINEAR))
		pos = GetPointSampled(m_traveled);
	else
		pos = GetPoint(m_traveled);

	m_pEntity->SetPos(pos);

}
