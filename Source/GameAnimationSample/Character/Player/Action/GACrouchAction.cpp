// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GACrouchAction.h"

#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"

void UGACrouchAction::ExecAction(const FInputActionValue& InputActionValue)
{
	Super::ExecAction(InputActionValue);

	AGAPlayer* Player(GetOwner<AGAPlayer>());

	if(Player == nullptr)
	{
		SNPLUGIN_LOG(TEXT("CrouchAction : Player is nullptr."));

		return;
	}

	if(Player->bIsCrouched == true)
	{
		Player->UnCrouch();
	} else
	{
		Player->Crouch();
	}
}
