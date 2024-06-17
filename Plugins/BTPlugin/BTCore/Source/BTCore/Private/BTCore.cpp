// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTCore.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#endif

#include "BTCorePrivate.h"

#if BT_DEVTOOL_IS_ENABLED

#include "WebSocketsModule.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

#endif

#include "BTCoreTickableManager.h"
#include "BTCoreDebugManager.h"

DEFINE_LOG_CATEGORY(LogBTCore);

#define LOCTEXT_NAMESPACE "BTCoreModule"

#if !UE_BUILD_SHIPPING
static TSharedPtr<IBTCoreTickableInterface> CreateBTCoreDebugManager(TObjectPtr<UWorld> InWorld, int32 InPriority)
{
	return MakeShared<FBTCoreDebugManager>(InWorld, InPriority);
}
#endif

void FBTCoreModule::StartupModule()
{
	if (IsRunningCommandlet())
	{
		return;
	}

#if !UE_BUILD_SHIPPING
	const TSharedRef<FBTCoreTickableSpawnerManager>& SpawnerManager = FBTCoreTickableSpawnerManager::Get();
	SpawnerManager->RegisterTickableInterface<FBTCoreDebugManager>(
		FBTCoreTickableEntry::FOnSpawnTickableInterface::CreateStatic(CreateBTCoreDebugManager));
#endif
	//FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FBTCoreModule::Initialize);
	//Initialize();
}

void FBTCoreModule::ShutdownModule()
{
#if !UE_BUILD_SHIPPING
	const TSharedRef<FBTCoreTickableSpawnerManager>& SpawnerManager = FBTCoreTickableSpawnerManager::Get();
	SpawnerManager->UnregisterTickableInterface<FBTCoreDebugManager>();
#endif
}

void FBTCoreModule::Initialize()
{
#if BT_DEVTOOL_IS_ENABLED
	FModuleManager::LoadModuleChecked<FWebSocketsModule>("WebSockets");

	FString BTConnectorId;
	FParse::Value(FCommandLine::Get(), TEXT("BTConnectorId="), BTConnectorId);

	FString ServerURL;
	if (!FParse::Value(FCommandLine::Get(), TEXT("BTConnectorURL="), ServerURL))
	{

		FString ServerIP;
		uint16 ServerPort = 8888;
		if (!FParse::Value(FCommandLine::Get(), TEXT("BTConnectorIP="), ServerIP) ||
			!FParse::Value(FCommandLine::Get(), TEXT("BTConnectorPort="), ServerPort))
		{
			UE_LOG(LogBTCore, Log, TEXT("BT is disabled, provide `BTConnectorIP` and `BTConnectorPort` cmd-args to enable it"));
			return;
		}
		UE_LOG(LogBTCore, Warning, TEXT("BTConnectorIP and BTConnectorPort are deprecated flags. Use BTConnectorURL instead. eg. -BTConnectorURL=ws://%s:%d"), *ServerIP, ServerPort);
		ServerURL = FString::Printf(TEXT("ws://%s:%d"), *ServerIP, ServerPort);
	}

	UE_LOG(LogBTCore, Log, TEXT("BTConnector endpoint ID: %s"), *BTConnectorId);

	if (!ensure(GEngine != nullptr))
	{
		return;
	}

#endif

	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBTCoreModule, BTCore)