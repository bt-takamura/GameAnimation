// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAMotionMatchingDef.h"
#include "GAInterfactionTransform.h"
#include "InstancedStruct.h"
#include "Animation/AnimInstance.h"
#include "GAMotionMatchingAnimInstance.generated.h"

struct FPoseSearchTrajectory_WorldCollisionResults;
struct FPoseSearchQueryTrajectory;
struct FPoseSearchTrajectoryData;
class UPoseSearchDatabase;
/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UGAMotionMatchingAnimInstance : public UAnimInstance, public IGAInterfactionTransform
{
	GENERATED_BODY()

public:

	UGAMotionMatchingAnimInstance();
	
	virtual void SetInteractionTransform(const FTransform& Transform) override;

	virtual const FTransform& GetInteractionTransform() const override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	FTransform CharacterTransform = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	FTransform RootTransform = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	bool HasAcceleration = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	FVector Acceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	float AccelerationAmount = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	bool HasVelocity = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	FVector VelocityLastFrame = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	FVector VelocityAcceleration = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	FVector LastNonZeroVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	float Speed2D = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	int DatabaseLOD = 0;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	TObjectPtr<UPoseSearchDatabase> CurrentSelectDatabase = nullptr;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	TArray<FName> CurrentDatabaseTags;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	float HeavyLandSpeedThreshold = 700.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	FTransform InteractionTransform = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|EssentialValues")
	bool OffsetRootBoneEnable = false;;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|States")
	TEnumAsByte<EMotionMatchingMovementMode> MovementMode = EMotionMatchingMovementMode::OnGround;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|States")
	TEnumAsByte<EMotionMatchingMovementMode> MovementModeLastFrame = EMotionMatchingMovementMode::OnGround;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|States")
	TEnumAsByte<EMotionMatchingRotationMode> RotationMode = EMotionMatchingRotationMode::OrientToMovement;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|States")
	TEnumAsByte<EMotionMatchingRotationMode> RotationModeLastFrame = EMotionMatchingRotationMode::OrientToMovement;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|States")
	TEnumAsByte<EMotionMatchingMovementState> MovementState = EMotionMatchingMovementState::Idle;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|States")
	TEnumAsByte<EMotionMatchingMovementState> MovementStateLastFrame = EMotionMatchingMovementState::Idle;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|States")
	TEnumAsByte<EStride> Stride = EStride::Walk;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|States")
	TEnumAsByte<EStride> StrideLastFrame = EStride::Walk;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|States")
	TEnumAsByte<EMotionMatchingStance> Stance = EMotionMatchingStance::Stand;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|States")
	TEnumAsByte<EMotionMatchingStance> StanceLastFrame = EMotionMatchingStance::Stand;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	FInstancedStruct TrajectoryGenerationDataIdle;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	FInstancedStruct TrajectoryGenerationDataMoving;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	FInstancedStruct Trajectory;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	FInstancedStruct TrajectoryCollision;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	float PreviousDesiredControllerYaw = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	FVector FutureVelocity = FVector::ZeroVector;
};
