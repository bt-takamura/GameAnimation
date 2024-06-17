// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SNContentsAsset.generated.h"

class USNContentAssetManager;

//!@{@defgroup コンテンツアセット
//!@{
//----------------------------------------------------------------------//
//
//! @brief ダウンロードコンテンツ用アセット管理クラス
//
//----------------------------------------------------------------------//
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class SNPLUGIN_API USNContentsAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	
	//! @{@name コンテンツオブジェクトを取得
	const TMap<FName, TSoftObjectPtr<UObject>>& GetContentAssetList() const ;
	//! @}
	
	//! @{@name コンテンツクラスを取得
	const TMap<FName, TSoftClassPtr<UObject>>& GetContentClassList() const ;
	//! @}
	
private:
	
	friend USNContentAssetManager;
	
	//!< コンテンツオブジェクトリスト
	UPROPERTY(EditAnywhere, Category="Data")
	TMap<FName, TSoftObjectPtr<UObject>> ContentsAssetList;
	
	//!< コンテンツクラスリスト
	UPROPERTY(EditAnywhere, Category="Class")
	TMap<FName, TSoftClassPtr<UObject>> ContentsClassList;
};
//! @}
//! @}

//----------------------------------------------------------------------//
//
//! @brief コンテンツオブジェクトを取得
//
//! @retval コンテンツオブジェクト
//
//----------------------------------------------------------------------//
FORCEINLINE const TMap<FName, TSoftObjectPtr<UObject>>& USNContentsAsset::GetContentAssetList() const {
	return ContentsAssetList;
}

//----------------------------------------------------------------------//
//
//! @brief コンテンツクラスを取得
//
//! @retval コンテンツクラス
//
//----------------------------------------------------------------------//
FORCEINLINE const TMap<FName, TSoftClassPtr<>>& USNContentsAsset::GetContentClassList() const {
	return ContentsClassList;
}


