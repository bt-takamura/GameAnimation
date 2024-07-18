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
,ClimbTransform()
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

bool AGAClimbCharacter::TryClimbAction(UMotionWarpingComponent* MotionWarping)
{
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

bool AGAClimbCharacter::CanselClimb()
{
	if (IsClimb == false)
	{
		return false;
	}

	//! Climbのフラグを下ろし、MovementModeをFallingにする
	IsClimb = false;
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp != nullptr)
	{
		MovementComp->SetMovementMode(EMovementMode::MOVE_Falling);
		MovementComp->GravityScale = 1.0f;
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

	ClimbWallNormal = HitResult.ImpactNormal;

	//! 掴まる姿勢を設定
	//! 衝突位置から腕が埋まらない程度で手前にする(AdjustValue)→後々IKで設定
	FVector Location = HitResult.ImpactPoint;
	Location = Location + (HitResult.ImpactNormal * AdjustValue);
	Location.Z = Height;
	ClimbTransform.SetLocation(Location);
	FRotator Rotator = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal * FVector(-1.0f, -1.0f, 0.0f));
	ClimbTransform.SetRotation(Rotator.Quaternion());

	return Result;
}

bool AGAClimbCharacter::ClimbAction(UMotionWarpingComponent* MotionWarping)
{
	//! 重力に反した動作のためMovementModeをFlyingに設定
	//! 動作的にはMovementModeのみでいいがTrajectoryのためにGravityScaleを0.fにする
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp == nullptr)
	{
		UE_LOG(Climb, Warning, TEXT("MovementComponent is nullptr."));
		return false;
	}
	MovementComp->SetMovementMode(EMovementMode::MOVE_Flying);
	MovementComp->GravityScale = 0.0f;
	
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
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(TargetName, ClimbTransform.GetLocation(), ClimbTransform.Rotator());
	
	IsClimb = true;
	return true;
}
