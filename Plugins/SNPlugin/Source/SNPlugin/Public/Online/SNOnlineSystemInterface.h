// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UObject/Interface.h"
#include "SNOnlineSystemInterface.generated.h"

class USNOnlineSystem;
class USNOnlineHostSessionRequest;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USNOnlineSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SNPLUGIN_API ISNOnlineSystemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	//! @{@name ログイン処理
	virtual void Login(IOnlineIdentityPtr Identity) = 0;
	//! @}
	
	//! @{@name ログイン済みかどうかチェック
	virtual bool IsLoggedIn(IOnlineIdentityPtr Identity) const = 0;
	//! @}
	
	//! @{@name ホストとしてセッションを生成
	virtual bool HostSession(IOnlineSessionPtr Sessions, USNOnlineHostSessionRequest* SessionRequest, const FName& Name) = 0;
	//! @}
	
	//! @{@name セッションを検索
	virtual void FindSession(IOnlineSessionPtr Sessions, TSharedPtr<class FOnlineSessionSearch>& SearchSettings) = 0;
	//! @}
	
	//! @{@name セッションに参加
	virtual void JoinSession(IOnlineSessionPtr Sessions, const FOnlineSessionSearchResult& SearchResult) = 0;
	//! @}
	
	//! @{@name セッションを終了
	virtual void KillSession(IOnlineSessionPtr Sessions, const FName& SessionName) = 0;
	//! @}
};
