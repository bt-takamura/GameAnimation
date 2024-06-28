// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Animation/GAMotionMatchingAnimInstance.h"

#include "Chooser.h"
#include "ChooserFunctionLibrary.h"
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

bool UGAMotionMatchingAnimInstance::ShouldSpinTransition() const 
{
	FRotator DeltaRotator(UKismetMathLibrary::NormalizedDeltaRotator(CharacterTransform.Rotator(), RootTransform.Rotator()));
	
	if((FMath::Abs(DeltaRotator.Yaw) >= 130.0f)
	&& (Speed2D >= 150.0f)
	&& (CurrentDatabaseTags.Contains(TEXT("Pivots")))){
		return true;
	}
	
	return false;
}

bool UGAMotionMatchingAnimInstance::SholdTurnInPlace() const
{
	if(OwnerCharacter == nullptr)
	{
		SNPLUGIN_ERROR(TEXT("Owner Character is nullptr."));
		
		return false;
	}
	
	bool Result = false;
	// 止まった瞬間かチェック(MovementStateがIdleに変わった瞬間)
	bool bStopTrigger = ((MovementState == EMotionMatchingMovementState::Idle) && (MovementStateLastFrame == EMotionMatchingMovementState::Moving));
	
	if((bStopTrigger == true) || (OwnerCharacter->bWantsToAim == true)){
		
		FRotator DeltaRotator(UKismetMathLibrary::NormalizedDeltaRotator(CharacterTransform.Rotator(), RootTransform.Rotator()));
		
		if(FMath::Abs(DeltaRotator.Yaw) >= 50.0f){
			Result = true;
		}
	}
	
	return true;
}

bool UGAMotionMatchingAnimInstance::JustLandedLight() const
{
	if(OwnerCharacter == nullptr)
	{
		SNPLUGIN_ERROR(TEXT("Owner Character is nullptr."));
		
		return false;
	}
	
	if((OwnerCharacter->bJustLanded == true)
	&& (FMath::Abs(OwnerCharacter->LandSpeed.Z) < FMath::Abs(HeavyLandSpeedThreshold)))
	{
		return true;
	}

	return false;
}

bool UGAMotionMatchingAnimInstance::JustLandedHeavy() const
{
	if(OwnerCharacter == nullptr)
	{
		SNPLUGIN_ERROR(TEXT("Owner Character is nullptr."));
		
		return false;
	}
	
	if((OwnerCharacter->bJustLanded == true)
	&& (FMath::Abs(OwnerCharacter->LandSpeed.Z) >= FMath::Abs(HeavyLandSpeedThreshold)))
	{
		return true;
	}

	return false;
}

bool UGAMotionMatchingAnimInstance::JustTraversed() const
{
	if((IsSlotActive(FName(TEXT("DefaultSlot"))) != true)
	&& (GetCurveValue(FName(TEXT("MovingTraversal"))) > 0.0))
	{
		return true;
	}
	
	return false;
}

float UGAMotionMatchingAnimInstance::GetOffsetRootTranslationHalfLife() const
{
	if(MovementState == EMotionMatchingMovementState::Idle)
	{
		return 0.1f;
	} else
	{
		return 0.2f;
	}
}

//void UGAMotionMatchingAnimInstance::UpdateMotionMatching(FAnimationUpdateContext& Context, FAnimNodeReference& Node)
//{
//	
//}

float UGAMotionMatchingAnimInstance::GetMotionMatchingBlendTime() const
{
	float Result = 0.0f;
	
	return Result;
}


EPoseSearchInterruptMode UGAMotionMatchingAnimInstance::GetMotionMatchingInterruptMode() const
{
	bool bStateChanged = ((MovementState != MovementStateLastFrame)
		||(Stride != StrideLastFrame)
		||(Stance != StanceLastFrame));
	
	if((((bStateChanged == true) && (MovementMode == EMotionMatchingMovementMode::OnGround)))
		|| (MovementMode != MovementModeLastFrame))
	{
		return EPoseSearchInterruptMode::InterruptOnDatabaseChange;
	} else
	{
		return EPoseSearchInterruptMode::DoNotInterrupt;
	}
	
}

bool UGAMotionMatchingAnimInstance::EnableSteering() const
{
	return ((MovementState == EMotionMatchingMovementState::Moving) || (MovementMode  == EMotionMatchingMovementMode::InAir));
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


