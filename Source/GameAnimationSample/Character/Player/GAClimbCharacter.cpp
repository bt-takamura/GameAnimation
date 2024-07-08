// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/GAClimbCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "GameAnimationSample/Character/Animation/GAClimbTransform.h"

// Sets default values
AGAClimbCharacter::AGAClimbCharacter()
:IsClimb(false)
,TargetRotator(0.0f,0.0f,0.0f)
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
	if (IsClimb == true)
	{
		//! Climb中はアクションを辞めて降りる
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
		//! 障害物が見つからない→Jumpを実施
		return false;
	}

	Result = ClimbAction(MotionWarping);
	if (Result == false)
	{
		//! Climbアクション失敗
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
	//! 衝突したらターゲット情報を取得
	if (Result == false)
	{
		return false;
	}

	//! 掴まる位置のTransfromを設定
	TargetRotator = UKismetMathLibrary::MakeRotFromZ(HitResult.Normal);
	ClimbTransform.SetRotation(TargetRotator.Quaternion());
	FVector Upward(0.0f, 0.0f, Height);
	ClimbTransform.SetLocation(GetActorLocation() + (GetActorForwardVector() + Upward)/* * 2.0f*/);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}
	if (AnimInstance->GetClass()->ImplementsInterface(UGAClimbTransform::StaticClass()))
	{
		IGAClimbTransform::Execute_SetClimbTransform(AnimInstance, ClimbTransform);
	}

	return Result;
}

bool AGAClimbCharacter::ClimbAction(UMotionWarpingComponent* MotionWarping)
{
	//! MotionWarpingで移動
	if (MotionWarping == nullptr)
	{
		return false;
	}
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(TargetName,ClimbTransform.GetLocation(), ClimbTransform.GetRotation().Rotator());
	
	IsClimb = true;
	return true;
}
