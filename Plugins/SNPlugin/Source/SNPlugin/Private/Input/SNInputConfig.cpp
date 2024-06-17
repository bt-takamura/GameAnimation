// Fill out your copyright notice in the Description page of Project Settings.
#include "Input/SNInputConfig.h"
#include "SNDef.h"
#include "Utility/SNUtility.h"
#include "Input/SNInputManagerSubsystem.h"
#include "Action/SNActionBase.h"

#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Character/Base/SNPlayablePawnInterface.h"


//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//! @param ObjectInitializer イニシャライザ
//
//----------------------------------------------------------------------//
USNInputConfig::USNInputConfig(const FObjectInitializer& ObjectInitializer):
 Super(ObjectInitializer)
{
	InputActionList.Empty();
}

//----------------------------------------------------------------------//
//
//! @brief 入力周りの初期化処理
//
//! @param Name        コンフィグ名
//! @param OwnerObject オーナーとなるオブジェクト
//
//! @retval true  正常終了
//! @retval false 異常終了
//
//----------------------------------------------------------------------//
bool	USNInputConfig::InitializeInput(FName Name, UObject* OwnerObject){

	SNPLUGIN_ASSERT(OwnerObject != nullptr, TEXT("Should set Input Action Owner."));

	if(OwnerActor != nullptr)
	{
		ISNPlayablePawnInterface* PlayablePawn = Cast<ISNPlayablePawnInterface>(OwnerActor);

		if(PlayablePawn != nullptr)
		{
			SNPLUGIN_LOG(TEXT("Owner is not nullptr.[%d]"), PlayablePawn->GetActionNum());	
		} else
		{
			SNPLUGIN_LOG(TEXT("PlayablePawn is not Owner."));
		}
		
	}
	// オーナーを設定
	OwnerActor = OwnerObject;
	// コンフィグ名を設定
	ConfigName = Name;

	APawn* OwnerPawn(Cast<APawn>(OwnerObject));
	// ローカルプレイヤー以外はContextMappingへの登録は行わない
	if((OwnerPawn != nullptr) && (OwnerPawn->Controller != nullptr) && (OwnerPawn->Controller->IsLocalController() == true))
	{
		// ゲームインスタンスを取得
		UGameInstance* GameInstance(SNUtility::GetGameInstance<UGameInstance>());
		// ゲームインスタンスが取得できない場合はアサート
		SNPLUGIN_ASSERT(GameInstance != nullptr, TEXT("GameInstance is nullptr"));
		// 入力マネージャーサブシステムを取得
		USNInputManagerSubsystem* InputManagerSubsystem(GameInstance->GetSubsystem<USNInputManagerSubsystem>());
		// 取得できない場合はアサート
		SNPLUGIN_ASSERT(InputManagerSubsystem != nullptr, TEXT("InputManagerSubsystem is nullptr."));
		// インプットコンテキストを同期ロード
		// (@@Note : ヒッチが発生する場合は非同期にするか検討)
		UInputMappingContext* InputMappingContextInstance(InputMappingContext.LoadSynchronous());
		// ロードに失敗した場合はアサート
		SNPLUGIN_ASSERT(InputMappingContextInstance != nullptr, TEXT("InputMappingContext is nullptr."));
		// コンテキストを追加
		InputManagerSubsystem->AddInputContext(ConfigName, InputMappingContextInstance);
	}
	// アクションは非同期でロード
	TArray<FSoftObjectPath> assetList;
	
	for(auto& action:InputActionList){
		
		assetList.Add(action.ActionClass.ToSoftObjectPath());
	}
	// 非同期ロードをリクエスト
	StreamableHandle = SNUtility::RequestAsyncLoad(assetList, this, &USNInputConfig::FinishLoadAsset);

	SNPLUGIN_LOG(TEXT("Config Initialize is done."));
	
	return true;
}

