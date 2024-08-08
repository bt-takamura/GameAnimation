// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SNOnlineCommonSetting.generated.h"

/** Specifies the online features and connectivity that should be used for a game session */
UENUM(BlueprintType)
enum class ESessionOnlineMode : uint8
{
	Offline,
	LAN,
	Online
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SNPLUGIN_API USNOnlineHostSessionRequest : public UObject
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnyWhere, Category=Session)
	ESessionOnlineMode OnlineMode = ESessionOnlineMode::LAN;
	
	UPROPERTY(EditAnyWhere, Category="Online")
	bool bShouldIdAdvertise = true;
	
	UPROPERTY(EditAnyWhere, Category="Online")
	bool bAllowJoinInProgress = true;
	
	UPROPERTY(EditAnyWhere, Category="Online")
	bool bAllowInvites = true;
	
	UPROPERTY(EditAnyWhere, Category="Online")
	bool bUsesPresence = true;
	
	UPROPERTY(EditAnyWhere, Category="Online")
	bool bAllowJoinViaPresense =true;
	
	UPROPERTY(EditAnyWhere, Category="Online")
	bool bUseLobbiesIfAvailable = true;
	
	UPROPERTY(EditAnyWhere, Category="Online")
	bool bUseLobbiesVoiceChatIfAvailable = true;
	
	UPROPERTY()
	int MaxPlayerNum = -1;
};
