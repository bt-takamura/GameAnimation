// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Components/SNLocomotionComponent.h"

#include "AITypes.h"
#include "SNDef.h"
#include "Character/Base/SNCharacterBase.h"
#include "Curves/CurveVector.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
USNLocomotionComponent::USNLocomotionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// Tickは止めておく
	SetComponentTickEnabled(false);
	// ...
}


// Called when the game starts
void USNLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();

	MaxSpeed.Sort([](const float& A, const float& B){ return (A<B)?true:false;});
	// ...
	
}


// Called every frame
void USNLocomotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SNPLUGIN_LOG(TEXT("TickComponent is move."));
	// ...
}

void USNLocomotionComponent::MoveGround(const FVector2D& Vector)
{
	ASNCharacterBase* Character = Cast<ASNCharacterBase>(GetOwner());

	if(Character == nullptr)
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent(Cast<UCharacterMovementComponent>(Character->GetMovementComponent()));

	if(MovementComponent == nullptr)
	{
		return;
	}

	float Mag = Vector.Length();

	float TmpSpeed = -1.0f, AccelRange = 0.0f , OldSpeed = 0.0f, CheckRange = 0.0f;

	for(float Speed : MaxSpeed)
	{
		if(Mag <= Speed)
		{
			AccelRange = UKismetMathLibrary::MapRangeClamped(Mag, OldSpeed, Speed, CheckRange, CheckRange+1.0f);
			
			TmpSpeed = Speed;
			
			break;
		}

		OldSpeed = Speed;

		CheckRange = CheckRange + 1.0f;
	}
	
	if(TmpSpeed < 0.0f)
	{
		TmpSpeed = (MaxSpeed.Num() > 0 ? MaxSpeed[MaxSpeed.Num()-1] : 300.0f);
	}

	MovementComponent->MaxWalkSpeed = TmpSpeed;

	//SNPLUGIN_LOG(TEXT("Speed %f / Mag %f"), MovementComponent->MaxWalkSpeed, Mag);

	FRotator ControlRotation(Character->GetControlRotation());

	FVector FrontRear = FRotationMatrix(FRotator(0.0, ControlRotation.Yaw, 0.0)).GetUnitAxis(EAxis::X);

	Character->AddMovementInput(FrontRear, Vector.Y);

	FVector RightLeft = FRotationMatrix(FRotator(0.0, ControlRotation.Yaw, 0.0)).GetUnitAxis(EAxis::Y);

	Character->AddMovementInput(RightLeft, Vector.X);

	if(MovementCurve != nullptr)
	{
		FVector Param = MovementCurve->GetVectorValue(AccelRange);

		MovementComponent->MaxAcceleration = Param.X;
		MovementComponent->BrakingDecelerationWalking = Param.Y;
		MovementComponent->GroundFriction = Param.Z;
	}
}

float USNLocomotionComponent::GetMaxSpeed() const
{
//	float MaxSpeed = FMath::Max(MoveSpeed.RunSpeed, MoveSpeed.WalkSpeed);

//	MaxSpeed = FMath::Max(MaxSpeed, MoveSpeed.SprintSpeed);

	return (MaxSpeed.Num() > 0) ? MaxSpeed[MaxSpeed.Num()-1] : 300.0f;
}




