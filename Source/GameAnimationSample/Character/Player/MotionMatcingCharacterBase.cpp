// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/MotionMatcingCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameAnimationSample/Character/Player/Component/MMLocomotionComponent.h"

// Sets default values
AMotionMatcingCharacterBase::AMotionMatcingCharacterBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	MMLocomotionComponent = CreateDefaultSubobject<UMMLocomotionComponent>(TEXT("MotionMatchingLocomotionComponent"));

}

// Called when the game starts or when spawned
void AMotionMatcingCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMotionMatcingCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMotionMatcingCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

