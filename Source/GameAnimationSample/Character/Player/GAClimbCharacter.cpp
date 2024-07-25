// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/GAClimbCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "GameAnimationSample/Character/Animation/GAClimbTransform.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AGAClimbCharacter::AGAClimbCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ClimbActionComponent = CreateDefaultSubobject<UClimbActionComponent>("ClimbActionComp");
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

//----------------------------------------------------------------------//
//
//! @brief Climb開始時の掴まる姿勢の設定
//
//! @param HitResult	TryAction時の衝突判定の結果
//! @param OutTransform 設定する掴まる姿勢情報
//
//----------------------------------------------------------------------//
void AGAClimbCharacter::SetClimbGraspTransform(FHitResult& HitResult, FTransform& OutTransform)
{
	//! 衝突位置から腕が埋まらないよう調整(AdjustValue)
	FVector Location = HitResult.ImpactPoint;
	Location = Location;
	Location.Z = GetActorLocation().Z + ClimbHeight;
	FRotator Rotator = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal * FVector(-1.0f, -1.0f, 0.0f));
	OutTransform.SetRotation(Rotator.Quaternion());
	OutTransform.SetLocation(Location);
}

//----------------------------------------------------------------------//
//
//! @brief Climb中の移動処理
//! 
//! @param InputValue	コントローラー3Dスティック入力値
//
//----------------------------------------------------------------------//
void AGAClimbCharacter::ClimbMove(const FVector2D& InputValue)
{
	//! 左右移動
	FVector RightVector = GetActorRightVector();
	AddMovementInput(RightVector, InputValue.X);
	//! 上下移動
	FVector UpVector = GetActorUpVector();
	AddMovementInput(UpVector, InputValue.Y);
}
