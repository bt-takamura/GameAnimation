// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GAAimAction.h"

#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"
#include "GameAnimationSample/Character/Player/Component/MMLocomotionComponent.h"

void UGAAimAction::ExecAction(const FInputActionValue& InputActionValue)
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
		SNPLUGIN_LOG(TEXT("MMLocomotionComponent is nullptr."));
		return;
	}

	MMLocomotionComponent->SetWantsToAim(InputActionValue.Get<bool>());

	if (MMLocomotionComponent->GetWantsToAim() == true)
	{
		MMLocomotionComponent->SetWantsToStrafe(true);
	}
}
