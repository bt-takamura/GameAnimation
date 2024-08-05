// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/SNGameInstance.h"
#include "GAGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UGAGameInstance : public USNGameInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="ItemBox")
	TSoftObjectPtr<UDataTable> ItemBoxData;
};
