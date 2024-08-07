// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GAResetClimbMoveAction.h"

#include "SNDef.h"
#include "GameFramework/Character.h"
#include "GameAnimationSample/Character/Player/Component/ClimbActionComponent.h"

void UGAResetClimbMoveAction::ExecAction(const FInputActionValue& InputActionValue)
{
	ACharacter* Character = GetOwner<ACharacter>();
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));

		return;
	}
	UClimbActionComponent* ClimbActionComponent = Character->GetComponentByClass<UClimbActionComponent>();
	if (ClimbActionComponent == nullptr)
	{
		SNPLUGIN_LOG(TEXT("ClimbActionComponent is nullptr."));

		return;
	}

	if (ClimbActionComponent->IsClimbing() == true)
	{
		//! EMovementMode::MOVE_FallingのためVelocityをリセットする
		ClimbActionComponent->ResetVelocity();
	}
}
