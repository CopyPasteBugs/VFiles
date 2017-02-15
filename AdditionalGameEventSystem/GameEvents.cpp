#include "StdAfx.h"
#include "GameEvents.h"


CGameEventsSystem* pGameEventSystem = nullptr;
CGameEventsSystem* CGameEventsSystem::m_instanceSingleton = nullptr;

void IGameEventsListener::SendGameEvent(SGameEvent& event)
{
	GetGameEventSystem()->getInstance()->PushGameEvent(event);
}

bool CGameEventsSystem::AddEventListener(IGameEventsListener * pListener)
{
	bool ret = false;

	for (const GameEvents e : GameEventsAll)
	{
		if (pListener->GetGameEventsMask() & (int)e)
		{
			pListenersMap[e].push_back(pListener);
			ret = true;
		}
	}

	return ret;
}

bool CGameEventsSystem::RemoveEventListener(IGameEventsListener * pListener)
{
	bool ret = false;

	for (const GameEvents e : GameEventsAll)
	{
		gameListenersIterator it = std::find(pListenersMap[e].begin(), pListenersMap[e].end(), pListener);

		if (it != pListenersMap[e].end())
			pListenersMap[e].erase(it);
	}

	return ret;
}

void CGameEventsSystem::PushGameEvent(const SGameEvent& event)
{
	// Collect all events the same type in vector 
	pGotEventsMap[event.type].push_back(event);
}

void CGameEventsSystem::OnUpdateDispatch()
{
	// Go through all types of event
	for (const GameEvents e : GameEventsAll)
	{
		// for each listener from list of listenerers
		for (auto &listener : pListenersMap[e])
		{
			// if this listener have mask with this type of event, then send all collected event(s) of this type for this listener
			if ((listener)->GetGameEventsMask() & e)
			{
				// send all events of E type to listener
				for (auto &event : pGotEventsMap[e])
				{
					(listener)->OnGameEvent((event));
				}
			}
		}
		// So, now we send all event (of this type), and for now we may clear the pool
		pGotEventsMap[e].clear();
	}
};

CGameEventsSystem * GetGameEventSystem()
{
	return pGameEventSystem->getInstance();
}
