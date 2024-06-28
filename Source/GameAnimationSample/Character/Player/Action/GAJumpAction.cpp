// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GAJumpAction.h"

#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGAJumpAction::ExecAction(const FInputActionValue& InputActionValue)
{
	Super::ExecAction(InputActionValue);

	AGAPlayer* Player(GetOwner<AGAPlayer>());

	if(Player == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Player is nullptr."));

		return;
	}

	if(Player->GetEnableTraversalAction() != true)
	{
		if(Player->GetCharacterMovement()->IsMovingOnGround() == true)
		{
			float FowardTraceDistance = Player->GetTraversalForwardTraceDistance();

			bool TraversalCheckFailed = false;
			bool MontageSelectionFailed = false;
			
			Player->ExecTraversalAction(FowardTraceDistance, TraversalCheckFailed, MontageSelectionFailed);

			if(TraversalCheckFailed == true)
			{
				Player->Jump();
			}

			if(MontageSelectionFailed == true)
			{
				SNPLUGIN_LOG(TEXT("Traversal Montagen can't find."));
			}
		}
	}
}
