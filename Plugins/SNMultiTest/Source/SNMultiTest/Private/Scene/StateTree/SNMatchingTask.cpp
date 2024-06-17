// Fill out your copyright notice in the Description page of Project Settings.


#include "Scene/StateTree/SNMatchingTask.h"

#include "UI/Widget/SNMatchingMenu.h"
#include "UI/Widget/SNButton.h"

EStateTreeRunStatus USNMatchingTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	EStateTreeRunStatus Result = Super::Tick(Context, DeltaTime);

	if(MatchingSelection != EMatchingSelection::None)
	{
		FinishTask();

		return EStateTreeRunStatus::Succeeded;
	}

	return Result;
}

EStateTreeRunStatus USNMatchingTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	EStateTreeRunStatus Result = Super::EnterState(Context, Transition);

	return Result;
}

void USNMatchingTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
}

void USNMatchingTask::HudPostLoad()
{
	Super::HudPostLoad();

	USNMatchingMenu* Menu = GetHudWidget<USNMatchingMenu>();

	if(Menu != nullptr)
	{
		if(USNButton* Button = Menu->GetHostSessionButton())
		{
			Button->OnClickedDelegate.AddDynamic(this, &USNMatchingTask::OnClickHostSession);
		}

		if(USNButton* Button = Menu->GetJoinSessionButton())
		{
			Button->OnClickedDelegate.AddDynamic(this, &USNMatchingTask::OnClickJoinSession);
		}

		SetVisibleWidget(EWidgetLayer::Layer3, Menu);
	}
}

void USNMatchingTask::OnClickHostSession(USNButton* Button)
{
	MatchingSelection = EMatchingSelection::Host;
}

void USNMatchingTask::OnClickJoinSession(USNButton* Button)
{
	MatchingSelection = EMatchingSelection::Join;
}

