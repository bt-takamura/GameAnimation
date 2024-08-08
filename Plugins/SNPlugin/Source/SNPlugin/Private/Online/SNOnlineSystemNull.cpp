// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/SNOnlineSystemNull.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "SNDef.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Online/OnlineSessionNames.h"
#include "Online/SNOnlineCommonSetting.h"
#include "Online/SNOnlineSystem.h"
#include "Utility/SNUtility.h"

//----------------------------------------------------------------------//
//
//! @brief ログイン処理
//
//! @param Identity 認証インターフェイス
//
//----------------------------------------------------------------------//
void USNOnlineSystemNull::Login(IOnlineIdentityPtr Identity){
	// OnlineIdentityが有効なものかチェック
	if(Identity.IsValid()){
		// プレイヤーコントローラを取得
		APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
		
		if(PlayerController != nullptr){
			// ローカルプレイヤーを取得
			ULocalPlayer* LocalPlayer(PlayerController->GetLocalPlayer());
			// コントローラIDを取得
			int Id = LocalPlayer->GetControllerId();
			
			ELoginStatus::Type Status = Identity->GetLoginStatus(Id);
			
			SNPLUGIN_LOG(TEXT("Login Status : %s"), ELoginStatus::ToString(Status));
		}
	}
}

//----------------------------------------------------------------------//
//
//! @brief ログイン済みかどうかチェック
//
//! @param Identity 認証インターフェイス
//
//! @retval true  ログイン済み
//! @retval false 未ログイン
//
//----------------------------------------------------------------------//
bool USNOnlineSystemNull::IsLoggedIn(IOnlineIdentityPtr Identity) const {
	// OnlineIdentityが有効なものかチェック
	if(Identity.IsValid()){
		// プレイヤーコントローラを取得
		APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
		
		if(PlayerController != nullptr){
			// ローカルプレイヤーを取得
			ULocalPlayer* LocalPlayer(PlayerController->GetLocalPlayer());
			// コントローラIDを取得
			int Id = LocalPlayer->GetControllerId();
			
			return (Identity->GetLoginStatus(Id) == ELoginStatus::LoggedIn) ? true : false;
		}
	}
	
	return false;
}

