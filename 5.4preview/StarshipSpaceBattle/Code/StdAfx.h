// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryCore/Project/CryModuleDefs.h>
#define eCryModule eCryM_Game
#define GAME_API   DLL_EXPORT

#include <CryCore/Platform/platform.h>
#include <CrySystem/ISystem.h>
#include <Cry3DEngine/I3DEngine.h>
#include <CryNetwork/ISerialize.h>

#include <CryEntitySystem/IEntitySystem.h>
#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/CoreAPI.h>
#include <CryAudio/IAudioSystem.h>
#include <CryAudio/IAudioInterfacesCommonData.h>

#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Utils/EnumFlags.h>
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Utils/SharedString.h>

enum GAME_COLLITION_CLASSES : uint32 
{
	GCC_SPACESHIP = collision_class_game << 0,
	GCC_ASTERIOD = collision_class_game << 1,
	GCC_BULLET = collision_class_game << 2,
	GCC_INVISIBLE_PLANE = collision_class_game << 3
};

//struct IEntityRegistrator
//{
//	IEntityRegistrator()
//	{
//		if (g_pFirst == nullptr)
//		{
//			g_pFirst = this;
//			g_pLast = this;
//		}
//		else
//		{
//			g_pLast->m_pNext = this;
//			g_pLast = g_pLast->m_pNext;
//		}
//	}
//
//	virtual void Register() = 0;
//
//public:
//	IEntityRegistrator *m_pNext;
//
//	static IEntityRegistrator *g_pFirst;
//	static IEntityRegistrator *g_pLast;
//};
//
//template<class T>
//static IEntityClass* RegisterEntityWithDefaultComponent(const char* name, const char* editorCategory = "", const char* editorIcon = "", bool bIconOnTop = false)
//{
//	IEntityClassRegistry::SEntityClassDesc clsDesc;
//	clsDesc.sName = name;
//
//	clsDesc.editorClassInfo.sCategory = editorCategory;
//	clsDesc.editorClassInfo.sIcon = editorIcon;
//	clsDesc.editorClassInfo.bIconOnTop = bIconOnTop;
//
//	struct CObjectCreator
//	{
//		static IEntityComponent* Create(IEntity* pEntity, SEntitySpawnParams& params, void* pUserData)
//		{
//			return pEntity->GetOrCreateComponentClass<T>();
//		}
//	};
//
//	clsDesc.pUserProxyCreateFunc = &CObjectCreator::Create;
//	clsDesc.flags |= ECLF_INVISIBLE;
//
//	return gEnv->pEntitySystem->GetClassRegistry()->RegisterStdClass(clsDesc);
//};