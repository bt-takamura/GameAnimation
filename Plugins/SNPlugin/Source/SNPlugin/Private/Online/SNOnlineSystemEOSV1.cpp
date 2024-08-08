// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/SNOnlineSystemEOSV1.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "SNDef.h"
#include "Online/OnlineSessionNames.h"
#include "Online/SNOnlineCommonSetting.h"
#include "Utility/SNUtility.h"

namespace{
	const FName SETTING_ONLINESUBSYSTEM_VERSION(TEXT("OSSv1"));
}

//----------------------------------------------------------------------//
//
//! @brief ログイン処理
//
//! @param Identity 認証インターフェイス
//
//----------------------------------------------------------------------//
void USNOnlineSystemEOSV1::Login(IOnlineIdentityPtr Identity){
	// OnlineIdentityが有効なものかチェック
	if(Identity.IsValid()){
		// プレイヤーコントローラを取得
		APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
		
		SNPLUGIN_ASSERT(PlayerController != nullptr, TEXT("PlayerController is nullptr"));
		// ローカルプレイヤーを取得
		ULocalPlayer* LocalPlayer(PlayerController->GetLocalPlayer());
		
		if(LocalPlayer != nullptr){
			// コントローラIDを取得
			int Id = LocalPlayer->GetControllerId();
			
			if(Identity->GetLoginStatus(Id) != ELoginStatus::LoggedIn){
				
				UGameInstance* GameInstance(SNUtility::GetGameInstance<UGameInstance>());
				
				if(GameInstance != nullptr){
					// 専用サーバーを持っているかチェック
					bIsDedicatedServer = GameInstance->IsDedicatedServerInstance();
				}
				
				//FOnlineAccountCredentials Credentials(TEXT("persistentauth"), TEXT(""), TEXT(""));
				FOnlineAccountCredentials Credentials(TEXT("AccountPortal"), TEXT(""), TEXT(""));
				//FOnlineAccountCredentials Credentials(TEXT("Developer"), TEXT("localhost:8080"), TEXT("Satoshi"));
				
				SNPLUGIN_WARNING(TEXT("CommandLine : %s"), FCommandLine::Get());
				
				//FName t0;
				//FName t1;
				//FName t2;
				
				//FCommandLine::Set(TEXT("AUTH_TYPE=Developer"));
				//FCommandLine::Set(TEXT("AUTH_LOGIN=localhost:8080"));
				//FCommandLine::Set(TEXT("AUTH_PASSWORD=Satoshi"));
				
				//FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), t0);
				//FParse::Value(FCommandLine::Get(), TEXT("AUTH_LOGIN="), t1);
				//FParse::Value(FCommandLine::Get(), TEXT("AUTH_PASSWORD="), t2);
				
				
				SNPLUGIN_LOG(TEXT("CommandLine : %s"), FCommandLine::Get());
				
				//Identity->AutoLogin(Id);
				
				Identity->Login(Id, Credentials);
			}
			
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
bool USNOnlineSystemEOSV1::IsLoggedIn(IOnlineIdentityPtr Identity) const {
	
	bool Result = false;
	// OnlineIdentityが有効なものかチェック
	if(Identity.IsValid()){
		// プレイヤーコントローラを取得
		APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
		
		if(PlayerController != nullptr){
			// ローカルプレイヤーを取得
			ULocalPlayer* LocalPlayer(PlayerController->GetLocalPlayer());
			
			if(LocalPlayer != nullptr){
				// コントローラIDを取得
				int Id = LocalPlayer->GetControllerId();
				
				if(Identity->GetLoginStatus(Id) == ELoginStatus::LoggedIn){
					Result = true;
				}
			}
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
bool USNOnlineSystemEOSV1::HostSession(IOnlineSessionPtr Sessions, USNOnlineHostSessionRequest* SessionRequest, const FName& Name)
{
	APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
	
	if(PlayerController != nullptr){
		
		FUniqueNetIdPtr UserId;
		
		ULocalPlayer* LocalPlayer(PlayerController->GetLocalPlayer());
		
		if(LocalPlayer != nullptr){
			
			FUniqueNetIdRepl UniqueNetId(LocalPlayer->GetPreferredUniqueNetId());
			
			FName Type(UniqueNetId.GetType());
			
			UserId = UniqueNetId.GetV1();
		} else
		if(bIsDedicatedServer){
			
			IOnlineSubsystem* const OnlineSubsystem = Online::GetSubsystem(GetWorld());
			
			UserId = OnlineSubsystem->GetIdentityInterface()->GetUniquePlayerId(0);
		}
		
		if(ensure(UserId.IsValid())){
			
			TSharedPtr<class FOnlineSessionSettings> SessionSettings(MakeShareable(new FOnlineSessionSettings()));
			
			SNPLUGIN_ASSERT(SessionSettings != nullptr, TEXT("Failed to allocate Session Settings."));
			// 生成するセッションの設定
			SessionSettings->NumPublicConnections			= SessionRequest->MaxPlayerNum;
			SessionSettings->NumPrivateConnections			= 0;
			SessionSettings->bIsLANMatch					= (SessionRequest->OnlineMode == ESessionOnlineMode::LAN) ? true : false;
			SessionSettings->bShouldAdvertise				= SessionRequest->bShouldIdAdvertise;
			SessionSettings->bAllowJoinInProgress			= SessionRequest->bAllowJoinInProgress;
			SessionSettings->bAllowInvites					= SessionRequest->bAllowInvites;
			SessionSettings->bUsesPresence					= SessionRequest->bUsesPresence;
			SessionSettings->bAllowJoinViaPresence			= SessionRequest->bAllowJoinViaPresense;
			SessionSettings->bUseLobbiesIfAvailable			= SessionRequest->bUseLobbiesIfAvailable;
			SessionSettings->bUseLobbiesVoiceChatIfAvailable= SessionRequest->bUseLobbiesVoiceChatIfAvailable;
			
			SessionSettings->Set(SEARCH_KEYWORDS, Name.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
			SessionSettings->Set(SETTING_MATCHING_TIMEOUT, 120.0f, EOnlineDataAdvertisementType::ViaOnlineService);
			SessionSettings->Set(SETTING_ONLINESUBSYSTEM_VERSION, true, EOnlineDataAdvertisementType::ViaOnlineService);
			
			bool bResult = Sessions->CreateSession(*UserId, Name, *SessionSettings);
			
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
void USNOnlineSystemEOSV1::FindSession(IOnlineSessionPtr Sessions, TSharedPtr<FOnlineSessionSearch>& SearchSettings){
	
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
void USNOnlineSystemEOSV1::JoinSession(IOnlineSessionPtr Sessions, const FOnlineSessionSearchResult& SearchResult){
	
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
void USNOnlineSystemEOSV1::KillSession(IOnlineSessionPtr Sessions, const FName& SessionName){
	// セッションを終了
	Sessions->DestroySession(SessionName);
}
