// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/SNGameInstance.h"
#include "SNContentAssetManager.generated.h"

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
class USNContentsAsset;

//!@{@defgroup コンテンツアセット
//!@{
//----------------------------------------------------------------------//
//
//! @brief コンテンツアセット管理クラス
//!
//! DLCなどを考慮するべきアセットを管理するクラス。
//! BP化しなくてもSNGameInstanceを使用していればデフォルトで生成されます。
//! (リリース時にアセットを含めたい場合はBP化してSNGameInstance側に設定してください。)
//! 
//! GameInstanceの初期化時にSetupDLCContents()がコールされ、ダウンロード済みの
//! ISNDLCModuleInterfaceから派生しているモジュールを検索し、USNContentAssetManager型の
//! アセットがDLCに含まれる場合はUSNContentAssetManagerに含まれるアセットをDataAssetListに追加します。
//! 
//! DLC側のUSNContentAssetManagerにアセットを登録する際はMapのキーにルールを
//! 持たせると汎用性が上がります。
//! 
//! また、複数のDLCで同じキーを持つものが見つかった場合はリストに追加しますが、
//! USNContentsAssetのキーが重複している場合は上書きします。
//! (FindModuleの検索順番に依存するので注意が必要です。)
//
//----------------------------------------------------------------------//
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class SNPLUGIN_API USNContentAssetManager : public UObject
{
	GENERATED_BODY()

public:
	
	//! @{@name アセットリストを取得
	const TMap<FName, TObjectPtr<USNContentsAsset>>& GetAssetList() const ;
	//! @}
	
	//! @{@name コンテンツのアセットを取得
	const USNContentsAsset* GetContentAsset(FName Key) const ;
	//! @}
	
private:
	
	friend USNGameInstance;
	
	//! @{@name DLCコンテンツの初期化処理
	bool SetupDLCContents();
	//! @}
	
	//!< アセットリスト
	UPROPERTY(EditAnyWhere, Category="Data")
	TMap<FName, TObjectPtr<USNContentsAsset>> DataAssetList;
};
//! @}
//! @}

//----------------------------------------------------------------------//
//
//! @brief アセットリストを取得
//
//! @retval アセットリスト
//
//----------------------------------------------------------------------//
FORCEINLINE const TMap<FName, TObjectPtr<USNContentsAsset>>& USNContentAssetManager::GetAssetList() const {
	return DataAssetList;
}

