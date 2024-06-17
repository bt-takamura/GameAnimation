// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTree/SNSceneStateTreeTaskBase.h"

#include "SNDef.h"
#include "Scene/SNSceneBase.h"
#include "System/SNGameInstance.h"
#include "UI/Widget/SNMasterWidget.h"
#include "Utility/SNUtility.h"

//----------------------------------------------------------------------//
//
//! @brief ステート開始に呼ばれます
//
//! @param Context    コンテキスト情報
//! @param Transition トランジション情報
//
//! @retval 実行ステータス
//
//----------------------------------------------------------------------//
EStateTreeRunStatus USNSceneStateTreeTaskBase::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition){
	// リザルトを取得
	EStateTreeRunStatus Result = Super::EnterState(Context, Transition);
	// ウィジェットのクラス情報を非同期ロード
	LoadHudClassASync();
	
	return Result;
}

//----------------------------------------------------------------------//
//
//! @brief ステート終了時に呼ばれます。
//
//! @param Context    コンテキスト情報
//! @param Transition トランジション情報
//
//----------------------------------------------------------------------//
void USNSceneStateTreeTaskBase::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition){
	
	Super::EnterState(Context, Transition);
	
	USNMasterWidget* MasterWidget(nullptr);
	// シーン情報を取得
	ASNSceneBase* Scene(Cast<ASNSceneBase>(GetOwnerActor(Context)));
	
	if(Scene != nullptr){
		// マスターウィジェットへのポインタを取得
		MasterWidget= Scene->GetMasterWidget();
	}
	
	for(USNUserWidgetBase* Widget:HudInsntaceList){
		
		if(Widget != nullptr){
			// ウィジェットがマスターウィジェットに登録されている場合は削除
			if((MasterWidget != nullptr) && (Widget->GetRegistLayer() != EWidgetLayer::Invalid)){
				MasterWidget->RemoveLayer(Widget->GetRegistLayer(), Widget);
			}
			// ウィジェットを破棄
			Widget->ConditionalBeginDestroy();
		}
	}
	// リストを空に設定
	HudInsntaceList.Empty();
}

//----------------------------------------------------------------------//
//
//! @brief ウィジェットクラスの非同期ロード処理
//
//----------------------------------------------------------------------//
void USNSceneStateTreeTaskBase::LoadHudClassASync(){
	
	if(HudClassList.Num() > 0){
		
		TArray<FSoftObjectPath> Array;
		// 非同期するリストを生成
		for(auto& Class:HudClassList){
			Array.Add(Class.ToSoftObjectPath());
		}
		// 非同期を実行
		HudStreamHundle = SNUtility::RequestAsyncLoad(Array, this, &USNSceneStateTreeTaskBase::FinishLoadHudClass);
	}
}

//----------------------------------------------------------------------//
//
//! @brief 非同期ロードが終了した際のデリゲート
//
//----------------------------------------------------------------------//
void USNSceneStateTreeTaskBase::FinishLoadHudClass(){
	// ロードが終了していない場合は終了
	if(HudStreamHundle->HasLoadCompleted() == false){
		return;
	}
	
	TArray<UObject*> ObjectList;
	// ロードしたアセットを取得
	HudStreamHundle->GetLoadedAssets(ObjectList);
	// アセットが存在しない場合は終了
	if(ObjectList.Num() <= 0){
		
		SNPLUGIN_ERROR(TEXT("Hud Classes is nullptr."));
		
		return;
	}
	// プレイヤーコントローラを取得
	APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
	// 取得に失敗した場合はエラー
	if(PlayerController == nullptr){
		
		SNPLUGIN_ERROR(TEXT("PlayerController is nullptr."));
		
		return;
	}
	
	for(UObject* Object:ObjectList){
		// クラス情報に変換
		UClass* Class(Cast<UClass>(Object));
		// 変換に失敗した場合はスキップ
		if(Class == nullptr){
			continue;
		}
		// ウィジェットを生成
		USNUserWidgetBase* Widget = Cast<USNUserWidgetBase>(CreateWidget(PlayerController, Class));
		
		if(Widget != nullptr){
			// 表示をOFF
			Widget->SetVisibility(ESlateVisibility::Hidden);
			// インスタンスリストに追加
			HudInsntaceList.Add(Widget);
		}
	}
	// ロード終了を派生クラスに通知
	HudPostLoad();
}

//----------------------------------------------------------------------//
//
//! @brief ロード終了後に呼ばれます。
//
//----------------------------------------------------------------------//
void USNSceneStateTreeTaskBase::HudPostLoad(){
	
}

ASNSceneBase* USNSceneStateTreeTaskBase::GetCurrentScene() const
{
	USNGameInstance* GameInstance(SNUtility::GetGameInstance<USNGameInstance>());

	if(GameInstance == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Failed to get current scene."));

		return nullptr;
	}
	
	ASNSceneBase* Scene(GameInstance->GetCurrentScene());

	return Scene;
}

bool USNSceneStateTreeTaskBase::SetWidgetLayer(EWidgetLayer Layer, USNUserWidgetBase* Widget)
{
	ASNSceneBase* Scene(GetCurrentScene());

	if(Scene == nullptr)
	{
		return false;
	}

	USNMasterWidget* MasterWidget(Scene->GetMasterWidget());

	if(MasterWidget == nullptr)
	{
		return false;
	}

	MasterWidget->SetLayer(Layer, Widget);

	return true;
}

bool USNSceneStateTreeTaskBase::SetVisibleWidget(EWidgetLayer Layer, USNUserWidgetBase* Widget)
{
	bool Result = SetWidgetLayer(Layer, Widget);

	if(Result == false)
	{
		return false;
	}

	Widget->SetVisibility(ESlateVisibility::Visible);

	return true;
}




