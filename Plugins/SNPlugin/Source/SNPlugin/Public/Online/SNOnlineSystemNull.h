// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SNOnlineSystemInterface.h"
#include "UObject/NoExportTypes.h"
#include "SNOnlineSystemNull.generated.h"

//!@{@defgroup オンライン
//!@{
//----------------------------------------------------------------------//
//
//! @brief プラットフォーム設定がNULLの場合の通信処理
//
//! @note NULLの場合、同じLAN内のみマッチング可能。ロビーなどは使用不可。
//
//----------------------------------------------------------------------//
UCLASS()
class SNPLUGIN_API USNOnlineSystemNull : public UObject, public ISNOnlineSystemInterface
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
};
//! @}
//! @}
