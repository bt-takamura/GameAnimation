// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAItemDef.h"
#include "GameFramework/Actor.h"
#include "GAItemBoxBase.generated.h"

class UArrowComponent;
class USoundCue;


//----------------------------------------------------------------------//
//
//! @brief 宝箱のベースクラス
//
//----------------------------------------------------------------------//
UCLASS()
class GAMEANIMATIONSAMPLE_API AGAItemBoxBase : public AActor
{
	GENERATED_BODY()
	
public:
	
	//! @{@name デフォルトコンストラクタ
	AGAItemBoxBase();
	//! @}
	
	//! @{@name Called every frame
	virtual void Tick(float DeltaTime) override;
	//! @}
	
	//! @{@name 宝箱を開くリクエスト
	UFUNCTION(BlueprintCallable)
	void OpenRequest();
	//! @}
	
	//! @{@name 宝箱を閉めるリクエスト
	UFUNCTION(BlueprintCallable)
	void CloseRequest();
	//! @}
	
	//! @{@name アイテムタイプの設定
	void SetItemType(EItemType Type);
	//! @}
	
	//! @{@name アイテムタイプの取得
	EItemType GetItemType() const ;
	//! @}
	
protected:
	
	//! @{@name Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//! @}
	
private:
	
	//! @{@name 宝箱を開く
	UFUNCTION()
	void Open();
	//! @}
	
	//! @{@name 宝箱を閉じる
	UFUNCTION()
	void Close();
	//! @}
	
	//!< タイマーハンドラー
	UPROPERTY()
	FTimerHandle TimerHandle;
	
	//!< 1度でも開いたかどうかのフラグ
	UPROPERTY()
	bool IsOpened = false;
	
	//!< 現在開いている最中かどうかのフラグ
	UPROPERTY()
	bool IsOpening = false;
	
	//!< 中に入っているアイテムのタイプ
	UPROPERTY()
	TEnumAsByte<EItemType> ItemType = EItemType_None;
	
	//!< オープンしたときに得るスコア
	UPROPERTY()
	int Score = -1;
	
	//!< 開閉スピード
	UPROPERTY(EditAnywhere, Category="ItemBox")
	float Speed = 3.0f;
	
	//!< ItemID(データテーブルのキー)
	UPROPERTY(EditAnywhere, Category="ItemBox")
	FName ItemId = NAME_None;
	
	//!< 開いたときの音
	UPROPERTY(EditAnywhere, Category="ItemBox")
	TObjectPtr<USoundCue> OpenSound = nullptr;
	
	//!< 閉じたときの音
	UPROPERTY(EditAnywhere, Category="ItemBox")
	TObjectPtr<USoundCue> CloseSound = nullptr;
	
	//!< 開くためのアローコンポーネント
	UPROPERTY(EditAnywhere, Category="ItemBox")
	TObjectPtr<UArrowComponent> LidPivotComponent = nullptr;
	
	//!< 宝箱の土台のメッシュ
	UPROPERTY(EditAnywhere, Category="ItemBox")
	TObjectPtr<UStaticMeshComponent> ItemBoxBaseComponent = nullptr;
	
	//!< 宝箱の蓋のメッシュ
	UPROPERTY(EditAnywhere, Category="ItemBox")
	TObjectPtr<UStaticMeshComponent> ItemBoxLidComponent = nullptr;
};

//----------------------------------------------------------------------//
//
//! @brief アイテムタイプの設定
//
//! @param Type アイテムタイプ
//
//----------------------------------------------------------------------//
FORCEINLINE void AGAItemBoxBase::SetItemType(EItemType Type){
	ItemType = Type;
}

//----------------------------------------------------------------------//
//
//! @brief アイテムタイプの取得
//
//! @retval アイテムタイプ
//
//----------------------------------------------------------------------//
FORCEINLINE EItemType AGAItemBoxBase::GetItemType() const {
	return ItemType;
}