//----------------------------------------------------------------------//
//
//! @brief ホストとしてセッションを生成
//
//! @param Sessions       セッションインターフェイス
//! @param SessionRequest セッションリクエスト
//! @param Name           セッション名
//
//! @retval true  正常終了
//! @retval false 異常終了
//
//----------------------------------------------------------------------//
bool USNOnlineSystemNull::HostSession(IOnlineSessionPtr Sessions, USNOnlineHostSessionRequest* SessionRequest, const FName& Name){
	
	if(Sessions.IsValid()){
		
		TSharedPtr<class FOnlineSessionSettings> SessionSettings(MakeShareable(new FOnlineSessionSettings()));
		
		SNPLUGIN_ASSERT(SessionSettings != nullptr, TEXT("Failed to allocate Session Settings."));
		// 生成するセッションの設定
		SessionSettings->NumPublicConnections			= SessionRequest->MaxPlayerNum;
		SessionSettings->NumPrivateConnections			= 0;
		SessionSettings->bIsLANMatch					= false;
		SessionSettings->bShouldAdvertise				= SessionRequest->bShouldIdAdvertise;
		SessionSettings->bAllowJoinInProgress			= SessionRequest->bAllowJoinInProgress;
		SessionSettings->bAllowInvites					= SessionRequest->bAllowInvites;
		SessionSettings->bUsesPresence					= SessionRequest->bUsesPresence;
		SessionSettings->bAllowJoinViaPresence			= SessionRequest->bAllowJoinViaPresense;
		SessionSettings->bUseLobbiesIfAvailable			= SessionRequest->bUseLobbiesIfAvailable;
		SessionSettings->bUseLobbiesVoiceChatIfAvailable= SessionRequest->bUseLobbiesVoiceChatIfAvailable;
		
		SessionSettings->Set(SEARCH_KEYWORDS, Name.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
		// プレイヤーコントローラを取得
		APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
		// プレイヤーコントローラがない場合はアサート
		SNPLUGIN_ASSERT(PlayerController != nullptr, TEXT("PlayerController is nullptr"));
		
		TSharedPtr<const FUniqueNetId> UniqueNetIdPtr(SNUtility::GetUniqueNetId(PlayerController));
		// ネットワームIDが有効かチェック
		if(UniqueNetIdPtr.IsValid()){
			
			bool bResult = Sessions->CreateSession(*UniqueNetIdPtr, Name, *SessionSettings);
			
			if(bResult == true){
				
				SNPLUGIN_LOG(TEXT("Success to Create Session."))
				
				return true;
				
			} else {
				SNPLUGIN_ERROR(TEXT("CreateSession: Fail"));
			}
		} else {
			SNPLUGIN_ERROR(TEXT("CreateSession : Unique Net Id is none."))
		}
	}
	
	return false;
}

//----------------------------------------------------------------------//
//
//! @brief セッションを検索
//
//! @param Sessions       セッションインターフェイス
//! @param SearchSettings 検索設定
//
//----------------------------------------------------------------------//
void USNOnlineSystemNull::FindSession(IOnlineSessionPtr Sessions, TSharedPtr<class FOnlineSessionSearch>& SearchSettings){
	
	SearchSettings->SearchResults.Empty();
	// クエリの設定
	SearchSettings->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	
	TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SearchSettings.ToSharedRef();
	// プレイヤーコントローラを取得
	APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
	
	SNPLUGIN_ASSERT(PlayerController != nullptr, TEXT("PlayerController is nullptr"));
	
	TSharedPtr<const FUniqueNetId> UniqueNetIdPtr(SNUtility::GetUniqueNetId(PlayerController));
	
	if(UniqueNetIdPtr.IsValid()){
		// セッションの検索を開始
		bool bResult = Sessions->FindSessions(*UniqueNetIdPtr, SearchSettingsRef);
		
		if(bResult == true){
			SNPLUGIN_LOG(TEXT("Success to FindSession."))
		} else {
			SNPLUGIN_ERROR(TEXT("FindSession: Fail"));
		}
	} else {
		SNPLUGIN_ERROR(TEXT("FindSession : Unique Net Id is none."))
	}
}

//----------------------------------------------------------------------//
//
//! @brief セッションに参加
//
//! @param Sessions     セッションインターフェイス
//! @param SearchResult 参加するセッションの情報
//
//----------------------------------------------------------------------//
void USNOnlineSystemNull::JoinSession(IOnlineSessionPtr Sessions, const FOnlineSessionSearchResult& SearchResult){
	
	if(SearchResult.IsValid()){
		// プレイヤーコントローラを取得
		APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
		
		SNPLUGIN_ASSERT(PlayerController != nullptr, TEXT("PlayerController is nullptr"));
		
		TSharedPtr<const FUniqueNetId> UniqueNetIdPtr(SNUtility::GetUniqueNetId(PlayerController));
		
		if(UniqueNetIdPtr.IsValid()){
			
			FName JoinSessionName(NAME_None);
			// 検索結果にキーワードが含まれているかチェック
			if(SearchResult.Session.SessionSettings.Settings.Contains(SEARCH_KEYWORDS) == true){
				// オンライン設定を取得
				const FOnlineSessionSetting& Setting(SearchResult.Session.SessionSettings.Settings[SEARCH_KEYWORDS]);
				// データからセッション名を取得
				JoinSessionName = *Setting.Data.ToString();
			}
			// セッションに参加
			Sessions->JoinSession(*UniqueNetIdPtr, JoinSessionName, SearchResult);
		} else {
			SNPLUGIN_ERROR(TEXT("JointSession : Unique Net Id is none."))
		}
	} else {
		SNPLUGIN_ERROR(TEXT("Invalid session."));
	}
}

//----------------------------------------------------------------------//
//
//! @brief セッションを終了
//
//! @param Sessions    セッションインターフェイス
//! @param SessionName セッション名
//
//----------------------------------------------------------------------//
void USNOnlineSystemNull::KillSession(IOnlineSessionPtr Sessions, const FName& SessionName){
	// セッションを終了
	Sessions->DestroySession(SessionName);
}
