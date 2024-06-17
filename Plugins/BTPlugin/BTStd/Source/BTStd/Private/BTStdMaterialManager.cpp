// Copyright Epic Games, Inc. All Rights Reserved.

#include "Materials/BTStdMaterialManager.h"

#include "BTCoreDebugTypes.h"

#include "Materials/BTStdMaterialUtils.h"

static void ShowMaterialMenu(const TArray<FString>& Args, UWorld* InWorld);

namespace BeeTribe::Material
{
	BT_DECLARE_DEBUGCOMMAND_WITH_WORLD_AND_ARGS(ShowMaterialMenu, TEXT("Material."), TEXT("マテリアルメニューを表示します"), FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(ShowMaterialMenu), 0);
}

void ShowMaterialMenu(const TArray<FString>& Args, UWorld* InWorld)
{
	if (FBTStdMaterialManager* Manager = FBTStdMaterialUtils::GetMaterialManager(InWorld))
	{

	}
}

bool FBTStdMaterialManager::Initialize()
{
	return true;
}

void FBTStdMaterialManager::Finalize()
{
}

void FBTStdMaterialManager::Tick(float DeltaTime, bool bIsPaused)
{

}

FBTStdMaterialParamChangerBase* FBTStdMaterialManager::GetChanger(const FBTStdMaterialParamChangerHandle& InHandle)
{
	if (const uint64* Id = ChangerHandleMap.Find(InHandle.m_pChanger))
	{
		if (*Id == InHandle.m_UniqueId)
		{
			if (!InHandle.m_pChanger->IsRestored() && !InHandle.m_pChanger->IsAborted())
			{
				return const_cast<FBTStdMaterialParamChangerBase*>(InHandle.m_pChanger);
			}
		}
	}

	return nullptr;
}

uint64 FBTStdMaterialManager::MakeUniqueId()
{
	static uint64 s_LastUniqueId = 0;
	return ++s_LastUniqueId;
}