// Fill out your copyright notice in the Description page of Project Settings.
#include "Online\SNOnlineSystem.h"

#include "SNDef.h"
#include "Utility/SNUtility.h"

#include "GameFramework/PlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionClient.h"
#include "OnlineSessionSettings.h"


//----------------------------------------------------------------------//
//
//! @brief ログイン処理
//
//----------------------------------------------------------------------//
void USNOnlineSystem::Login(){
	
	IOnlineSubsystem* OnlineSubsystem(Online::GetSubsystem(GetWorld()));
	
	SNPLUGIN_ASSERT(OnlineSubsystem != nullptr, TEXT("OnlineSubsystem is nullptr"));
	
	IOnlineIdentityPtr Identity(OnlineSubsystem->GetIdentityInterface());
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

				
				//FOnlineAccountCredentials Credentials(TEXT("persistentauth"), TEXT(""), TEXT(""));
				FOnlineAccountCredentials Credentials(TEXT("AccountPortal"), TEXT(""), TEXT(""));
				//FOnlineAccountCredentials Credentials(TEXT("Developer"), TEXT("localhost:8080"), TEXT("Satoshi"));
				
				SNPLUGIN_WARNING(TEXT("CommandLine : %s"), FCommandLine::Get());

				FName t0;
				FName t1;
				FName t2;

				//FCommandLine::Set(TEXT("AUTH_TYPE=Developer"));
				//FCommandLine::Set(TEXT("AUTH_LOGIN=localhost:8080"));
				//FCommandLine::Set(TEXT("AUTH_PASSWORD=Satoshi"));

				//FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), t0);
				//FParse::Value(FCommandLine::Get(), TEXT("AUTH_LOGIN="), t1);
				//FParse::Value(FCommandLine::Get(), TEXT("AUTH_PASSWORD="), t2);
				
				
				SNPLUGIN_LOG(TEXT("CommandLine : %s"), FCommandLine::Get());
				
				Identity->AddOnLoginCompleteDelegate_Handle(Id, FOnLoginCompleteDelegate::CreateUObject(this, &USNOnlineSystem::OnLoginComplete));
				
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
//! @brief ホストとしてセッションを生成
//
//! @retval true  正常終了
//! @retval false 異常終了
//
//----------------------------------------------------------------------//
bool USNOnlineSystem::HostSession(int ConnectionNum, FName Name){
	
	IOnlineSubsystem* const OnlineSubsystem = Online::GetSubsystem(GetWorld());
	
	SNPLUGIN_ASSERT(OnlineSubsystem != nullptr, TEXT("OnlineSubsystem is nullptr"));

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	
	if(Sessions.IsValid()){
		
		TSharedPtr<class FOnlineSessionSettings> SessionSettings(MakeShareable(new FOnlineSessionSettings()));
		
		SNPLUGIN_ASSERT(SessionSettings != nullptr, TEXT("Failed to allocate Session Settings."));
		// 生成するセッションの設定
		SessionSettings->NumPublicConnections			= ConnectionNum;
		SessionSettings->NumPrivateConnections			= 0;
		SessionSettings->bIsLANMatch					= false;
		SessionSettings->bShouldAdvertise				= bShouldIdAdvertise;
		SessionSettings->bAllowJoinInProgress			= bAllowJoinInProgress;
		SessionSettings->bAllowInvites					= bAllowInvites;
		SessionSettings->bUsesPresence					= bUsesPresence;
		SessionSettings->bAllowJoinViaPresence			= bAllowJoinViaPresense;
		SessionSettings->bUseLobbiesIfAvailable			= bUseLobbiesIfAvailable;
		SessionSettings->bUseLobbiesVoiceChatIfAvailable= bUseLobbiesVoiceChatIfAvailable;
		
		SessionSettings->Set(SEARCH_KEYWORDS, Name.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
		
		Sessions->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &USNOnlineSystem::OnCreateSessionComplete));
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

				MaxPlayerNum = ConnectionNum;
				
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
//----------------------------------------------------------------------//
void USNOnlineSystem::FindSession(){
	
	IOnlineSubsystem* const OnlineSubsystem = Online::GetSubsystem(GetWorld());
	
	SNPLUGIN_ASSERT(OnlineSubsystem != nullptr, TEXT("OnlineSubsystem is nullptr"));
	
	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	
	if(Sessions.IsValid()){
		
		SearchSettings = MakeShareable(new FOnlineSessionSearch());
		
		SNPLUGIN_ASSERT(SearchSettings != nullptr, TEXT("Failed to allocate Session Search Settings."));

		SearchSettings->SearchResults.Empty();
		
		SearchSettings->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
		
		Sessions->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &USNOnlineSystem::OnFindSessionsComplete));

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
}

//----------------------------------------------------------------------//
//
//! @brief セッションを終了
//
//----------------------------------------------------------------------//
void USNOnlineSystem::KillSession(const FName& SessionName){
	
	IOnlineSubsystem* const OnlineSubsystem = Online::GetSubsystem(GetWorld());
	
	SNPLUGIN_ASSERT(OnlineSubsystem != nullptr, TEXT("OnlineSubsystem is nullptr"));
	
	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	
	if(Sessions.IsValid()){
		// セッションを終了
		Sessions->DestroySession(SessionName);
		
		ConnectURL = TEXT("");
	}
}

//----------------------------------------------------------------------//
//
//! @brief ログインが完了した際に呼ばれるデリゲート
//
//! @param LocalUserNum   ユーザー数
//! @param bWasSuccessful ログインが完了したかのフラグ
//! @param UserId         ユーザーID
//! @param Error          エラーの内容
//
//----------------------------------------------------------------------//
void USNOnlineSystem::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error){
	
	if(bWasSuccessful == false){
		
		SNPLUGIN_ERROR(TEXT("Login Failed : [%s] : %s"), *UserId.ToString(), *Error);
		
		return;
	}
	
	IOnlineIdentityPtr Identity(Online::GetIdentityInterface(GetWorld()));
	
	if(Identity.IsValid()){
		
		APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
		
		SNPLUGIN_ASSERT(PlayerController != nullptr, TEXT("PlayerController is nullptr"));
		
		ULocalPlayer* LocalPlayer(PlayerController->GetLocalPlayer());
		
		if(LocalPlayer != nullptr){
			
			int Id = LocalPlayer->GetControllerId();
			
			FUniqueNetIdRepl uniqueId = PlayerController->PlayerState->GetUniqueId();
			
			uniqueId.SetUniqueNetId(FUniqueNetIdWrapper(LocalPlayer->GetCachedUniqueNetId()).GetUniqueNetId());
			
			PlayerController->PlayerState->SetUniqueId(uniqueId);
			
			ELoginStatus::Type Status = Identity->GetLoginStatus(Id);
			
			SNPLUGIN_LOG(TEXT("Login Status : %s"), ELoginStatus::ToString(Status));
		}
	}
}

