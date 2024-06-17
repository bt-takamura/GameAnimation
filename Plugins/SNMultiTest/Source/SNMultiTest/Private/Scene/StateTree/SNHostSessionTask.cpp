// Fill out your copyright notice in the Description page of Project Settings.


#include "Scene/StateTree/SNHostSessionTask.h"

#include "Utility/SNUtility.h"
#include "Online/SNOnlineSystem.h"
#include "UI/Widget/SNMatchingHostSessionMenu.h"
#include "UI/Widget/SNButton.h"


EStateTreeRunStatus USNHostSessionTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	EStateTreeRunStatus Result = Super::Tick(Context, DeltaTime);

	if(bCancel == true)
	{
		FinishTask();

		return EStateTreeRunStatus::Failed;
	}

	if(bExit == true)
	{
		FinishTask();

		return (bSucceed == true) ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
	}

	return Result;
}

EStateTreeRunStatus USNHostSessionTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	EStateTreeRunStatus Result = Super::EnterState(Context, Transition);

	return Result;
}

void USNHostSessionTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);

	USNOnlineSystem* OnlineSystem(SNUtility::GetOnlineSystem<USNOnlineSystem>());

	if(OnlineSystem != nullptr)
	{
		OnlineSystem->OnCompleteHostSession.Clear();
	}

	if(LobbyMap.IsNull() == false)
	{
		UGameplayStatics::OpenLevel(GetWorld(), *LobbyMap.GetAssetName(), true, TEXT("listen"));
	}
}

void USNHostSessionTask::HudPostLoad()
{
	Super::HudPostLoad();

	USNMatchingHostSessionMenu* Menu = GetHudWidget<USNMatchingHostSessionMenu>();

	if(Menu != nullptr)
	{
		if(USNButton* Button = Menu->GetCreateButton())
		{
			Button->OnClickedDelegate.AddDynamic(this, &USNHostSessionTask::OnCreateButtonClicked);
		}

		if(USNButton* Button = Menu->GetCancelButton())
		{
			Button->OnClickedDelegate.AddDynamic(this, &USNHostSessionTask::OnCancelButtonClicked);
		}

		SetVisibleWidget(EWidgetLayer::Layer3, Menu);
	}
}

void USNHostSessionTask::OnCreateButtonClicked(USNButton* Button)
{
	USNOnlineSystem* OnlineSystem(SNUtility::GetOnlineSystem<USNOnlineSystem>());

	if(OnlineSystem != nullptr)
	{
		OnlineSystem->OnCompleteHostSession.AddDynamic(this, &USNHostSessionTask::OnCompleteHostSession);

		FName Name(TEXT("Session"));

		int ConnectionNum = -1;

		if(USNMatchingHostSessionMenu* Menu = GetHudWidget<USNMatchingHostSessionMenu>())
		{
			Name = Menu->GetSessionName();

			ConnectionNum = Menu->GetConnectionNum();
		}

		if(ConnectionNum > 0)
		{
			OnlineSystem->HostSession(ConnectionNum, Name);
		}
	}
}

void USNHostSessionTask::OnCancelButtonClicked(USNButton* Button)
{
	bCancel = true;
}


void USNHostSessionTask::OnCompleteHostSession(FName SessionName, bool bWasSuccessful)
{
	bSucceed = bWasSuccessful;

	bExit = true;

	if(bWasSuccessful == true)
	{
		USNMultiTestMatchingSceneBase::SetSessionName(SessionName);
	}

	USNOnlineSystem* OnlineSystem(SNUtility::GetOnlineSystem<USNOnlineSystem>());

	if(OnlineSystem != nullptr)
	{
		OnlineSystem->OnCompleteHostSession.Clear();
	}
}




