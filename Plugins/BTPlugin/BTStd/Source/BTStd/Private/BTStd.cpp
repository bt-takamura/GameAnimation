// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTStd.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#endif

#include "BTStdPrivate.h"

#if BT_DEVTOOL_IS_ENABLED

#include "WebSocketsModule.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

#endif

#include "BTCoreTickableManager.h"

#include "Materials/BTStdMaterialManager.h"
#include "Effects/BTStdEffectManager.h"

DEFINE_LOG_CATEGORY(LogBTStd);

#define LOCTEXT_NAMESPACE "BTStdModule"

static TSharedPtr<IBTCoreTickableInterface> CreateBTStdMaterialManager(TObjectPtr<UWorld> InWorld, int32 InPriority)
{
	return MakeShared<FBTStdMaterialManager>(InWorld, InPriority);
}

static TSharedPtr<IBTCoreTickableInterface> CreateBTStdEffectManager(TObjectPtr<UWorld> InWorld, int32 InPriority)
{
	return MakeShared<FBTStdEffectManager>(InWorld, InPriority);
}

void FBTStdModule::StartupModule()
{
	if (IsRunningCommandlet())
	{
		return;
	}

	const TSharedRef<FBTCoreTickableSpawnerManager>& SpawnerManager = FBTCoreTickableSpawnerManager::Get();
	SpawnerManager->RegisterTickableInterface<FBTStdMaterialManager>(
		FBTCoreTickableEntry::FOnSpawnTickableInterface::CreateStatic(CreateBTStdMaterialManager), FBTStdMaterialManager::PRIORITY);
	SpawnerManager->RegisterTickableInterface<FBTStdEffectManager>(
		FBTCoreTickableEntry::FOnSpawnTickableInterface::CreateStatic(CreateBTStdEffectManager), FBTStdEffectManager::PRIORITY);

	FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FBTStdModule::Initialize);
}

void FBTStdModule::ShutdownModule()
{
	const TSharedRef<FBTCoreTickableSpawnerManager>& SpawnerManager = FBTCoreTickableSpawnerManager::Get();
	SpawnerManager->UnregisterTickableInterface<FBTStdMaterialManager>();
	SpawnerManager->UnregisterTickableInterface<FBTStdEffectManager>();
}

void FBTStdModule::Initialize()
{
#if BT_DEVTOOL_IS_ENABLED

#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBTStdModule, BTStd)