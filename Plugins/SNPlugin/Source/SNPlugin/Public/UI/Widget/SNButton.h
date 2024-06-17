// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "SNButton.generated.h"

class USNUserWidgetBase;

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSNButtonClicked,   USNButton*, Button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSNButtonHovered,   USNButton*, Button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSNButtonUnhovered, USNButton*, Button);

//----------------------------------------------------------------------//
//
//! @brief カスタマイズボタンクラス
//
//----------------------------------------------------------------------//
UCLASS()
class SNPLUGIN_API USNButton : public UButton
{
	GENERATED_BODY()

public:
	//! @{@name デフォルトコンストラクタ
	USNButton(const FObjectInitializer& ObjectInitializer);
	//! @}
	
	//! @{@name ボタンクリック時のイベント
	FSNButtonClicked OnClickedDelegate;
	//! @}
	
	//! @{@name ボタンにホバー時のイベント
	FSNButtonHovered OnHoveredDelegate;
	//! @}
	
	//! @{@name ボタンにホバーアウト時のイベント
	FSNButtonUnhovered OnUnhoveredDelegate;
	//! @}
	
	//! @{@name ルートとなるWidgetを設定
	void SetRootWidget(USNUserWidgetBase* Root);
	//! @}
	
	//! @{@name ルートとなるWidgetを取得
	USNUserWidgetBase* GetRootWidget();
	//! @}
	
	//! @{@name 破棄される直前に呼ばれます。
	virtual void BeginDestroy() override;
	//! @}
	
protected:
	//! @{@name ボタンが押された時に呼ばれる
	UFUNCTION()
	void OnButtonClicked();	
	//! @}
	
	//! @{@name ボタンにホバー時のイベント
	UFUNCTION()
	void OnButtonHovered();
	//! @}
	
	//! @{@name ボタンにホバーアウト時のイベント
	UFUNCTION()
	void OnButtonUnhovered();
	//! @}
	
private:
	//!< ルートウィジェット
	UPROPERTY()
	USNUserWidgetBase* RootWidget = nullptr;
};

//----------------------------------------------------------------------//
//
//! @brief ルートウィジェットを設定
//
//! @param Root ルートウィジェット
//
//----------------------------------------------------------------------//
FORCEINLINE void USNButton::SetRootWidget(USNUserWidgetBase* Root){
	RootWidget = Root;
}

//----------------------------------------------------------------------//
//
//! @brief ルートウィジェットを取得
//
//! @retval ルートウィジェット
//
//----------------------------------------------------------------------//
FORCEINLINE USNUserWidgetBase* USNButton::GetRootWidget(){
	return RootWidget;
}

