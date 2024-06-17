// Fill out your copyright notice in the Description page of Project Settings.


#include "System\SNOnlineGameModeBase.h"
#include "EngineUtils.h"
#include "SNDef.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/PlayerState.h"
#include "Online/SNOnlineSystem.h"
#include "System/SNGameInstance.h"
#include "Utility/SNUtility.h"

//----------------------------------------------------------------------//
//
//! @brief ゲームの状態を初期化
//
//----------------------------------------------------------------------//
void ASNOnlineGameModeBase::InitGameState(){
	
	Super::InitGameState();
	// プレイヤーIDを初期化
	PlayerIDList.Empty();
	
	UWorld* World(GetWorld());
	// nullチェック
	if((World != nullptr) && (PlayerStartList.Num() == 0)){
		// ワールドに配置されているプレイヤースタートをイテレーション
		for(TActorIterator<APlayerStart> It(World) ; It; ++It){
			// ポインタに変換
			if(APlayerStart* PlayerStart = *It){
				// リストに追加
				PlayerStartList.Add(PlayerStart);
			}
		}
	}
	
	SNPLUGIN_LOG(TEXT("Init Game State Done."))
}

//----------------------------------------------------------------------//
//
//! @brief 新しく参加したプレイヤーの初期化処理
//
//! @param NewPlayerController 
//! @param UniqueId            
//! @param Options             
//! @param Portal              
//
//! @retval 
//
//----------------------------------------------------------------------//
FString ASNOnlineGameModeBase::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal){
	
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	// プレイヤーIDを取得
	int PlayerID = NewPlayerController->PlayerState->GetPlayerId();
	// すでに登録されているかチェック
	if(PlayerIDList.Contains(PlayerID) == false){
		PlayerIDList.Add(PlayerID);
	}
	
	SNPLUGIN_LOG(TEXT("Init New Player Done."))
	
	return Result;
}

//----------------------------------------------------------------------//
//
//! @brief ログインが終了した後の処理
//
//! @param NewPlayer ログインしたプレイヤーの情報
//
//! @note ホストはPIE後1回、クライアントはセッションを開始した後に1回呼ばれます。
//
//----------------------------------------------------------------------//
void ASNOnlineGameModeBase::PostLogin(APlayerController* NewPlayer){
	
	Super::PostLogin(NewPlayer);
#if 0 // テスト用のコードなので、一旦コメントアウト
	FString Nickname;
	
	if(ULocalPlayer* LocalPlayer = NewPlayer->GetLocalPlayer())
	{
		Nickname = LocalPlayer->GetNickname();

		
	} else
	if(UNetConnection* NetConnection = NewPlayer->GetNetConnection())
	{
		Nickname = NetConnection->GetPlayerOnlinePlatformName().ToString();
	}

	Nickname = SNUtility::GetGameInstance<USNGameInstance>()->GetOnlineSystem()->GetNickname(NewPlayer);

	SNPLUGIN_LOG(TEXT("%s"), *Nickname);
	
	SNPLUGIN_LOG(TEXT("Post Login.[Player Start Num : %d]"), PlayerStartList.Num());
#endif
}

//----------------------------------------------------------------------//
//
//! @brief プレイヤーのスタート位置を取得する
//
//! @param Player コントローラへのポインタ
//
//! @retval プレイヤースタートへのポインタ
//
//----------------------------------------------------------------------//
AActor* ASNOnlineGameModeBase::ChoosePlayerStart_Implementation(AController* Player){
	
	if(AActor* PlayerStart = ChoosePlayerStart(Player)){
		return PlayerStart;
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

//----------------------------------------------------------------------//
//
//! @brief プレイヤースタートへのポインタを取得
//
//! @param Player コントローラへのポインタ
//
//! @retval プレイヤースタートへのポインタ
//
//----------------------------------------------------------------------//
AActor* ASNOnlineGameModeBase::ChoosePlayerStart(AController* Player){
	
	TArray<APlayerStart*> StartPoints;
	
	for(auto It=PlayerStartList.CreateIterator() ; It; ++It){
		
		if(APlayerStart* Start = Cast<APlayerStart>((*It).Get())){
			StartPoints.Add(Start);
		} else {
			It.RemoveCurrent();
		}
	}
	// プレイヤーステートを取得
	if(APlayerState* PlayerState = Player->GetPlayerState<APlayerState>()){
		// プレイヤーIDを取得
		int PlayerID = PlayerState->GetPlayerId();
		// プレイヤースタートが存在するかチェック
		if(StartPoints.IsEmpty() == false){
			// プレイヤーのIDが含まれてない場合はここで追加
			// ※InitNewPlayerとの呼び出し順が環境によって変わることがあるっぽい
			if(PlayerIDList.Contains(PlayerID) == false){
				PlayerIDList.Add(PlayerID);
			}
			// プレイヤーIDが登録されているインデックスを取得
			int index = PlayerIDList.IndexOfByKey(PlayerID);
			// レンジチェック
			if(index < StartPoints.Num()){
				
				SNPLUGIN_LOG(TEXT("Choose Player Start.[Player Start Index : %d]"), index);
				
				return StartPoints[index];	
			}
		}
	}
	
	SNPLUGIN_LOG(TEXT("Choose Player Start is failed."));
	
	return nullptr;
}
