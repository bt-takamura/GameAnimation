// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

#include "SNRelevantAssetDataTable.generated.h"

//----------------------------------------------------------------------//
//
//! @brief 関連するアセットを登録するデータテーブル
//
//----------------------------------------------------------------------//
USTRUCT()
struct FSNRelevantAssetDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UObject> Asset;
};

