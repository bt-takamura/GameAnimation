// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SNPlayerController.h"

#include "SNDef.h"
#include "Utility/SNUtility.h"
#include "Input/SNInputConfig.h"
#include "Input/SNInputManagerSubsystem.h"

//----------------------------------------------------------------------//
//
//! @brief プレイが終了した際のイベント
//
//! @param EndPlayReason 終了する原因
//
//----------------------------------------------------------------------//
void ASNPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason){
	
	Super::EndPlay(EndPlayReason);
	// 入力管理用マネージャーを取得
	USNInputManagerSubsystem* InputManagerSubsystem(SNUtility::GetSNInputManagerSubsystem());
	
	if(bInitializedInput == true){
		
		for(auto& InputConfig:InputConfigMap){
			// 入力マッピングを無効化
			InputManagerSubsystem->DisableInputMapping(InputConfig.Key);
			// リソースを解放
			InputConfig.Value->Release();
		}
		
		SNPLUGIN_LOG(TEXT("PlayerController Endplay is done."))
	}
}

bool ASNPlayerController::InitializeInput(){
	// オーナー用のポーンを取得
	APawn* OwnerPawn = GetPawn();
	// 
	SNPLUGIN_ASSERT(OwnerPawn != nullptr, TEXT("Pawn is nullptr. This function should be called from Pawn's SetupPlayerInputComponent."));
	// 初期化処理
	for(auto& InputConfig:InputConfigMap){
		InputConfig.Value->InitializeInput(InputConfig.Key, OwnerPawn);
	}
	
	SNPLUGIN_LOG(TEXT("Input Config Initialize."));
	
	bInitializedInput = true;
	
	if(CurrentInputTypes.Num() > 0){
		// カレントに設定されているタイプを設定
		for(auto& Name:CurrentInputTypes){
			SetEnableInput(Name, false);
		}
	}

	return true;
}

void ASNPlayerController::EnabledInputType(FName Name, bool bClearAll){
	
	if(bClearAll){
		// カレントタイプをクリア
		CurrentInputTypes.Empty();
	}
	
	CurrentInputTypes.Add(Name);
	
	if(bInitializedInput == true){
		
		SetEnableInput(Name, bClearAll);
	}
}

void ASNPlayerController::DisableInputType(FName Name){
	
	CurrentInputTypes.Remove(Name);
	
	if(bInitializedInput == true){
		
		SetDisableInput(Name);
	}
}

void ASNPlayerController::SetEnableInput(FName Name, bool bClearAll)
{
	USNInputManagerSubsystem* InputManagerSubsystem(SNUtility::GetSNInputManagerSubsystem());
	// 指定されたマッピングを有効化
	InputManagerSubsystem->EnableInputMapping(Name, bClearAll);
		
	SNPLUGIN_LOG(TEXT("Current Input Type : %s"), *Name.ToString());
}

void ASNPlayerController::SetDisableInput(FName Name)
{
	USNInputManagerSubsystem* InputManagerSubsystem(SNUtility::GetSNInputManagerSubsystem());
		
	InputManagerSubsystem->DisableInputMapping(Name);
		
	SNPLUGIN_LOG(TEXT("Disable Input Type : %s"), *Name.ToString());
}


