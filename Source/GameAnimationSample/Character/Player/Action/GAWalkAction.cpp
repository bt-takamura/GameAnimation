// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GAWalkAction.h"

#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"

void UGAWalkAction::ExecAction(const FInputActionValue& InputActionValue)
{
	Super::ExecAction(InputActionValue);

	AGAPlayer* Player(GetOwner<AGAPlayer>());

	if(Player == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Player is nullptr."));

		return;
	}

	if(Player->bWantsToSprint != true)
	{
		Player->bWantsToWalk = (Player->bWantsToWalk != true);
	}
}
