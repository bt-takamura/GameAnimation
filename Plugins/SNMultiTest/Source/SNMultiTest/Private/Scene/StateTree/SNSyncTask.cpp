// Fill out your copyright notice in the Description page of Project Settings.


#include "Scene/StateTree/SNSyncTask.h"

#include "SNDef.h"
#include "Utility/SNUtility.h"
#include "Character/SNPlayerController.h"
#include "UI/Widget/SNMatchingSyncMenu.h"
#include "UI/Widget/SNButton.h"

EStateTreeRunStatus USNSyncTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	EStateTreeRunStatus Result = Super::Tick(Context, DeltaTime);
#if 0
	if(SNUtility::IsServer(GetWorld()))
	{
		ACWGameState* GameState(Cast<ACWGameState>(GetWorld()->GetGameState()));

		if(GameState != nullptr)
		{
			if(GameState->IsReadyToBattle() == true)
			{
				if(BattleMap.IsNull() == false)
				{
					SNUtility::Travel(BattleMap.GetAssetName());
					//GetWorld()->ServerTravel(*BattleMap.GetAssetName(), true);
					//UGameplayStatics::OpenLevel(GetWorld(), *BattleMap.GetAssetName(), true, "listen");
				}
				FinishTask();

				return EStateTreeRunStatus::Succeeded;
			}
		}
	}
#endif
	return Result;
}

EStateTreeRunStatus USNSyncTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	EStateTreeRunStatus Result = Super::EnterState(Context, Transition);

	ASNPlayerController* PlayerController(SNUtility::GetPlayerController<ASNPlayerController>());

	SNPLUGIN_ASSERT(PlayerController != nullptr, TEXT("PlayerController is nullptr."));
	
	PlayerController->EnabledInputType(FName(TEXT("Player")), true);
	
	return Result;
}

void USNSyncTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);

}

void USNSyncTask::HudPostLoad()
{
	USNMatchingSyncMenu* Menu = GetHudWidget<USNMatchingSyncMenu>();

	if(Menu != nullptr)
	{
		Menu->SetSessionName(USNMultiTestMatchingSceneBase::GetSessionName());

		if(USNButton* Button = Menu->GetStartButton())
		{
			Button->OnClickedDelegate.AddDynamic(this, &USNSyncTask::OnCreateButtonClicked);
		}

		SetVisibleWidget(EWidgetLayer::Layer3, Menu);
	}
}

void USNSyncTask::OnCreateButtonClicked(USNButton* Button)
{
	APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());

	SNPLUGIN_ASSERT(PlayerController != nullptr, TEXT("PlayerController is nullptr."));

	SNPLUGIN_ASSERT(PlayerController->IsLocalController() == true, TEXT("PlayerController is not local."));
#if 0
	
	ACWPlayerState* PlayerState(PlayerController->GetPlayerState<ACWPlayerState>());

	if(PlayerState != nullptr)
	{
		PlayerState->SetReadyToBattle(true);
		
		SNPLUGIN_LOG(TEXT("Ready To Battle On."));
	} else
	{
		SNPLUGIN_LOG(TEXT("PlayerState is nullptr."));
	}
	if(BattleMap.IsNull() == false)
	{
		//UGameplayStatics::OpenLevel(GetWorld(), *BattleMap.GetAssetName(), true, "listen");
		
	}	
#endif
}
