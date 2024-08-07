// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GALookGamepadAction.h"

#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"
#include "Kismet/GameplayStatics.h"

void UGALookGamepadAction::ExecAction(const FInputActionValue& InputActionValue)
{
	Super::ExecAction(InputActionValue);

	ACharacter* Character = GetOwner<ACharacter>();
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));

		return;
	}

	FVector2D Vector(InputActionValue[0], InputActionValue[1]);

	Vector *= UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	Character->AddControllerYawInput(Vector.X);

	Character->AddControllerPitchInput(Vector.Y);
}
