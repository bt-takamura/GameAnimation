// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Base/SNPlayablePawnInterface.h"

#include "SNDef.h"
#include "Action/SNActionBase.h"

// Add default functionality here for any ISNPlayablePawnInterface functions that are not pure virtual.

//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//----------------------------------------------------------------------//
ISNPlayablePawnInterface::ISNPlayablePawnInterface()
{
	
}

//----------------------------------------------------------------------//
//
//! @brief アクションを実行
//
//! @param Name             アクション名
//! @param InputActionValue 入力値
//
//----------------------------------------------------------------------//
void ISNPlayablePawnInterface::ExecuteAction(const FName& Name, const FInputActionValue& InputActionValue){
	
	USNActionBase* Action(GetAction(Name));
	
	if(Action == nullptr){
		
		SNPLUGIN_LOG(TEXT("Action is nullptr."));
		
		return;
	}
	// 入力値を出力
	SNPLUGIN_LOG(TEXT("Input Value : %f : %f"), InputActionValue[0], InputActionValue[1]);
	// アクションを実行
	Action->ExecAction(InputActionValue);
}
