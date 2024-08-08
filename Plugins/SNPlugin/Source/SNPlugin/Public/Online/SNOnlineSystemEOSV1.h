// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SNOnlineSystemInterface.h"
#include "SNOnlineSystemEOSV1.generated.h"

//!@{@defgroup オンライン
//!@{
//----------------------------------------------------------------------//
//
//! @brief プラットフォーム設定がEOSの場合の通信処理
//
//----------------------------------------------------------------------//
UCLASS()
class SNPLUGIN_API USNOnlineSystemEOSV1 : public UObject, public ISNOnlineSystemInterface
{
	GENERATED_BODY()

public:
	
	//! @{@name ログイン処理
	virtual void Login(IOnlineIdentityPtr Identity) override;
	//! @}
	
	//! @{@name 既にログインしているかチェック
	virtual bool IsLoggedIn(IOnlineIdentityPtr Identity) const override;
	//! @}
	
	//! @{@name ホストとしてセッションを生成
	virtual bool HostSession(IOnlineSessionPtr Sessions, USNOnlineHostSessionRequest* SessionRequest, const FName& Name) override;
	//! @}
	
	//! @{@name セッションを検索
	virtual void FindSession(IOnlineSessionPtr Sessions, TSharedPtr<class FOnlineSessionSearch>& SearchSettings) override;
	//! @}
	
	//! @{@name セッションに参加
	virtual void JoinSession(IOnlineSessionPtr Sessions, const FOnlineSessionSearchResult& SearchResult) override;
	//! @}
	
	//! @{@name セッションを終了
	virtual void KillSession(IOnlineSessionPtr Sessions, const FName& SessionName) override;
	//! @}
	
public:
	//!< セッションを作成するためにLocalPlayerを必要としない専用サーバーかどうか。
	bool bIsDedicatedServer = false;
};
//! @}
//! @}
