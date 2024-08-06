// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAnimationSample/ItemBox/GAItemDef.h"
#include "System/SNGameInstance.h"
#include "GAGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UGAGameInstance : public USNGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;

	const FItemData* GetItemData(const FName& Name) const ;
	
private:
	UPROPERTY(EditAnywhere, Category="ItemBox")
	TSoftObjectPtr<UDataTable> ItemDataObject = nullptr;

	UPROPERTY()
	TObjectPtr<UDataTable> ItemDataTable;
};
