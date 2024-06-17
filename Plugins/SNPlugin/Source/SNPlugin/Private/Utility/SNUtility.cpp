// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/SNUtility.h"
#include "SNDef.h"
#include "Input/SNInputManagerSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Online/SNOnlineSystem.h"
#include "System/SNGameInstance.h"

UWorld* SNUtility::GetWorld(){
	return GetPrimaryWorld();
}

//----------------------------------------------------------------------//
//
//! @brief 非同期ロードのリクエスト処理
//
//! @param assetList           ロードするアセットのパスリスト
//! @param delegate            ロード終了時に呼ばれるデリゲート
//! @param priority            プライオリティ指定
//! @param bManageActiveHandle 
//! @param bStartStalled       
//! @param name                
//
//! @retval 非同期ロードハンドルへのポインタ
//
//----------------------------------------------------------------------//
TSharedPtr<FStreamableHandle> SNUtility::RequestAsyncLoad(TArray<FSoftObjectPath>& assetList, FStreamableDelegate delegate, TAsyncLoadPriority priority, bool bManageActiveHandle, bool bStartStalled, FString name){
	// アセットリストに何も登録されていない場合は終了
	if(assetList.Num() <= 0){
		return nullptr;
	}
	// ストリームマネージャーを取得
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	// ロードをリクエスト
	TSharedPtr<FStreamableHandle> handle(Streamable.RequestAsyncLoad(assetList, delegate, priority, bManageActiveHandle, bStartStalled, name));
	
	return handle;
}

USNInputManagerSubsystem* SNUtility::GetSNInputManagerSubsystem()
{
	UGameInstance* GameInstance = GetGameInstance<UGameInstance>();

	SNPLUGIN_ASSERT(GameInstance != nullptr, TEXT("GameInstance is nullptr.[%s]"), __FUNCTIONW__);

	USNInputManagerSubsystem* InputManagerSubsystem = GameInstance->GetSubsystem<USNInputManagerSubsystem>();

	SNPLUGIN_ASSERT(InputManagerSubsystem != nullptr, TEXT("InputManagerSubsystem is nullptr.[%s]"), __FUNCTIONW__);

	return InputManagerSubsystem;
}

TSharedPtr<const FUniqueNetId> SNUtility::GetUniqueNetId(APlayerController* PlayerController)
{
	TSharedPtr<const FUniqueNetId> UniqueNetId;

			
	if(ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player))
	{
		UniqueNetId = LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId();	
	} else
	if(UNetConnection* NetConnection = Cast<UNetConnection>(PlayerController->Player))
	{
		UniqueNetId = NetConnection->PlayerId.GetUniqueNetId();
	} else
	{
		SNPLUGIN_LOG(TEXT("Unique Net Id is None."));
	}

	return UniqueNetId;
}

bool SNUtility::IsServer(UWorld* World)
{
	return UKismetSystemLibrary::IsServer(World);
}

void SNUtility::Travel(const FString& MapName)
{
	USNGameInstance* GameInstance(GetGameInstance<USNGameInstance>());

	SNPLUGIN_ASSERT(GameInstance != nullptr, TEXT("GameInstance is nullptr."));

	if(GameInstance->IsEnabledOnlineSystem() == true)
	{
		GameInstance->GetOnlineSystem()->MapTravel(MapName);
	} else
	{
		SNPLUGIN_LOG(TEXT("Open Level : %s"), *MapName);
		
		UGameplayStatics::OpenLevel(GetWorld(), *MapName, true, "listen");
	}
}