//----------------------------------------------------------------------//
//
//! @brief アクションアセットの非同期ロードが終了した際の処理
//
//----------------------------------------------------------------------//
void	USNInputConfig::FinishLoadAsset(){
	
	UEnhancedInputComponent* InputComponent = nullptr;
	
	APawn* Pawn = Cast<APawn>(OwnerActor);
	
	if(Pawn != nullptr){
		// 入力コンポーネントを取得
		InputComponent = Cast<UEnhancedInputComponent>(Pawn->InputComponent);
	} else {
		// ポーンではない場合はプレイヤーコントローラから入力コンポーネントを取得
		APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
		// プレイヤーコントローラがない場合はアサート
		SNPLUGIN_ASSERT(PlayerController != nullptr, TEXT("PlayerController is nullptr."));
		// 入力コンポーネントを取得
		InputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
	}
	
	TArray<UObject*> ObjectList;
	// ロードしたアセットを取得
	StreamableHandle->GetLoadedAssets(ObjectList);
	// プレイヤブルポーンインターフェイスを取得
	ISNPlayablePawnInterface* PlayablePawn = Cast<ISNPlayablePawnInterface>(Pawn);
	
	int Count = 0;
	
	for(auto& Object: ObjectList){
		// クラス情報へキャスト
		UClass* ClassPtr = Cast<UClass>(Object);
		
		if(ClassPtr != nullptr){
			
			USNActionBase* Action = NewObject<USNActionBase>(OwnerActor, ClassPtr);
			
			if(Action != nullptr){
				// ※@@Note : ロードの順番が登録した順番として保証されてない場合は見直しが必要
				FSNInputAction& Input(InputActionList[Count]);
				// アクション名を設定
				Action->SetActionName(*(Input.InputAction->GetName()));
				
				if(PlayablePawn != nullptr){
					// ポーンにアクションを追加
					// ※オンライン非対応であればここの処理は不要
					PlayablePawn->AddInputAction(Action->GetActionName(), Action);
				}
				// 入力コンポーネントにバインド
				Action->Initialize(InputComponent, Input.InputAction.Get(), OwnerActor);
				// リストに登録
				ActionList.Add(Action);
			}
		}
		
		++Count;
	}
	
	SNPLUGIN_LOG(TEXT("Config Post Load Async is done."));
}

//----------------------------------------------------------------------//
//
//! @brief 入力コンフィグを有効、無効化に設定
//
//! @param bEnabled true : 有効化 / false : 無効化
//
//----------------------------------------------------------------------//
void USNInputConfig::SetEnabled(bool bEnabled){
	// ゲームインスタンスを取得
	UGameInstance* GameInstance(SNUtility::GetGameInstance<UGameInstance>());
	// ゲームインスタンスが取得できない場合はアサート
	SNPLUGIN_ASSERT(GameInstance != nullptr, TEXT("GameInstance is nullptr"));
	// 入力マネージャーサブシステムを取得
	USNInputManagerSubsystem* InputManagerSubsystem(GameInstance->GetSubsystem<USNInputManagerSubsystem>());
	
	if(InputManagerSubsystem != nullptr){
		
		if(bEnabled == true){
			// マッピングコンテキストを有効化
			InputManagerSubsystem->EnableInputMapping(GetConfigName());
		} else {
			// マッピングコンテキストを無効化
			InputManagerSubsystem->DisableInputMapping(GetConfigName());
		}
	}
}

//----------------------------------------------------------------------//
//
//! @brief 解放処理
//
//----------------------------------------------------------------------//
void USNInputConfig::Release(){
	// ゲームインスタンスを取得
	UGameInstance* GameInstance(SNUtility::GetGameInstance<UGameInstance>());
	// ゲームインスタンスが取得できない場合はアサート
	SNPLUGIN_ASSERT(GameInstance != nullptr, TEXT("GameInstance is nullptr"));
	// 入力マネージャーサブシステムを取得
	USNInputManagerSubsystem* InputManagerSubsystem(GameInstance->GetSubsystem<USNInputManagerSubsystem>());
	
	if(InputManagerSubsystem != nullptr){
		// マネージャーから解放
		InputManagerSubsystem->RemoveInputContext(GetConfigName());
	}
	// オーナーを初期化
	OwnerActor = nullptr;
	// アクションをリセット
	ActionList.Empty();
}
