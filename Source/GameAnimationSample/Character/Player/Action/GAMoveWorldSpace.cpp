// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GAMoveWorldSpace.h"

#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"

void UGAMoveWorldSpace::ExecAction(const FInputActionValue& InputActionValue)
{
	Super::ExecAction(InputActionValue);

	AGAPlayer* Player(GetOwner<AGAPlayer>());

	if(Player == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Player is nullptr."));

		return;
	}
	
	FVector2D Vector(InputActionValue[0], InputActionValue[1]);

	Vector.Normalize();

	Player->AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Vector.X);

	Player->AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Vector.Y);

	{
		bool FullMovementInput = false;
		
		float InputMag = Vector.Length();

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
