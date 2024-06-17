// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Base/SNWheeledVehiclePlayerBase.h"

#include "SNDef.h"
#include "Action/SNActionBase.h"
#include "Character/SNPlayerController.h"

//----------------------------------------------------------------------//
//
//! @brief プレイヤーのリスタート処理
//
//! @note 接続後、InputComponentなどの初期化も終わったタイミングで呼ばれます。
//        ホストではないプレイヤーにInputComponentは存在しないので、注意。
//
//----------------------------------------------------------------------//
void ASNWheeledVehiclePlayerBase::NotifyRestarted(){
	
	Super::NotifyRestarted();
	
	ASNPlayerController* PlayerController(Cast<ASNPlayerController>(Controller));
	
	if(PlayerController != nullptr){
		// 入力の初期化処理
		PlayerController->InitializeInput();
		
		SNPLUGIN_LOG(TEXT("NotifyRestarted."));
	}
}

//----------------------------------------------------------------------//
//
//! @brief 終了処理
//
//! @param EndPlayReason 終了の理由
//
//----------------------------------------------------------------------//
void ASNWheeledVehiclePlayerBase::EndPlay(const EEndPlayReason::Type EndPlayReason){
	
	Super::EndPlay(EndPlayReason);
	
	for(auto& Action:InputActionMap){
		// アクションを破棄
		Action.Value->ConditionalBeginDestroy();
	}
	// アクションリストをリセット
	InputActionMap.Reset();
}


//----------------------------------------------------------------------//
//
//! @brief 入力された際に呼ばれるアクションを追加
//
//! @param Name   アクション名
//! @param Action アクションへのポインタ
//
//----------------------------------------------------------------------//
void ASNWheeledVehiclePlayerBase::AddInputAction(const FName& Name, USNActionBase* Action){
	InputActionMap.Add(Name, Action);
}

//----------------------------------------------------------------------//
//
//! @brief アクションを取得
//
//! @param Name アクション名
//
//! @retval アクションへのポインタ
//
//----------------------------------------------------------------------//
USNActionBase* ASNWheeledVehiclePlayerBase::GetAction(const FName& Name){
	
	USNActionBase* Action = nullptr;
	
	if(InputActionMap.Contains(Name) == true){
		Action = InputActionMap[Name];
	}
	
	return Action;
}

int ASNWheeledVehiclePlayerBase::GetActionNum() const
{
	return InputActionMap.Num();
}


//----------------------------------------------------------------------//
//
//! @brief サーバー側でアクションを実行
//
//! @param Name             アクション名
//! @param InputActionValue 入力値
//
//----------------------------------------------------------------------//
void ASNWheeledVehiclePlayerBase::ExecuteActionOnServer(const FName& Name, const FInputActionValue& InputActionValue){

	FInputActionValueProxy Proxy(InputActionValue);
	
	ExecuteInputAction_OnServer(Name, Proxy.GetValue(), Proxy.GetValueType());
}

//----------------------------------------------------------------------//
//
//! @brief 入力からアクション実行用のレプリケーション
//
//! @param Name       アクション名
//! @param InputValue 入力値
//! @param Type       入力タイプ
//
//----------------------------------------------------------------------//
void ASNWheeledVehiclePlayerBase::ExecuteInputAction_OnServer_Implementation(FName Name, FVector InputValue, EInputActionValueType Type){

	FInputActionValue InputActionValue(Type, InputValue);
	
	ISNPlayablePawnInterface::ExecuteAction(Name, InputActionValue);
	
	SNPLUGIN_LOG(TEXT("[%s]Server Input Action is calling. %d : %f : %f"), *(Name.ToString()), InputActionMap.Num(), InputActionValue[0], InputActionValue[1]);
}
