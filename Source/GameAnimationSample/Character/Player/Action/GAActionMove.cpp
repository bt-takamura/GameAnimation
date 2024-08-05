// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Action/GAActionMove.h"

#include "SNDef.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameAnimationSample/Character/Player/Component/MMLocomotionComponent.h"

void UGAActionMove::ExecAction(const FInputActionValue& InputActionValue)
{

	ACharacter* Character = GetOwner<ACharacter>();
	if(Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("GAActionMove : Character is nullptr."));
		
		return;
	}
	UMMLocomotionComponent* MMLocomotionComponent = Character->GetComponentByClass<UMMLocomotionComponent>();
	if (MMLocomotionComponent == nullptr)
	{
		SNPLUGIN_LOG(TEXT("GAActionMove : MMLocomotionComponent is nullptr."));
		return;
	}

	FRotator Rotation(Character->GetControlRotation());

	FVector2D InputValue(MMLocomotionComponent->GetMovementInputScaleValue(FVector2D(InputActionValue[0], InputActionValue[1])));

	FVector RightVector(UKismetMathLibrary::GetRightVector(FRotator(0.0f, Rotation.Yaw, 0.0f)));

	Character->AddMovementInput(RightVector, InputValue.X);

	FVector ForwardVector(UKismetMathLibrary::GetForwardVector(FRotator(0.0f, Rotation.Yaw, 0.0f)));

	Character->AddMovementInput(ForwardVector, InputValue.Y);

	
	MMLocomotionComponent->SetupStride(InputValue);

}
