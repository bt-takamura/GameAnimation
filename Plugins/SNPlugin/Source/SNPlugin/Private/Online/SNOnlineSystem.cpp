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
#include "Online/SNOnlineCommonSetting.h"
#include "Online/SNOnlineSystemEOSV1.h"
#include "Online/SNOnlineSystemNull.h"


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
			
			Identity->AddOnLoginCompleteDelegate_Handle(Id, FOnLoginCompleteDelegate::CreateUObject(this, &USNOnlineSystem::OnLoginComplete));
		}
		
		OnlineImpl->Login(Identity);
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
bool USNOnlineSystem::IsLoggedIn() const {
	
	IOnlineSubsystem* OnlineSubsystem(Online::GetSubsystem(GetWorld()));
	
	SNPLUGIN_ASSERT(OnlineSubsystem != nullptr, TEXT("OnlineSubsystem is nullptr"));
	
	IOnlineIdentityPtr Identity(OnlineSubsystem->GetIdentityInterface());
	
	return OnlineImpl->IsLoggedIn(Identity);
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
	// リクエストが設定されていない場合は処理を終了
	if(HostSessionRequest == nullptr){
		
		SNPLUGIN_ERROR(TEXT("Host Session Request is nullptr."));
		
		return false;
	}
	
	bool Result = false;
	
	IOnlineSubsystem* const OnlineSubsystem = Online::GetSubsystem(GetWorld());
	
	SNPLUGIN_ASSERT(OnlineSubsystem != nullptr, TEXT("OnlineSubsystem is nullptr"));
	
	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	
	if(Sessions.IsValid()){
		
		HostSessionRequest->MaxPlayerNum = ConnectionNum;
		
		Sessions->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &USNOnlineSystem::OnCreateSessionComplete));
		
		Result = OnlineImpl->HostSession(Sessions, HostSessionRequest, Name);
	}
	
	return Result;
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
		
		Sessions->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &USNOnlineSystem::OnFindSessionsComplete));
		
		OnlineImpl->FindSession(Sessions, SearchSettings);
	}
}

//----------------------------------------------------------------------//
//
//! @brief セッションに参加
//
//! @param SearchResult 参加するセッションの検索結果
//
//----------------------------------------------------------------------//
void USNOnlineSystem::JoinSession(const FOnlineSessionSearchResult& SearchResult){
	
	IOnlineSubsystem* const OnlineSubsystem = Online::GetSubsystem(GetWorld());
	
	if(OnlineSubsystem != nullptr){
		
		IOnlineSessionPtr Sessions(OnlineSubsystem->GetSessionInterface());
		
		if(Sessions.IsValid()){
			
			Sessions->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &USNOnlineSystem::OnJoinSessionComplete));
			
			OnlineImpl->JoinSession(Sessions, SearchResult);
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
		OnlineImpl->KillSession(Sessions, SessionName);
		
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
	
	if(HostSessionRequestClass.IsNull() == false){
		// クラス情報をロード
		UClass* Class(HostSessionRequestClass.LoadSynchronous());
		
		SNPLUGIN_ASSERT(Class != nullptr, TEXT("Host Session Request Class is nullptr."));
		// ホスト生成リクエスト設定をロード
		HostSessionRequest = NewObject<USNOnlineHostSessionRequest>(this, Class, TEXT("HostSessionRequest"));
	}
	
	IOnlineSubsystem* const OnlineSubsystem = Online::GetSubsystem(GetWorld());
	
	SNPLUGIN_ASSERT(OnlineSubsystem != nullptr, TEXT("OnlineSubsystem is nullptr"));
	
	FName PlatformName(OnlineSubsystem->GetSubsystemName());
	
	if(PlatformName == TEXT("NULL")){
		OnlineImpl = NewObject<USNOnlineSystemNull>();
	} else
	if(PlatformName == TEXT("EOS")){
#if COMMONUSER_OSSV1
		OnlineImpl = NewObject<USNOnlineSystemEOSV1>();
#else
		
#endif
	}
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

