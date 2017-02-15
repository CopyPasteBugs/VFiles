#include "StdAfx.h"
#include "GameEvents.h"


CGameEventsSystem* pGameEventSystem = nullptr;
CGameEventsSystem* CGameEventsSystem::m_instanceSingleton = nullptr;

void IGameEventsListener::SendGameEvent(SGameEvent& event)
{
	GetGameEventSystem()->getInstance()->PushGameEvent(event);
}

void CGameEventsSystem::PushGameEvent(const SGameEvent& event)
{
	// Collect all events the same type in vector 
	pGotEventsMap[event.type].push_back(event);
}

CGameEventsSystem * GetGameEventSystem()
{
	return pGameEventSystem->getInstance();
}
