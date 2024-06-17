// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"

#include "BTCoreEngineSubsystem.generated.h"

UCLASS()
class UBTCoreEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UBTCoreEngineSubsystem();

#if !UE_BUILD_SHIPPING
	bool GetConsoleCommandNames(TArray<FString>& OutNames) const;
	class FBTCoreConsoleVariableRef* GetConsoleVariableRef(const FString& InName) const;

	bool GetConsoleWithWorldAndArgsNames(TArray<FString>& OutNames) const;
	class FBTCoreConsoleWithWorldAndArgs* GetConsoleWithWorldAndArgs(const FString& InName) const;
#endif//UE_BUILD_SHIPPING
private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};