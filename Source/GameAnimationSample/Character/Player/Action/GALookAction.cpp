// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GALookAction.h"

#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"

void UGALookAction::ExecAction(const FInputActionValue& InputActionValue)
{
	Super::ExecAction(InputActionValue);

	ACharacter* Character = GetOwner<ACharacter>();
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));

		return;
	}

	Character->AddControllerYawInput(InputActionValue[0]);

	Character->AddControllerPitchInput(InputActionValue[1]);
}
