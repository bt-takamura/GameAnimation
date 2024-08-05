// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GAWalkAction.h"

#include "SNDef.h"
#include "GameFramework/Character.h"
#include "GameAnimationSample/Character/Player/Component/MMLocomotionComponent.h"

void UGAWalkAction::ExecAction(const FInputActionValue& InputActionValue)
{
	Super::ExecAction(InputActionValue);

	ACharacter* Character = GetOwner<ACharacter>();
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));
		return;
	}
	UMMLocomotionComponent* MMLocomotionComponent = Character->GetComponentByClass<UMMLocomotionComponent>();
	if (MMLocomotionComponent == nullptr)
	{
		SNPLUGIN_LOG(TEXT("MMLocomotion is nullptr."));
		return;
	}

	if (MMLocomotionComponent->GetWantsToSprint() != true)
	{
		bool Flag = (MMLocomotionComponent->GetWantsToWalk() != true);
		MMLocomotionComponent->SetWantsToWalk(Flag);
	}
}
