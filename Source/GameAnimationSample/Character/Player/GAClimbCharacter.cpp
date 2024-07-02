// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/GAClimbCharacter.h"

// Sets default values
AGAClimbCharacter::AGAClimbCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGAClimbCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGAClimbCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGAClimbCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

