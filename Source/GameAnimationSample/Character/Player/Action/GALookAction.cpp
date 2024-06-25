// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GALookAction.h"

#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"

void UGALookAction::ExecAction(const FInputActionValue& InputActionValue)
{
	Super::ExecAction(InputActionValue);

	AGAPlayer* Player(GetOwner<AGAPlayer>());

	if(Player == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Player is nullptr."));

		return;
	}

	Player->AddControllerYawInput(InputActionValue[0]);

	Player->AddControllerPitchInput(InputActionValue[1]);
}
