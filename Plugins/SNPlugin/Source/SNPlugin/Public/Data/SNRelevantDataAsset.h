// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SNRelevantDataAsset.generated.h"

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
class UDataTable;

USTRUCT()
struct FAssetSetting{
	
	GENERATED_USTRUCT_BODY()
	
	//!< アセットのディレクトリパス
	UPROPERTY(EditAnywhere, Category = "Asset", DisplayName = "参照アセットのディレクトリ")
	FName	AssetPath;
	
	UPROPERTY(EditAnywhere, Category = "Asset", DisplayName = "サブディレクトリも検索するか")
	bool	bRecursiveSerarch = true;;
};

//----------------------------------------------------------------------//
//
//! @brief 関連するアセットを管理するクラス
//
//----------------------------------------------------------------------//
UCLASS()
class SNPLUGIN_API USNRelevantDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	//! @{@name デフォルトコンストラクタ
	USNRelevantDataAsset(const FObjectInitializer& objectInitializer);
	//! @}
	//! @{@name セットアップ処理
	void	Setup();
	//! @}
	
	//! @{@name	終了処理
	void	Finalize();
	//! @}
	
	//! @{@name データ作成用の関数
	UFUNCTION(BlueprintCallable, meta = (CallInEditor = "true"))
	void AssetRebuild();
	
	UFUNCTION(BlueprintCallable, meta = (CallInEditor = "true"))
	void AssetBuild();
	//! @}

	int	GetAssetNum() const ;

	TArray<FName> GetAssetKeys() const ;
	
	//! @{@name アセットへのポインタを取得
	UObject*	GetAsset(const FName& key);
	//! @}
	
	//! @{@name 初期化が終了しているか
	bool		IsSetupDone() const ;
	//! @}

	
	
	virtual void BeginDestroy() override ;
	
	//! @{@name アセットロード終了通知イベントを取得
	DECLARE_EVENT(USNRelevantDataAsset, FOnAssetLoadFinished);
	FOnAssetLoadFinished& GetAssetLoadFinishedEvent() { return OnAssetLoadFinishedEvent; }
	//! @}
#if WITH_EDITOR
	
	//! @{@name AssetPathの設定
	void		AddAssetPath(const FAssetSetting& AssetSetting);
	//! @}
	
	//! @{@name AssetPathの設定
	void		AppendAssetPath(const TArray<FAssetSetting>& AssetSettings);
	//! @}
	
	//! @{@name DataTableの設定
	bool		SetDataTable(UDataTable* DataTable);
	//! @}
#endif

private:
	
	//! @{@name アセットのロード処理
	bool LoadAssetAsync();
	//! @}
	
	//! @{@name ロード終了時のイベント
	void FinishedLoadAsset();
	//! @}
	
#if WITH_EDITORONLY_DATA
	
	//! @{@name アセットのビルドを実行
	void	ExecBuild(bool isRebuild, bool bShowDialog = true);
	//! @}

	UPROPERTY(EditAnywhere, Category = "Asset|Header", DisplayName = "ENUMのヘッダを出力するか")
	bool	bOutputHeader = false;
	
	UPROPERTY(EditAnywhere, Category = "Asset|Header", DisplayName = "ヘッダを出力するディレクトリ")
	FName	HeaderPath;
	
	//!< アニメーションアセットのフォルダ
	UPROPERTY(EditAnywhere, Category = "Animation", DisplayName = "参照アセット情報")
	TArray<FAssetSetting> AssetPath;
#endif
	
	//!< アニメーションのデータベース
	UPROPERTY(EditAnywhere, Category = "Animation", DisplayName = "アニメーションアセットリスト")
	TObjectPtr<UDataTable> AssetList;
	
	//!< アセットの管理マップ
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UObject>> AssetMap;

	//!< アセットロード完了イベント
	FOnAssetLoadFinished OnAssetLoadFinishedEvent;
	
	//!< ロード終了フラグ
	bool bAssetLoaded;
};

//----------------------------------------------------------------------//
//
//! @brief 初期化が終了しているかの判定
//
//! @retval true  終了している
//! @retval false 終了していない
//
//----------------------------------------------------------------------//
FORCEINLINE bool	USNRelevantDataAsset::IsSetupDone() const {
	return bAssetLoaded;
}

FORCEINLINE int USNRelevantDataAsset::GetAssetNum() const
{
	return AssetMap.Num();
}

FORCEINLINE TArray<FName> USNRelevantDataAsset::GetAssetKeys() const 
{
	TArray<FName> Keys;

	AssetMap.GetKeys(Keys);
	
	return Keys;
}

