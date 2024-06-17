// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SNOnlineGameModeBase.generated.h"

/**
 * オンライン対応用ゲームベース
 */
UCLASS()
class SNPLUGIN_API ASNOnlineGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	
	//! @{@name ログインが終了した後の処理
	virtual void PostLogin(APlayerController* NewPlayer) override;
	//! @}
	
	//! @{@name ゲームの状態を初期化
	virtual void InitGameState() override;
	//! @}

protected:
	
	//! @{@name 新しく参加したプレイヤーの初期化処理
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	//! @}
	
	//! @{@name プレイヤーのスタート位置を取得する
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	//! @}
	
private:
	
	//! @{@name プレイヤースタートへのポインタを取得
	AActor* ChoosePlayerStart(AController* Player);
	//! @}
	
	//!< プレイヤーのIDリスト
	UPROPERTY()
	TArray<int> PlayerIDList;
	
	//!< プレイヤースタートのリスト
	UPROPERTY()
	TArray<TObjectPtr<AActor>> PlayerStartList;
};
