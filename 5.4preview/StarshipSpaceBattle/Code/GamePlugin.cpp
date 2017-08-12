
#include "StdAfx.h"
#include "GamePlugin.h"

#include "Components/Player.h"
#include "Spaceship/Spaceship.h"
#include "AsteroidGenerator/AsteroidGenerator.h"
#include "Asteroid/Asteroid.h"
#include "Hit/Hit.h"
#include "TestEntity/TestEntity.h"

#include <IGameObjectSystem.h>
#include <IGameObject.h>

#include <Cry3DEngine/ITimeOfDay.h>

// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>

CGamePlugin::~CGamePlugin()
{
	// Remove any registered listeners before 'this' becomes invalid
	gEnv->pGameFramework->RemoveNetworkedClientListener(*this);
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	if (gEnv->pSchematyc)
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(GetSchematycPackageGUID());
	}
}

bool CGamePlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	// Register for engine system events, in our case we need ESYSTEM_EVENT_GAME_POST_INIT to load the map
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CGamePlugin");
	// Listen for client connection events, in order to create the local player
	gEnv->pGameFramework->AddNetworkedClientListener(*this);

	return true;
}

void CGamePlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		// Called when the game framework has initialized and we are ready for game logic to start
	case ESYSTEM_EVENT_GAME_POST_INIT:
	{

		//gEnv->pConsole->ExecuteString("e_TimeOfDay 1.0");

		gEnv->pConsole->ExecuteString("e_waterocean=0");
		gEnv->pConsole->ExecuteString("e_fog=0");
		gEnv->pConsole->ExecuteString("e_clouds=0");
		gEnv->pConsole->ExecuteString("e_terrain = 0");
		gEnv->pConsole->ExecuteString("e_sun=0");
		//gEnv->pConsole->ExecuteString("e_waterOceanFFT=0");
		//gEnv->pConsole->ExecuteString("e_physOceanCell=0.0");
		gEnv->pConsole->ExecuteString("e_Wind=0");
		gEnv->pConsole->ExecuteString("s_DrawAudioDebug=1");
		

		gEnv->pConsole->ExecuteString("p_draw_helpers=0");

		// Don't need to load the map in editor
		if (!gEnv->IsEditor())
		{
			gEnv->pConsole->ExecuteString("map example", false, true);
			gEnv->pConsole->ExecuteString("p_garvity_z = 0.0");




		}
		break;
	}
	case ESYSTEM_EVENT_TIME_OF_DAY_SET:
	{

	}
	break;
	case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
	{
		// 5.4 register

		gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
			stl::make_unique<Schematyc::CEnvPackage>(
				GetSchematycPackageGUID(),
				"EntityComponents",
				"Username123",
				"Spaceship Game Components",
				[this](Schematyc::IEnvRegistrar& registrar) { RegisterComponents(registrar); }
				)
		);		
	}
	break;
	}
}

bool CGamePlugin::OnClientConnectionReceived(int channelId, bool bIsReset)
{

	ITimeOfDay* tod = gEnv->p3DEngine->GetTimeOfDay();
	tod->LoadPreset("Assets\\libs\\environmentpresets\\example.xml");
	tod->SetTime(21.0f);
	tod->Update(true, true);

	return true;
}

bool CGamePlugin::OnClientReadyForGameplay(int channelId, bool bIsReset)
{
	return true;
}

void CGamePlugin::OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient)
{
	// Client disconnected, remove the entity and from map
	auto it = m_players.find(channelId);
	if (it != m_players.end())
	{
		gEnv->pEntitySystem->RemoveEntity(it->second);

		m_players.erase(it);
	}
}

void CGamePlugin::RegisterComponents(Schematyc::IEnvRegistrar & registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CSpaceship));
		CSpaceship::Register(componentScope);
	}
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CAsteroidGenerator));
		CAsteroidGenerator::Register(componentScope);
	}
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CAsteroidComponent));
		CAsteroidComponent::Register(componentScope);
	}
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CHit));
		CHit::Register(componentScope);
	}
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CTestEntity));
		CTestEntity::Register(componentScope);
	}
}

CRYREGISTER_SINGLETON_CLASS(CGamePlugin)