//----------------------------------------------------------------------//
//
//! @brief セッション生成に成功した際に呼ばれるデリゲート
//
//! @param InSessionName  セッション名
//! @param bWasSuccessful セッション生成に成功したかどうかのフラグ
//
//----------------------------------------------------------------------//
void USNOnlineSystem::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful){
	
	if(bWasSuccessful == true){
		SNPLUGIN_LOG(TEXT("Create Session Succeed [%s]."), *InSessionName.ToString());
	} else {
		SNPLUGIN_ERROR(TEXT("Failed to Session Succeed [%s]."), *InSessionName.ToString());
	}
	
	if(OnCompleteHostSession.IsBound()){
		OnCompleteHostSession.Broadcast(InSessionName, bWasSuccessful);
	}
}

//----------------------------------------------------------------------//
//
//! @brief セッションの検索が完了した際に呼ばれるデリゲート
//
//! @param bWasSuccessful セッションの検索に成功したかのフラグ
//
//----------------------------------------------------------------------//
void USNOnlineSystem::OnFindSessionsComplete(bool bWasSuccessful){
	
	if(bWasSuccessful == true){
		
		SNPLUGIN_LOG(TEXT("Find Session: Success"));
		// ※エディタ同士の場合検索結果が0になり、セッション検索に失敗したことになります。
		//   スタンドアローン起動なら大丈夫です。
		if(SearchSettings->SearchResults.Num() == 0){
			SNPLUGIN_LOG(TEXT("No session found."));
		} else {
			
			const TCHAR* SessionId = *SearchSettings->SearchResults[0].GetSessionIdStr();
			// DISPLAY_LOG("Session ID: %s", SessionId);
			// JoinSession(SearchSettings->SearchResults[0]);
		}
	} else {
		// セッション検索失敗
		SNPLUGIN_LOG(TEXT("Find Session: Fail"));
	}

	if(OnCompleteFindSession.IsBound())
	{
		OnCompleteFindSession.Broadcast(bWasSuccessful);	
	}
}

