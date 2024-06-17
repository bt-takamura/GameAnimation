// Fill out your copyright notice in the Description page of Project Settings.


#include "Scene/StateTree/SNJointSessionTask.h"

#include "OnlineSessionSettings.h"
#include "SNDef.h"
#include "Online/OnlineSessionNames.h"
#include "Online/SNOnlineSystem.h"
#include "UI/Widget/SNMatchingJoinSessionMenu.h"
#include "Utility/SNUtility.h"

EStateTreeRunStatus USNJointSessionTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	EStateTreeRunStatus Result = Super::Tick(Context, DeltaTime);

	if(bExit == true)
	{
		return (bSucceed == true) ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
	}
	
	return Result;
}

EStateTreeRunStatus USNJointSessionTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	EStateTreeRunStatus Result = Super::EnterState(Context, Transition);

	USNOnlineSystem* OnlineSystem(SNUtility::GetOnlineSystem<USNOnlineSystem>());

	if(OnlineSystem != nullptr)
	{
		OnlineSystem->OnCompleteFindSession.AddDynamic(this, &USNJointSessionTask::OnCompleteSearchSession);

		OnlineSystem->OnCompleteJoinSession.AddDynamic(this, &USNJointSessionTask::OnCompleteJoinSession);
		
		OnlineSystem->FindSession();
	}
		
	return Result;
}

void USNJointSessionTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);

	USNOnlineSystem* OnlineSystem(SNUtility::GetOnlineSystem<USNOnlineSystem>());

	if(OnlineSystem != nullptr)
	{
		OnlineSystem->OnCompleteFindSession.Clear();

		OnlineSystem->OnCompleteJoinSession.Clear();
	}
}

void USNJointSessionTask::HudPostLoad()
{
	if(USNMatchingJoinSessionMenu* Menu = GetHudWidget<USNMatchingJoinSessionMenu>())
	{
		Menu->OnJoinButtonClickedDelegate.BindDynamic(this, &USNJointSessionTask::OnStartSearchSession);

		SetVisibleWidget(EWidgetLayer::Layer3, Menu);
	}
}

void USNJointSessionTask::OnStartSearchSession(const FString& Name)
{
	USNOnlineSystem* OnlineSystem(SNUtility::GetOnlineSystem<USNOnlineSystem>());

	if(OnlineSystem != nullptr)
	{
		const TSharedPtr<FOnlineSessionSearch>& SessionList(OnlineSystem->GetSearchSessionList());

		for(auto& Result:SessionList->SearchResults)
		{
			const FOnlineSessionSetting& Setting(Result.Session.SessionSettings.Settings[SEARCH_KEYWORDS]);

			FString RoomName = Setting.Data.ToString();

			if(RoomName == Name)
			{
				OnlineSystem->JoinSession(Result);

				return;
			}
		}
	}
	// ここまで処理が来た場合は接続に失敗
	bSucceed = false;
	// 処理を終了
	bExit = true;
}


void USNJointSessionTask::OnCompleteSearchSession(bool bResult)
{
	USNOnlineSystem* OnlineSystem(SNUtility::GetOnlineSystem<USNOnlineSystem>());

	const TSharedPtr<FOnlineSessionSearch>& SessionList(OnlineSystem->GetSearchSessionList());

	if(SessionList != nullptr)
	{
		// 検索結果が0の場合は終了
		// リトライをいれるべきか...
		if(SessionList->SearchResults.Num() <= 0)
		{
			SNPLUGIN_LOG(TEXT("Host Session is not Found."))
			// 接続失敗判定
			bSucceed = false;
			// 処理を終了
			bExit = true;
			
			return;
		}
		
		USNMatchingJoinSessionMenu* Menu = GetHudWidget<USNMatchingJoinSessionMenu>();

		if(Menu != nullptr)
		{
			for(auto& Result:SessionList->SearchResults)
			{
				const FOnlineSessionSetting& Setting(Result.Session.SessionSettings.Settings[SEARCH_KEYWORDS]);

				FString RoomName = Setting.Data.ToString();

				Menu->ShowRoomItem(RoomName, Result.Session.NumOpenPublicConnections);
			}
		}
	}
}

void USNJointSessionTask::OnCompleteJoinSession(FName SessionName, bool bResult)
{
	bSucceed = bResult;

	bExit = true;

	if(bSucceed == true)
	{
		USNMultiTestMatchingSceneBase::SetSessionName(SessionName);
	}
}
