// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SNSceneBase.generated.h"

class USNMasterWidget;

//----------------------------------------------------------------------//
//
//! @brief シーン管理用のベースクラス
//
//----------------------------------------------------------------------//
UCLASS()
class SNPLUGIN_API ASNSceneBase : public AActor
{
	GENERATED_BODY()
	
public:
	//! @{@name デフォルトコンストラクタ
	ASNSceneBase();
	//! @}
	
	//! @{@name フレームごとに呼び出される
	virtual void Tick(float DeltaTime) override;
	//! @}
	
	//! @{@name マスターウィジェットを取得
	USNMasterWidget* GetMasterWidget();
	//! @}
	
protected:
	
	//! @{@name ゲーム開始時またはスポーン時にコールされる
	virtual void BeginPlay() override;
	//! @}
	
	//! @{@name プレイ終了時に呼ばれる
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//! @}
	
private:
	//!< マスターウィジェットクラス情報
	UPROPERTY(EditAnywhere, Category="UserInterface")
	TSoftClassPtr<USNMasterWidget> MasterWidgetClass = nullptr;
	
	//!< マスターウィジェットのインスタンス
	UPROPERTY()
	TObjectPtr<USNMasterWidget> MasterWidget = nullptr;
};

//----------------------------------------------------------------------//
//
//! @brief マスターウィジェットを取得
//
//! @retval マスターウィジェット
//
//----------------------------------------------------------------------//
FORCEINLINE USNMasterWidget* ASNSceneBase::GetMasterWidget(){
	return MasterWidget;
}
