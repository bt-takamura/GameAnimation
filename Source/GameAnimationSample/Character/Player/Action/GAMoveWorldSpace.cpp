// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GAMoveWorldSpace.h"

#include "SNDef.h"
#include "GameFramework/Character.h"
#include "GameAnimationSample/Character/Player/Component/MMLocomotionComponent.h"

void UGAMoveWorldSpace::ExecAction(const FInputActionValue& InputActionValue)
{
	Super::ExecAction(InputActionValue);

	ACharacter* Character = GetOwner<ACharacter>();
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("GAMoveWorldSpace : Character is nullptr."));

		return;
	}

	UMMLocomotionComponent* MMLocomotionComponent = Character->GetComponentByClass<UMMLocomotionComponent>();
	if (MMLocomotionComponent == nullptr)
	{
		SNPLUGIN_LOG(TEXT("GAMoveWorldSpace : MMLocomotionComponent is nullptr."));
		return;
	}
	FVector2D Vector(InputActionValue[0], InputActionValue[1]);

	Vector.Normalize();

	Character->AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Vector.X);

	Character->AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Vector.Y);

	MMLocomotionComponent->SetupStride(Vector);

}
