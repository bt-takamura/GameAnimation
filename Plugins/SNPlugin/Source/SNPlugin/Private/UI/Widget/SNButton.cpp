// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/SNButton.h"
#include "SNDef.h"

//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//! @param ObjectInitializer イニシャライザ
//
//----------------------------------------------------------------------//
USNButton::USNButton(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	OnClicked.AddDynamic(this, &USNButton::OnButtonClicked);
	OnHovered.AddDynamic(this, &USNButton::OnButtonHovered);
	OnUnhovered.AddDynamic(this, &USNButton::OnButtonUnhovered);
}

//----------------------------------------------------------------------//
//
//! @brief 破棄前の処理
//
//----------------------------------------------------------------------//
void USNButton::BeginDestroy(){
	
	Super::BeginDestroy();
	// デリゲートをクリア
	OnClickedDelegate.Clear();
	OnHoveredDelegate.Clear();
	OnUnhoveredDelegate.Clear();
	// 登録している関数を削除
	OnClicked.RemoveDynamic(this, &USNButton::OnButtonClicked);
	OnHovered.RemoveDynamic(this, &USNButton::OnButtonHovered);
	OnUnhovered.RemoveDynamic(this, &USNButton::OnButtonUnhovered);
}

//----------------------------------------------------------------------//
//
//! @brief ボタンをクリックされた際の処理
//
//----------------------------------------------------------------------//
void USNButton::OnButtonClicked(){
	
	SNPLUGIN_LOG(TEXT("OnButtonClicked %s"), *GetName());
	// デリゲートを実行
	if(OnClickedDelegate.IsBound()){
		OnClickedDelegate.Broadcast(this);
	}
}

//----------------------------------------------------------------------//
//
//! @brief マウスカーソルがボタンの上に来た際の処理
//
//----------------------------------------------------------------------//
void USNButton::OnButtonHovered(){
	
	SNPLUGIN_LOG(TEXT("OnButtonHovered %s"), *GetName());
	// デリゲートを実行
	if(OnHoveredDelegate.IsBound()){
		OnHoveredDelegate.Broadcast(this);
	}
}

//----------------------------------------------------------------------//
//
//! @brief マウスカーソルがボタンの上から離れたときの処理
//
//----------------------------------------------------------------------//
void USNButton::OnButtonUnhovered(){
	
	SNPLUGIN_LOG(TEXT("OnButtonUnhovered %s"), *GetName());
	// デリゲートを実行
	if(OnUnhoveredDelegate.IsBound()){
		OnUnhoveredDelegate.Broadcast(this);
	}
}
