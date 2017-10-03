#include "StdAfx.h"
#include "SplineMoverComponent.h"

CSplineMoverComponent::~CSplineMoverComponent()
{
}

void CSplineMoverComponent::Initialize()
{
	m_resetButton = Serialization::ActionButton(std::function<void()>([this]() { Reset(); }));

#ifndef RELEASE
	if (gEnv->IsEditor())
		m_pEntity->SetTimer(2, 200);
#endif

}

void CSplineMoverComponent::ProcessEvent(SEntityEvent & event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_START_GAME:
	{
		Reset();

		break;
	}
	case ENTITY_EVENT_TIMER:
	{
#ifndef RELEASE
		int TimerId = (int)event.nParam[0];

		if (gEnv->IsEditor())
			if (TimerId == 2) 
			{
				Reset();
			}
#endif

		break;
	}
	case ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED:
	{
		Reset();
		break;
	}
	case ENTITY_EVENT_UPDATE:
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];
		
		if (m_EnableMovement)
			Update(pCtx->fFrameTime);
		
		break;
	}
	}
}

uint64 CSplineMoverComponent::GetEventMask() const
{
	return BIT64(ENTITY_EVENT_START_GAME) | BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_TIMER) | BIT64(ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED);
}

void CSplineMoverComponent::Update(float fFrameTime)
{
	if (!m_SplinePath) return;

	m_Move = fFrameTime * m_Speed; // units to move

	// between current position and target
	float distance = m_Lookahead.GetDistance(m_Pos);

	// new m_Lookahead pos
	if (distance <= 0.01f) 
	{	
		m_Delta += m_LookaheadStep;

		if (m_Delta >= 1.0f) m_Delta = 0.0f;

		if (m_SplinePath->m_splineSampleOptions.m_useSampledSpline)
			m_Lookahead = m_SplinePath->GetPointSampled(m_Delta);
		else
			m_Lookahead = m_SplinePath->GetPoint(m_Delta);
	}

	// how much we're actually moving this iteration
	float actual = min(m_Move, distance);
	// the normalized vector between lookahead and position
	Vec3 direction = (m_Lookahead - m_Pos).GetNormalized();
	// move my position accordingly
	m_Pos += direction * actual;
	// update move to be the remaining amount we need to move
	m_Move -= actual;

	m_pEntity->SetPos(m_Pos);
}

void CSplineMoverComponent::Reset()
{
	m_SplinePath = GetSplinePath();
	if (m_SplinePath == nullptr) return;

	if (m_SplinePath->m_splineSampleOptions.m_useSampledSpline)
	{
		m_LookaheadStep = 1.0f / m_LookaheadOverallSteps.value;
		m_Delta = m_LookaheadStep;
		m_Pos = m_SplinePath->GetPointSampled(0.0f);
		m_Lookahead = m_SplinePath->GetPointSampled(m_Delta);
	}
	else
	{
		m_LookaheadStep = 1.0f / m_LookaheadOverallSteps.value;
		m_Delta = m_LookaheadStep;
		m_Pos = m_SplinePath->GetPoint(0.0f);
		m_Lookahead = m_SplinePath->GetPointSampled(m_Delta);
	}
}

CSplineComponent * CSplineMoverComponent::GetSplinePath()
{
	IEntityLink* link = m_pEntity->GetEntityLinks();
	CSplineComponent* spline = nullptr;

	if (link)
	{
		while (link)
		{
			IEntity* linkedEntity = gEnv->pEntitySystem->GetEntity(link->entityId);
			if (linkedEntity)
			{
				spline = linkedEntity->GetComponent<CSplineComponent>();
				if (spline) break;
			}

			link = link->next;
		}
	}

	return spline;
}
