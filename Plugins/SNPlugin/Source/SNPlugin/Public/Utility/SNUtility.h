// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/AssetManager.h"
#include <Kismet/GameplayStatics.h>

#include "System/SNGameInstance.h"

class USNInputManagerSubsystem;
/**
 * 
 */
class SNPLUGIN_API SNUtility
{
public:
	 SNUtility() = delete;
	~SNUtility() = delete;
	
	static UWorld* GetWorld();
	
	//! @{@name 非同期ロードのリクエスト処理
	template<class T>
	static TSharedPtr<FStreamableHandle> RequestAsyncLoad(TArray<FSoftObjectPath>& assetList, T* pClass, typename TMemFunPtrType<false, T, void ()>::Type pFunc, TAsyncLoadPriority priority = FStreamableManager::DefaultAsyncLoadPriority, bool bManageActiveHandle = false, bool bStartStalled = false, FString name = TEXT("ArrayDelegate"));
	
	static TSharedPtr<FStreamableHandle> RequestAsyncLoad(TArray<FSoftObjectPath>& assetList, FStreamableDelegate delegate = FStreamableDelegate(), TAsyncLoadPriority priority = FStreamableManager::DefaultAsyncLoadPriority, bool bManageActiveHandle = false, bool bStartStalled = false, FString name = TEXT("ArrayDelegate"));
	//! @}

	static USNInputManagerSubsystem* GetSNInputManagerSubsystem();
	
	//! @{@name 指定されたプレイヤーコントローラのネットワークIDを取得
	static TSharedPtr<const FUniqueNetId> GetUniqueNetId(APlayerController* PlayerController);
	//! @}
	
	//! @{@name サーバーかどうかの判定
	static bool IsServer(UWorld* World);
	//! @}

	static void Travel(const FString& MapName);

	//! @{@name ゲームインスタンスを取得
	template<class T>
	static T*	GetGameInstance();
	//! @}
	
	//! @{@name プレイヤーコントローラを取得
	template<class T>
	static T*	GetPlayerController();
	//! @}
	
	//! @{@name カレントのプレイヤーを取得
	template<class T>
	static T*	GetCurrentPlayer();
	//! @}
	
	//! @{@name プレイヤーカメラマネージャーを取得
	template<class T>
	static T* GetPlayerCameraManager();
	//! @}
	
	//! @{@name カレントに設定されているカメラアクターを取得
	template<class T>
	static T* GetCurrentCameraActor();
	//! @}

	template<class T>
	static T* GetOnlineSystem();
};

//----------------------------------------------------------------------//
//
//! @brief 非同期ロードのリクエスト処理
//
//! @param assetList            ロードするアセットのパスリスト
//! @param pClass               クラスへのポインタ
//! @param TMemFunPtrType<false ロード終了デリゲート
//
//! @retval 非同期ロードハンドルへのポインタ
//
//----------------------------------------------------------------------//
template<class T>
inline TSharedPtr<FStreamableHandle> SNUtility::RequestAsyncLoad(TArray<FSoftObjectPath>& assetList, T* pClass, typename TMemFunPtrType<false, T, void ()>::Type pFunc, TAsyncLoadPriority priority, bool bManageActiveHandle, bool bStartStalled, FString name){
	
	if((pClass != nullptr) && (pFunc != nullptr)){
		return RequestAsyncLoad(assetList, FStreamableDelegate::CreateUObject(pClass, pFunc), priority, bManageActiveHandle, bStartStalled, name);
	} else {
		return RequestAsyncLoad(assetList, FStreamableDelegate(), priority, bManageActiveHandle, bStartStalled, name);
	}
}

//----------------------------------------------------------------------//
//
//! @brief ゲームインスタンスを取得
//
//! @retval ゲームインスタンスへのポインタ
//
//----------------------------------------------------------------------//
template<class T>
FORCEINLINE T* SNUtility::GetGameInstance(){
	
	if(UWorld* pPrimaryWorld = SNUtility::GetWorld()){
		return Cast<T>(pPrimaryWorld->GetGameInstance());
	}
	
	return nullptr;
}

//----------------------------------------------------------------------//
//
//! @brief プレイヤーコントローラを取得
//
//! @retval プレイヤーコントローラへのポインタ
//
//----------------------------------------------------------------------//
template<class T>
FORCEINLINE T* SNUtility::GetPlayerController(){

	UGameInstance* GameInstance = SNUtility::GetGameInstance<UGameInstance>();

	if(GameInstance != nullptr)
	{
		return Cast<T>(UGameplayStatics::GetPlayerController(GameInstance->GetWorld(), 0));
	}
	
	return nullptr;
}

//----------------------------------------------------------------------//
//
//! @brief カレントのプレイヤーへのポインタを取得
//
//! @retval カレントプレイヤーへのポインタ
//
//----------------------------------------------------------------------//
template<class T>
FORCEINLINE T* SNUtility::GetCurrentPlayer(){
	
	APawn* Player(nullptr);
	
	APlayerController* PlayerController(GetPlayerController<APlayerController>());
	
	if(PlayerController != nullptr){
		Player = PlayerController->GetPawn();
	}
	
	return Cast<T>(Player);
}

//----------------------------------------------------------------------//
//
//! @brief プレイヤーカメラマネージャーを取得
//
//! @retval プレイヤーカメラマネージャーへのポインタ
//
//----------------------------------------------------------------------//
template <class T>
FORCEINLINE T* SNUtility::GetPlayerCameraManager(){
	
	APlayerCameraManager* PlayerCameraManager(nullptr);
	
	APlayerController* PlayerController(GetPlayerController<APlayerController>());
	
	if(PlayerController != nullptr){
		PlayerCameraManager = PlayerController->PlayerCameraManager;
	}
	
	return Cast<T>(PlayerCameraManager);
}


//----------------------------------------------------------------------//
//
//! @brief カレントに設定されているカメラアクターを取得
//
//! @retval カレントに設定されているカメラアクターへのポインタ
//
//----------------------------------------------------------------------//
template <class T>
FORCEINLINE T* SNUtility::GetCurrentCameraActor(){
	
	AActor* CameraActor(nullptr);
	
	APlayerCameraManager* CameraManager(GetPlayerCameraManager<APlayerCameraManager>());
	
	if(CameraManager != nullptr){
		CameraActor = CameraManager->GetViewTarget();
	}
	
	return Cast<T>(CameraActor);
}

template <class T>
T* SNUtility::GetOnlineSystem()
{
	USNGameInstance* GameInstance(SNUtility::GetGameInstance<USNGameInstance>());

	if(GameInstance == nullptr)
	{
		return nullptr;
	}

	return Cast<T>(GameInstance->GetOnlineSystem());
}

