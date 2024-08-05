// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAMotionMatchingDef.h"
#include "GAInterfactionTransform.h"
#include "BoneControllers/AnimNode_OffsetRootBone.h"
#include "BoneControllers/AnimNode_OrientationWarping.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNodeBase.h"
#include "PoseSearch/PoseSearchLibrary.h"
#include "GAMotionMatchingAnimInstance.generated.h"

class UChooserTable;
class UCharacterMovementComponent;
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

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable, Category="GA|Animation|MotionMatching", meta=(BlueprintThreadSafe))
	void SetReferences();
	
	UFUNCTION(BlueprintCallable, Category="GA|Animation|MotionMatching", meta=(BlueprintThreadSafe))
	void UpdateState();

	UFUNCTION(BlueprintCallable, Category="GA|Animation|MotionMatching", meta=(BlueprintThreadSafe))
	void UpdateEssentialValues();

	UFUNCTION(BlueprintCallable, Category="GA|Animation|MotionMatching", meta=(BlueprintThreadSafe))
	void GenerateTrajectory();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|MovementAnalysis", meta=(BlueprintThreadSafe))
	bool IsMoving() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GA|Animation|MotionMatching|MovementAnalysis", meta = (BlueprintThreadSafe))
	bool IsStarting() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GA|Animation|MotionMatching|MovementAnalysis", meta = (BlueprintThreadSafe))
	bool IsPivoting() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|MovementAnalysis", meta=(BlueprintThreadSafe))
	bool ShouldSpinTransition() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|MovementAnalysis", meta=(BlueprintThreadSafe))
	bool SholdTurnInPlace() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|MovementAnalysis", meta=(BlueprintThreadSafe))
	bool JustLandedLight() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|MovementAnalysis", meta=(BlueprintThreadSafe))
	bool JustLandedHeavy() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|MovementAnalysis", meta=(BlueprintThreadSafe))
	bool JustTraversed() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|Additive Lean", meta=(BlueprintThreadSafe))
	FVector CalculateRelativeAccelerationAmount() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|Additive Lean", meta=(BlueprintThreadSafe))
	FVector2D GetLeanAmount() const ;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|RootOffset", meta=(BlueprintThreadSafe))
	float GetOffsetRootTranslationHalfLife() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|RootOffset", meta=(BlueprintThreadSafe))
	EOffsetRootBoneMode GetOffsetRootTranslationMode() const ;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|RootOffset", meta=(BlueprintThreadSafe))
	EOffsetRootBoneMode GetOffsetRootRotationMode() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|RootOffset", meta=(BlueprintThreadSafe))
	EOrientationWarpingSpace GetOrientationWarpingSpace() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching", meta=(BlueprintThreadSafe))
	void UpdateMotionMatching(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching", meta=(BlueprintThreadSafe))
	void UpdateMotionMatchingPoseSelection(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching", meta=(BlueprintThreadSafe))
	float GetMotionMatchingBlendTime() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|Aim Offset", meta=(BlueprintThreadSafe))
	FVector2D GetAOValue() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|Aim Offset", meta=(BlueprintThreadSafe))
	bool IsEnableAO() const ;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching", meta=(BlueprintThreadSafe))
	EPoseSearchInterruptMode GetMotionMatchingInterruptMode() const ;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|Steering", meta=(BlueprintThreadSafe))
	bool EnableSteering() const ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GA|Animation|MotionMatching|Steering", meta=(BlueprintThreadSafe))
	FQuat GetTrajectoryFacing() const ;
	
	virtual void SetInteractTransform(const FTransform& InteractionTransform) override;

	virtual const FTransform& GetInteractTransform() const override;
	//UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	//void SetInteractTransform(const FTransform& Transform) override ;
	
	//UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	//const FTransform& GetInteractTransform() const override ;

	UPROPERTY(EditAnywhere, Category="GA|Animation Chooser")
	TObjectPtr<UChooserTable> AnimationSearchChooser = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Refarence")
	ACharacter* OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Refarence")
	UCharacterMovementComponent* OwnerMovementComponent = nullptr;
	
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

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "GA|States")
	bool IsClimb = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "GA|States")
	bool IsClimbLastFrame = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	FPoseSearchTrajectoryData TrajectoryGenerationDataIdle;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	FPoseSearchTrajectoryData TrajectoryGenerationDataMoving;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	FPoseSearchQueryTrajectory Trajectory;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	FPoseSearchTrajectory_WorldCollisionResults TrajectoryCollision;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	float PreviousDesiredControllerYaw = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Trajectory")
	FVector FutureVelocity = FVector::ZeroVector;
};
