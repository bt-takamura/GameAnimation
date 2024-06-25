// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GALookGamepadAction.h"

#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"
#include "Kismet/GameplayStatics.h"

void UGALookGamepadAction::ExecAction(const FInputActionValue& InputActionValue)
{
	Super::ExecAction(InputActionValue);

	AGAPlayer* Player(GetOwner<AGAPlayer>());

	if(Player == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Player is nullptr."));

		return;
	}

	FVector2D Vector(InputActionValue[0], InputActionValue[1]);

	Vector *= UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	Player->AddControllerYawInput(Vector.X);

	Player->AddControllerPitchInput(Vector.Y);
}
