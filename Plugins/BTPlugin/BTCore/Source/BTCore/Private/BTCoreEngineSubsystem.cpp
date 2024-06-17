// Copyright Epic Games, Inc. All Rights Reserved.


#include "BTCoreEngineSubsystem.h"

#include "BTCoreDebugTypes.h"

UBTCoreEngineSubsystem::UBTCoreEngineSubsystem()
{

}
#if !UE_BUILD_SHIPPING
bool UBTCoreEngineSubsystem::GetConsoleCommandNames(TArray<FString>& OutNames) const
{
	return IBTCoreConsoleManager::Get().GetConsoleVariableNames(OutNames);
}

FBTCoreConsoleVariableRef* UBTCoreEngineSubsystem::GetConsoleVariableRef(const FString& InName) const
{
	return IBTCoreConsoleManager::Get().GetConsoleVariableRef(InName);
}

bool UBTCoreEngineSubsystem::GetConsoleWithWorldAndArgsNames(TArray<FString>& OutNames) const
{
	return IBTCoreConsoleManager::Get().GetConsoleWithWorldAndArgsNames(OutNames);
}

FBTCoreConsoleWithWorldAndArgs* UBTCoreEngineSubsystem::GetConsoleWithWorldAndArgs(const FString& InName) const
{
	return IBTCoreConsoleManager::Get().GetConsoleWithWorldAndArgs(InName);
}
#endif//UE_BUILD_SHIPPING
void UBTCoreEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UBTCoreEngineSubsystem::Deinitialize()
{
}