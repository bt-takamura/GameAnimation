// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/GAClimbCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "GameAnimationSample/Character/Animation/GAClimbTransform.h"

#include "Kismet/KismetSystemLibrary.h"
DEFINE_LOG_CATEGORY(Climb);


// Sets default values
AGAClimbCharacter::AGAClimbCharacter()
:IsClimb(false)
,ClimbLocation(0.0f, 0.0f, 0.0f)
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
	//! 掴まり動作のMotageの再生が終わったら、MovementModeをWalkingに戻す
	if (IsClimb == true)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			if (AnimInstance->Montage_IsPlaying(ClimbAnimMontage) == false)
			{
				UCharacterMovementComponent* MovementComp = GetCharacterMovement();
				if (MovementComp != nullptr)
				{
					MovementComp->SetMovementMode(EMovementMode::MOVE_Walking);
				}
			}
		}
	}

	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGAClimbCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AGAClimbCharacter::TryClimbAction(UMotionWarpingComponent* MotionWarping)
{
	if (IsClimb == true)
	{
		//! Climbのフラグを下ろし、MovementModeをFallingにする
		IsClimb = false;
		UCharacterMovementComponent* MovementComp = GetCharacterMovement();
		if (MovementComp != nullptr)
		{
			MovementComp->SetMovementMode(EMovementMode::MOVE_Falling);
		}
		return true;
	}

	bool Result = FindObjectInFront();
	if (Result == false)
	{
		return false;
	}

	Result = ClimbAction(MotionWarping);
	if (Result == false)
	{
		return false;
	}

	return true;
}

bool AGAClimbCharacter::FindObjectInFront()
{
	//! 今のアクターの座標(Start)からアクターの前方方向ベクトル * 判定距離(End)までトレース
	FVector ActorLoccation = GetActorLocation();
	FVector ActorForwardVec = GetActorForwardVector();

	TArray<AActor*> IgnoreActors;
	FHitResult HitResult = {};
	bool Result = UKismetSystemLibrary::CapsuleTraceSingle(
		this, GetActorLocation(), ActorLoccation + (ActorForwardVec * TraceDistance/*判定距離*/),
		30.0f/*CapsuleRadius*/, 60.0f/*HalfHeight*/,
		TraceChannel, false, IgnoreActors,
		EDrawDebugTrace::ForDuration,
		HitResult, true
	);
	if (Result == false)
	{
		return false;
	}

	//! 掴まる位置のLocationを設定、衝突位置から腕が埋まらない程度で手前にする(AdjustValue)
	ClimbLocation = HitResult.ImpactPoint;
	ClimbLocation = ClimbLocation + (HitResult.ImpactNormal * AdjustValue);
	ClimbLocation.Z = Height;

	return Result;
}

bool AGAClimbCharacter::ClimbAction(UMotionWarpingComponent* MotionWarping)
{
	//! 重力に反した動作のためMovementModeをFlyingに設定
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp == nullptr)
	{
		UE_LOG(Climb, Warning, TEXT("MovementComponent is nullptr."));
		return false;
	}
	MovementComp->SetMovementMode(EMovementMode::MOVE_Flying);
	
	//! 掴まり動作のMontageを再生
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}
	if (ClimbAnimMontage == nullptr)
	{
		UE_LOG(Climb, Warning, TEXT("ClimbAnimMontage is nullptr."));
		return false;
	}
	AnimInstance->Montage_Play(ClimbAnimMontage);

	//! MotionWarpingで移動
	if (MotionWarping == nullptr)
	{
		UE_LOG(Climb, Warning, TEXT("MotionWarping is nullptr."));
		return false;
	}
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(TargetName, ClimbLocation, GetActorRotation());
	
	IsClimb = true;
	return true;
}
