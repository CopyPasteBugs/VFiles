#pragma once
#include "StdAfx.h"

#include "GamePlugin.h"
#include <algorithm>

class CGameEventsSystem;

typedef enum GameEvents
{
	OnNothing	= BIT(0),
	OnShot		= BIT(1),
	OnDeath		= BIT(2),
	OnSpawn		= BIT(3),
} GameEvents;

static GameEvents GameEventsAll[] = {	OnShot, 
										OnDeath, 
										OnSpawn};

CGameEventsSystem* GetGameEventSystem();


typedef struct SGameEvent
{
	GameEvents type;
	int value;

	SGameEvent() { type = OnNothing; };
	SGameEvent(GameEvents type_, int value_ = 0) { type = type_; value = value_; };

} SGameEvent;

struct IGameEventsListener
{
	virtual ~IGameEventsListener() {};
	// ! events to entity
	virtual void OnGameEvent(const SGameEvent& event) = 0;
	// ! Set mask for subscribe to game events return (OnShot | OnDeath | OnSpawn )
	virtual int GetGameEventsMask() const { return 0; }
	void SendGameEvent(SGameEvent& event);
};

class CGameEventsSystem
{
private:
	CGameEventsSystem() {};
	~CGameEventsSystem() {};

	static CGameEventsSystem *m_instanceSingleton;

public:
	typedef std::vector<IGameEventsListener*> gameListeners;
	typedef std::vector<IGameEventsListener*>::iterator gameListenersIterator;
	typedef std::vector<SGameEvent> gameEventsPool;
	typedef std::vector<SGameEvent>::iterator gameEventsPoolIterator;

public:
	static CGameEventsSystem* getInstance() {

		return (!m_instanceSingleton) ?
			m_instanceSingleton = new CGameEventsSystem :
			m_instanceSingleton;
	}

	bool AddEventListener(IGameEventsListener* pListener)
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

	bool RemoveEventListener(IGameEventsListener* pListener)
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

	void PushGameEvent(const SGameEvent& event);

	void OnUpdateDispatch() 
	{
		// Go through all types of event
		for (const GameEvents e : GameEventsAll)
		{
			// Get listener from list
			for (gameListenersIterator listener = pListenersMap[e].begin(); listener != pListenersMap[e].end(); listener++)
			{
				// if this listener have mask with this type of event, then send all collected event(s) of this type for this listener
				if ((*listener)->GetGameEventsMask() & e)
				{
					for (gameEventsPoolIterator event = pGotEventsMap[e].begin(); event != pGotEventsMap[e].end(); event++)
					{
						(*listener)->OnGameEvent((*event));
					}
				}
			}

			// So, now we send all event (of this type), and for now we may clear the pool
			pGotEventsMap[e].clear();
		}
	};

protected:
	std::map<GameEvents, gameListeners> pListenersMap;
	std::map<GameEvents, gameEventsPool> pGotEventsMap;
};