//----------------------------------------------------------------------//
//
//! @brief セッションに参加
//
//! @param SearchResult 参加するセッションの検索結果
//
//----------------------------------------------------------------------//
void USNOnlineSystem::JoinSession(FOnlineSessionSearchResult SearchResult){
	
	IOnlineSubsystem* const OnlineSubsystem = Online::GetSubsystem(GetWorld());
	
	if(OnlineSubsystem != nullptr){
		
		IOnlineSessionPtr Sessions(OnlineSubsystem->GetSessionInterface());
		
		if(Sessions.IsValid()){
			
			if(SearchResult.IsValid()){
				
				Sessions->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &USNOnlineSystem::OnJoinSessionComplete));
				
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
	}
}

void USNOnlineSystem::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result){
	
	IOnlineSubsystem* const OnlineSubsystem = Online::GetSubsystem(GetWorld());
	
	if(OnlineSubsystem != nullptr){
		
		IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
		
		if(Sessions.IsValid()){
			
			if(Result == EOnJoinSessionCompleteResult::Success){
				
				if(Sessions->GetResolvedConnectString(InSessionName, ConnectURL)){
					
					UE_LOG_ONLINE_SESSION(Log, TEXT("Join session: traveling to %s"), *ConnectURL);
					
					APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
					
					SNPLUGIN_ASSERT(PlayerController != nullptr, TEXT("PlayerController is nullptr"));
					
					PlayerController->ClientTravel(ConnectURL, TRAVEL_Absolute);
				}
			}
		}
	}
	
	if(OnCompleteJoinSession.IsBound()){
		OnCompleteJoinSession.Broadcast(InSessionName, Result == EOnJoinSessionCompleteResult::Success ? true : false);	
	}
}

//----------------------------------------------------------------------//
//
//! @brief 初期化処理
//
//----------------------------------------------------------------------//
void USNOnlineSystem::Initialize(){
	
}

//----------------------------------------------------------------------//
//
//! @brief 指定されたプレイヤーコントローラのニックネームを取得
//
//! @param PlayerController プレイヤーコントローラ
//
//! @retval ニックネーム
//
//----------------------------------------------------------------------//
FString USNOnlineSystem::GetNickname(APlayerController* PlayerController) const {
	
	FString Nickname;
	
	IOnlineIdentityPtr Identity(Online::GetIdentityInterface(GetWorld()));
	
	if(Identity.IsValid()){
		
		TSharedPtr<const FUniqueNetId> UniqueNetId(SNUtility::GetUniqueNetId(PlayerController));
		
		if(UniqueNetId.IsValid()){
			Nickname = Identity->GetPlayerNickname(*UniqueNetId);
		}
	}
	
	return Nickname;
}

void USNOnlineSystem::MapTravel(const FString& MapName){

	UWorld* World = GetWorld();
	
	if(World == nullptr){
		
		SNPLUGIN_ERROR(TEXT("MapTravel_OnServer : World is nullptr."));
		
		return;
	}
	
	if(SNUtility::IsServer(World) == true){
		
		World->ServerTravel(MapName, false);

		SNPLUGIN_LOG(TEXT("MapTravel_OnServer : ServerTravel is done."));
		
		//MapTravel_OnMulticast();
	} else
	{
		SNPLUGIN_LOG(TEXT("MapTravel_OnServer : Called in Client."));
	}
}

void USNOnlineSystem::MapTravel_OnMulticast_Implementation()
{
	UWorld* World = GetWorld();
	
	if(World == nullptr){
		
		SNPLUGIN_ERROR(TEXT("MapTravel : World is nullptr."));
		
		return;
	}
	
	if(SNUtility::IsServer(World) == false)
	{
		APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());

		SNPLUGIN_ASSERT(PlayerController != nullptr, TEXT("PlayerController is nullptr."));

		SNPLUGIN_ASSERT(PlayerController->IsLocalController() != false, TEXT("PlayerControler is not Local Controller."));

		PlayerController->ClientTravel(GetConnectURL(), TRAVEL_Absolute);

		SNPLUGIN_LOG(TEXT("MapTravel_OnMulticast : ClientTravel is done."));
	} else
	{
		SNPLUGIN_LOG(TEXT("MapTravel_OnMulticast : Server is coming."));
	}
}

