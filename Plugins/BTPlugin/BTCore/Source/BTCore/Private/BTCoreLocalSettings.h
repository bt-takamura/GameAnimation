// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BTCoreLocalSettings.generated.h"

UCLASS(config = BTCoreLocal, configdonotcheckdefaults)
class UBTCoreLocalSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, config, Category = "Debug", Meta = (DisplayName = "お気に入りコンソールリスト"))
	TArray<FString> m_FavoriteConsoleList;

public:
	UBTCoreLocalSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("BTCoreLocal");
	}
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif
};