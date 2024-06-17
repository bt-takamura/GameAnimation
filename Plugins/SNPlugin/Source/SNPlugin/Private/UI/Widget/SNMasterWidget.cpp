// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SNMasterWidget.h"

#include "SNDef.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//----------------------------------------------------------------------//
USNMasterWidget::USNMasterWidget()
:Super()
{
	// レイヤーパネルを初期化
	for(int i=0 ; i<(int)EWidgetLayer::Num ; ++i){
		LayerPanel[i] =nullptr;
	}
}

//----------------------------------------------------------------------//
//
//! @brief 初期化処理
//
//----------------------------------------------------------------------//
void USNMasterWidget::Init(){
	
	for(int i=0 ; i<(int)EWidgetLayer::Num ; ++i){
		// レイヤー名を生成
		FName Name(*FString::Printf(TEXT("Layer%d"), i));
		// レイヤーを登録
		LayerPanel[i] = Cast<UCanvasPanel>(GetWidgetFromName(Name));
	}
}

//----------------------------------------------------------------------//
//
//! @brief 終了処理
//
//----------------------------------------------------------------------//
void USNMasterWidget::NativeDestruct(){
	
	Super::NativeDestruct();
	// 登録されているオブジェクトを一旦破棄
	ClearAll();
	// レイヤーパネルを初期化
	for(int i=0 ; i<(int)EWidgetLayer::Num ; ++i){
		LayerPanel[i] =nullptr;
	}
}

//----------------------------------------------------------------------//
//
//! @brief レイヤーを設定
//
//! @param Layer  レイヤー番号
//! @param Widget 登録するウィジェット
//
//----------------------------------------------------------------------//
void USNMasterWidget::SetLayer(EWidgetLayer Layer, USNUserWidgetBase* Widget){
	// レンジチェック
	SNPLUGIN_ASSERT(Layer < EWidgetLayer::Num, TEXT("Widget Layer is Invalid."));
	// ウィジェットがnullの場合は警告
	if(Widget == nullptr){
		
		SNPLUGIN_WARNING(TEXT("SetLayer : Widget is nullptr."));
		
		return;
	}

	if(Widget->GetRegistLayer() != EWidgetLayer::Invalid)
	{
		SNPLUGIN_WARNING(TEXT("Widget is already registed."));

		return;
	}
	// パネルがない場合も警告
	if(LayerPanel[(int)Layer] == nullptr){
		
		SNPLUGIN_WARNING(TEXT("SetLayer : Layer Panel is nullptr -> %d"), (int)Layer);
		
		return;
	}

	if(LayerPanel[(int)Layer]->HasChild(Widget) == true)
	{
		SNPLUGIN_ERROR(TEXT("SetLayer : Widget is already setted. -> %s"), *Widget->GetName());

		return;
	}
	// キャンバスを取得
	UCanvasPanelSlot* CanvasPanelSlot(Cast<UCanvasPanelSlot>(LayerPanel[(int)Layer]->AddChildToCanvas(Widget)));
	// nullチェック
	if(CanvasPanelSlot != nullptr){
		// アンカーを設定
		CanvasPanelSlot->SetAnchors(FAnchors(0, 0, 1, 1));
		// オフセットを設定
		CanvasPanelSlot->SetOffsets(FMargin(0, 0, 0, 0));
		// 登録したレイヤー情報を設定
		Widget->SetRegistLayer(Layer);
	}
}

//----------------------------------------------------------------------//
//
//! @brief レイヤーから削除
//
//! @param Layer  レイヤー番号
//! @param Widget 削除するウィジェット
//
//----------------------------------------------------------------------//
void USNMasterWidget::RemoveLayer(EWidgetLayer Layer, USNUserWidgetBase* Widget){
	// レンジチェック
	SNPLUGIN_ASSERT(Layer < EWidgetLayer::Num, TEXT("Widget Layer is Invalid."));
	// ウィジェットがnullの場合は警告
	if(Widget == nullptr){
		
		SNPLUGIN_WARNING(TEXT("RemoveLayer : Widget is nullptr."));
		
		return;
	}
	// パネルがない場合も警告
	if(LayerPanel[(int)Layer] == nullptr){
		
		SNPLUGIN_WARNING(TEXT("RemoveLayer : Layer Panel is nullptr -> %d"), (int)Layer);
		
		return;
	}
	// レイヤーから削除
	LayerPanel[(int)Layer]->RemoveChild(Widget);
	// 登録したレイヤー情報を無効化
	Widget->SetRegistLayer(EWidgetLayer::Invalid);
}

//----------------------------------------------------------------------//
//
//! @brief レイヤーに登録されているウィジェットをクリア
//
//! @param Layer レイヤー番号
//
//----------------------------------------------------------------------//
void USNMasterWidget::CleanLayer(EWidgetLayer Layer){
	// レンジチェック
	SNPLUGIN_ASSERT(Layer < EWidgetLayer::Num, TEXT("Widget Layer is Invalid."));
	// パネルがない場合は警告
	if(LayerPanel[(int)Layer] == nullptr){
		
		SNPLUGIN_WARNING(TEXT("SetLayer : Layer Panel is nullptr -> %d"), (int)Layer);
		
		return;
	}
	// 登録されているウィジェットを削除
	LayerPanel[(int)Layer]->ClearChildren();
}

//----------------------------------------------------------------------//
//
//! @brief 全てのレイヤーに登録されているウィジェットをクリア
//
//----------------------------------------------------------------------//
void USNMasterWidget::ClearAll(){
	
	for(int i=0 ; i<(int)EWidgetLayer::Num ; ++i){
		CleanLayer((EWidgetLayer)i);
	}
}
