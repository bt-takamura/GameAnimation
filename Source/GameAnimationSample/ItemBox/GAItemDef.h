// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAItemDef.generated.h"

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
UENUM()
enum EItemType:uint8
{
	EItemType_None = 0xff UMETA(DisplayName = "Item Type is None."),
};

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
USTRUCT(BlueprintType)
struct FItemElement
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Category="ItemBox", DisplayName="アイテムが選択される比率")
	int Ratio = 0;
	
	UPROPERTY(EditAnywhere, Category="ItemBox", DisplayName="アイテムのタイプ")
	TEnumAsByte<EItemType> Type = EItemType_None;
};

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Category = "ItemBox", DisplayName = "アイテムボックスの設定")
	TArray<FItemElement> Items;
};
