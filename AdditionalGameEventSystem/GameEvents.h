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

	bool AddEventListener(IGameEventsListener* pListener);
	bool RemoveEventListener(IGameEventsListener* pListener);
	void PushGameEvent(const SGameEvent& event);
	void OnUpdateDispatch();

private:
	CGameEventsSystem() {};
	~CGameEventsSystem() {};

	std::map<GameEvents, gameListeners> pListenersMap;
	std::map<GameEvents, gameEventsPool> pGotEventsMap;

	static CGameEventsSystem *m_instanceSingleton;
};