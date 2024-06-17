// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SNUIDef.h"
#include "UI/Widget/SNUserWidgetBase.h"
#include "SNMasterWidget.generated.h"

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
class UCanvasPanel;

//----------------------------------------------------------------------//
//
//! @brief すべてのWidgetの親になるWidgetクラス
//
//! @note このクラスから派生したWidgetを作成し、Widget側で「Layer0からLayer5」の
//        名前でCanvasPanelを作る必要があります。
//
//----------------------------------------------------------------------//
UCLASS()
class SNPLUGIN_API USNMasterWidget : public USNUserWidgetBase
{
	GENERATED_BODY()

public:
	
	//! @{@name デフォルトコンストラクタ
	USNMasterWidget();
	//! @}
	
	//! @{@name 初期化処理
	virtual void Init();
	//! @}
	
	//! @{@name 終了処理
	virtual void NativeDestruct() override;
	//! @}
	
	//! @{@name レイヤーを設定
	void SetLayer(EWidgetLayer Layer, USNUserWidgetBase* Widget);
	//! @}
	
	//! @{@name レイヤーから削除
	void RemoveLayer(EWidgetLayer Layer, USNUserWidgetBase* Widget);
	//! @}
	
	//! @{@name レイヤーに登録されているウィジェットをクリア
	void CleanLayer(EWidgetLayer Layer);
	//! @}
	
	//! @{@name 全てのレイヤーに登録されているウィジェットをクリア
	void ClearAll();
	//! @}
	
private:
	//!< パネル
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanel> LayerPanel[(int)EWidgetLayer::Num];
};
