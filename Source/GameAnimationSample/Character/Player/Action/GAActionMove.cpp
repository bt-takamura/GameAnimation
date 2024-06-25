// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GAActionMove.h"

#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"
#include "Kismet/KismetMathLibrary.h"

void UGAActionMove::ExecAction(const FInputActionValue& InputActionValue)
{
	AGAPlayer* Player(GetOwner<AGAPlayer>());

	if(Player == nullptr)
	{
		SNPLUGIN_LOG(TEXT("GAActionMove : GAPlayer is nullptr."));
		
		return;
	}

	FRotator Rotation(Player->GetControlRotation());

	FVector2D InputValue(Player->GetMovementInputScaleValue(FVector2D(InputActionValue[0], InputActionValue[1])));

	FVector RightVector(UKismetMathLibrary::GetRightVector(FRotator(0.0f, Rotation.Yaw, 0.0f)));

	Player->AddMovementInput(RightVector, InputValue.X);

	FVector ForwardVector(UKismetMathLibrary::GetForwardVector(FRotator(0.0f, Rotation.Yaw, 0.0f)));

	Player->AddMovementInput(ForwardVector, InputValue.Y);

	{
		bool FullMovementInput = false;
		
		float InputMag = InputValue.Length();

		if(InputMag >= Player->AnalogWalkRunThreshold)
		{
			FullMovementInput = true;
		} else
		{
			if((Player->MovementStickMode == EAnalogueMovementBehavior::FixedSpeed_SingleStride)
			||(Player->MovementStickMode == EAnalogueMovementBehavior::VariableSpeed_SingleStride)){
				FullMovementInput = true;
			}
		}

		if(Player->bWantsToSprint == true)
		{
			Player->Stride = EStride::Run;
		} else
		{
			if(Player->bWantsToWalk == true)
			{
				Player->Stride = EStride::Walk;
			} else
			{
				if(FullMovementInput == true)
				{
					Player->Stride = EStride::Run;
				} else
				{
					Player->Stride = EStride::Walk;
				}
			}
		}
	}
}
