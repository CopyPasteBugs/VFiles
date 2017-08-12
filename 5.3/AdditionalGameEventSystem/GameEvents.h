#pragma once
#include "StdAfx.h"

#include "GamePlugin.h"
#include <algorithm>

class CGameEventsSystem;

enum EGameEvents : const unsigned __int64
{
	GE_NOTHING					= BIT64(0),
	GE_SHOT						= BIT64(1),
	GE_DEATH					= BIT64(2),
	GE_SPAWN					= BIT64(3),
	GE_TURNOFF_ALL_ELEVATORS	= BIT64(4),
	GE_UNNAMED_EVENT_1			= BIT64(5),
	GE_UNNAMED_EVENT_2			= BIT64(6),
	GE_UNNAMED_EVENT_3			= BIT64(7),
	GE_UNNAMED_EVENT_4			= BIT64(8),
	GE_UNNAMED_EVENT_5			= BIT64(9),
	GE_UNNAMED_EVENT_6			= BIT64(10),
	GE_UNNAMED_EVENT_7			= BIT64(11),

	// Add more if needed, allowed 63 events total count

	GE_LAST = BIT64(63)
};

// List of allowed events for processing by GameEventSystem 
static EGameEvents AllowedToProcessEvents[] = 
{ 
	GE_SHOT,
	GE_DEATH,
	GE_SPAWN,
	GE_TURNOFF_ALL_ELEVATORS,
	GE_UNNAMED_EVENT_1,
	GE_UNNAMED_EVENT_2,
	GE_UNNAMED_EVENT_3,
	GE_UNNAMED_EVENT_4,
	GE_UNNAMED_EVENT_5,
	GE_UNNAMED_EVENT_6,
	GE_UNNAMED_EVENT_7
};

CGameEventsSystem* GetGameEventSystem();


typedef struct SGameEvent
{
	EGameEvents type;
	int value;

	SGameEvent() { type = GE_NOTHING; };
	SGameEvent(EGameEvents type_, int value_ = 0) { type = type_; value = value_; };

} SGameEvent;

struct IGameEventsListener
{
	virtual ~IGameEventsListener() {};
	// ! events to entity
	virtual void ProcessGameEvent(const SGameEvent& event) {};
	// ! Set mask for subscribe to game events return (OnShot | OnDeath | OnSpawn )
	virtual const int64 GetGameEventsMask() const { return 0; }
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

	std::map<EGameEvents, gameListeners> pListenersMap;
	std::map<EGameEvents, gameEventsPool> pGotEventsMap;

	static CGameEventsSystem *m_instanceSingleton;
};