cmake_minimum_required (VERSION 3.6.2)
set(CRYENGINE_DIR "C:/Program Files (x86)/Crytek/CRYENGINE Launcher/Crytek/CRYENGINE_5.4")
set(TOOLS_CMAKE_DIR "${CRYENGINE_DIR}/Tools/CMake")

set(PROJECT_BUILD 1)
set(PROJECT_DIR "C:/CryProjects/StarshipSpaceBottle")

include("${TOOLS_CMAKE_DIR}/CommonOptions.cmake")

add_subdirectory("${CRYENGINE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CRYENGINE")

include("${TOOLS_CMAKE_DIR}/Configure.cmake")
start_sources()

sources_platform(ALL)
add_sources("Code_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Root"
		"GamePlugin.cpp"
		"StdAfx.cpp"
		"GamePlugin.h"
		"StdAfx.h"
)
add_sources("Components_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Components"
		"Components/Player.cpp"
		"Components/Player.h"
)

add_sources("Spaceship_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Spaceship"
		"Spaceship/Spaceship.cpp"
		"Spaceship/Spaceship.h"
)

add_sources("Bullet_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Bullet"
		"Bullet/Bullet.cpp"
		"Bullet/Bullet.h"
)

add_sources("Asteroid_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Asteroid"
		"Asteroid/Asteroid.cpp"
		"Asteroid/Asteroid.h"
)

add_sources("AsteroidGenerator_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "AsteroidGenerator"
		"AsteroidGenerator/AsteroidGenerator.cpp"
		"AsteroidGenerator/AsteroidGenerator.h"
)

add_sources("Hit_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Hit"
		"Hit/Hit.cpp"
		"Hit/Hit.h"
)

add_sources("TestEntity_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "TestEntity"
		"TestEntity/TestEntity.cpp"
		"TestEntity/TestEntity.h"
)

end_sources()

CryEngineModule(Game PCH "StdAfx.cpp" SOLUTION_FOLDER "Project")

target_include_directories(${THIS_PROJECT}
PRIVATE 
  "${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
  "${CRYENGINE_DIR}/Code/CryEngine/CryAction"
	"${CRYENGINE_DIR}/Code/CryEngine/CrySchematyc/Core/Interface"
	"${CRYENGINE_DIR}/Code/CryPlugins/CryDefaultEntities/Module"
)

# Set StartUp project in Visual Studio
set_solution_startup_target(${THIS_PROJECT})

if (WIN32)
    set_visual_studio_debugger_command( ${THIS_PROJECT} "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"C:/CryProjects/StarshipSpaceBottle/Game.cryproject\"" )
endif()

#BEGIN-CUSTOM
# Make any custom changes here, modifications outside of the block will be discarded on regeneration.
#END-CUSTOM