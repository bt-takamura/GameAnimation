// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Animation/GAMotionMatchingAnimInstance.h"

#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PoseSearch/PoseSearchTrajectoryLibrary.h"

UGAMotionMatchingAnimInstance::UGAMotionMatchingAnimInstance()
:Super()
,TrajectoryGenerationDataIdle(FPoseSearchTrajectoryData::StaticStruct())
,TrajectoryGenerationDataMoving(FPoseSearchTrajectoryData::StaticStruct())
,Trajectory(FPoseSearchQueryTrajectory::StaticStruct())
,TrajectoryCollision(FPoseSearchTrajectory_WorldCollisionResults::StaticStruct())
{
	
}

void UGAMotionMatchingAnimInstance::SetInteractTransform(const FTransform& Transform)
{
	InteractionTransform = Transform;
}

const FTransform& UGAMotionMatchingAnimInstance::GetInteractTransform() const
{
	return InteractionTransform;
}

bool UGAMotionMatchingAnimInstance::ShouldSpinTransition()
{
	FRotator DeltaRotator(UKismetMathLibrary::NormalizedDeltaRotator(CharacterTransform.Rotator(), RootTransform.Rotator()));

	bool Result = false;

	if((MovementState == EMotionMatchingMovementState::Idle)
	&& (MovementStateLastFrame == EMotionMatchingMovementState::Moving))
	{
		if(OwnerCharacter->bWantsToAim)
		{
			if(DeltaRotator.Yaw >= 50.0f)
			{
				Result = true;
			}
		}
	}

	return Result;
}

void UGAMotionMatchingAnimInstance::UpdateState()
{
	if((OwnerCharacter == nullptr) || (OwnerMovementComponent == nullptr))
	{
		SNPLUGIN_ERROR(TEXT("Owner Character is nullptr."));

		return;
	}

	// キャラクタのGaitをキャッシュし、最後のフレームの値を保存します。Gait Enumは、現在の速度ではなく、入力から意図された運動スタイルを記述します。
	// 例えば、キャラクタはゆっくり動いていても、Run Gait Enumの状態である可能性があります。
	StrideLastFrame = Stride;

	Stride = OwnerCharacter->Stride;
	
	// キャラクタのスタンスをキャッシュし、最後のフレームの値を保存します。Stance Enumは、キャラクタの移動コンポーネントがしゃがみモードにあるかどうかを表します。
	StanceLastFrame = Stance;

	if(OwnerMovementComponent->IsCrouching() == true)
	{
		Stance = EMotionMatchingStance::Crouch;
	} else
	{
		Stance = EMotionMatchingStance::Stand;
	}
}


