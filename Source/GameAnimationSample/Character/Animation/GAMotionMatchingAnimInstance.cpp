// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Animation/GAMotionMatchingAnimInstance.h"

#include "Chooser.h"
#include "ChooserFunctionLibrary.h"
#include "SNDef.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PoseSearch/PoseSearchTrajectoryLibrary.h"
#include "PoseSearch/PoseSearchDatabase.h"
#include "PoseSearch/MotionMatchingAnimNodeLibrary.h"

UGAMotionMatchingAnimInstance::UGAMotionMatchingAnimInstance()
:Super()
{
	
}

void UGAMotionMatchingAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SetReferences();
}

void UGAMotionMatchingAnimInstance::SetReferences()
{
	AGAPlayer* Player(Cast<AGAPlayer>(TryGetPawnOwner()));

	if(Player != nullptr)
	{
		OwnerCharacter = Player;

		OwnerMovementComponent = Cast<UCharacterMovementComponent>(Player->GetMovementComponent());
	} else
	{
		SNPLUGIN_ERROR(TEXT("Player is nullptr."));
	}
}

void UGAMotionMatchingAnimInstance::GenerateTrajectory()
{
	const FPoseSearchTrajectoryData& TrajectoryData = (Speed2D> 0.0f) ? TrajectoryGenerationDataMoving : TrajectoryGenerationDataIdle;

	FPoseSearchQueryTrajectory ResultTrajectory;
	
	UPoseSearchTrajectoryLibrary::PoseSearchGenerateTrajectory(this, TrajectoryData, GetDeltaSeconds(), Trajectory, PreviousDesiredControllerYaw, ResultTrajectory, -1.0f, 30, 0.1f, 15);

	UPoseSearchTrajectoryLibrary::HandleTrajectoryWorldCollisions(GetWorld(), this, ResultTrajectory, true, 0.01f, Trajectory, TrajectoryCollision, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<AActor*>(), EDrawDebugTrace::None, true, 150.0f);

	FPoseSearchQueryTrajectorySample Sample00;

	UPoseSearchTrajectoryLibrary::GetTrajectorySampleAtTime(Trajectory, 0.5f, Sample00);
	
	FPoseSearchQueryTrajectorySample Sample01;

	UPoseSearchTrajectoryLibrary::GetTrajectorySampleAtTime(Trajectory, 0.4f, Sample01);

	FVector TmpVector((Sample00.Position - Sample01.Position) * 10.0f);

	FutureVelocity = TmpVector;
}

void UGAMotionMatchingAnimInstance::UpdateEssentialValues()
{
	VelocityAcceleration = (Velocity - VelocityLastFrame) / FMath::Max(UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 0.001f);
	
	if(HasVelocity == true)
	{
		LastNonZeroVelocity = Velocity;
	}
#if !UE_BUILD_SHIPPING
	// データ駆動型CVar（プロジェクト設定で作成されるコンソール変数）の値をキャッシュします。この整数をメインのMotion Matching Chooserで使用し、DenseとSparseのデータベースを入れ替えます。
	DatabaseLOD = UKismetSystemLibrary::GetConsoleVariableIntValue(TEXT("DDCvar.MMDatabaseLOD"));
#endif
	// 現在選択されているMotionマッチングデータベースのタグをキャッシュする。これにより、現在のデータベースに基づいて追加の選択を行うことができる。
	if(IsValid(CurrentSelectDatabase))
	{
		CurrentDatabaseTags = CurrentSelectDatabase->Tags;
	}
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

bool UGAMotionMatchingAnimInstance::IsMoving() const
{
	if((UKismetMathLibrary::NotEqual_VectorVector(Velocity      , FVector::ZeroVector, 0.1f) == true)
	&& (UKismetMathLibrary::NotEqual_VectorVector(FutureVelocity, FVector::ZeroVector, 0.1f) == true))
	{
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

FVector UGAMotionMatchingAnimInstance::CalculateRelativeAccelerationAmount() const
{
	FVector Result(FVector::ZeroVector);

	if(OwnerMovementComponent != nullptr)
	{
		if((OwnerMovementComponent->MaxAcceleration > 0.0f)
		&& (OwnerMovementComponent->GetMaxBrakingDeceleration() > 0.0f))
		{
			if(FVector::DotProduct(Acceleration, Velocity) > 0.0f)
			{
				FVector TmpVector(UKismetMathLibrary::Vector_ClampSizeMax(VelocityAcceleration, OwnerMovementComponent->MaxAcceleration));

				TmpVector /= OwnerMovementComponent->MaxAcceleration;

				Result = UKismetMathLibrary::Quat_UnrotateVector(CharacterTransform.GetRotation(), TmpVector);
			} else
			{
				FVector TmpVector(UKismetMathLibrary::Vector_ClampSizeMax(VelocityAcceleration, OwnerMovementComponent->GetMaxBrakingDeceleration()));

				TmpVector /= OwnerMovementComponent->GetMaxBrakingDeceleration();

				Result = UKismetMathLibrary::Quat_UnrotateVector(CharacterTransform.GetRotation(), TmpVector);
			}
		}
	}

	return Result;
}

FVector2D UGAMotionMatchingAnimInstance::GetLeanAmount() const
{
	FVector RelativeAccelerate(CalculateRelativeAccelerationAmount());

	float Range = UKismetMathLibrary::MapRangeClamped(Speed2D, 200.0f, 500.0f, 0.5, 1.0f);
	
	float Result = RelativeAccelerate.Y * Range;

	return FVector2D(Result, 0.0f);
}


EOffsetRootBoneMode UGAMotionMatchingAnimInstance::GetOffsetRootTranslationMode() const
{
	if(IsSlotActive(FName(TEXT("DefaultSlot"))))
	{
		return EOffsetRootBoneMode::Release;
	} else
	{
		if(MovementMode == EMotionMatchingMovementMode::OnGround)
		{
			if(IsMoving() == true)
			{
				return EOffsetRootBoneMode::Interpolate;
			} else
			{
				return EOffsetRootBoneMode::Release;
			}
		}
	}

	return EOffsetRootBoneMode::Release;
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

EOrientationWarpingSpace UGAMotionMatchingAnimInstance::GetOrientationWarpingSpace() const
{
	if(OffsetRootBoneEnable == true)
	{
		return EOrientationWarpingSpace::RootBoneTransform;
	} else
	{
		return EOrientationWarpingSpace::CustomTransform;
	}
}

// この Anim Node Function は、Motion Matching ノードが更新されるたびに呼び出されます。この関数はChooserアセットを評価し、現在のゲームプレイコンテキストに基づいたポーズ検索データベースアセットの配列を返します。
// これにより、より高度なフィルタリングを行うことができ、モーションマッチングシステムが選択できるアニメーションをよりコントロールできるようになります。
// 例えば、キャラクターが（プレイヤーの入力でコントロールされて）歩いているときだけ、歩行データベースから検索し、キャラクターが歩こうとしているときにモーションマッチングがランを選択するのを防ぎます。
void UGAMotionMatchingAnimInstance::UpdateMotionMatching(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	// ここからChooserTableのEvaluate処理
	// 2024年6月時点で実験的機能なのでこれから結構変更ありそう...。
	FChooserEvaluationContext ChooserContext(UChooserFunctionLibrary::MakeChooserEvaluationContext());

	ChooserContext.AddObjectParam(this);
	// 評価するChooserを設定
	FInstancedStruct ChooserInstance(UChooserFunctionLibrary::MakeEvaluateChooser(AnimationSearchChooser));
	// 評価を実行
	TArray<UObject*> Assets(UChooserFunctionLibrary::EvaluateObjectChooserBaseMulti(ChooserContext, ChooserInstance, UPoseSearchDatabase::StaticClass()));

	EAnimNodeReferenceConversionResult Result = EAnimNodeReferenceConversionResult::Failed;
	
	const FMotionMatchingAnimNodeReference& MotionMatchingNode = UMotionMatchingAnimNodeLibrary::ConvertToMotionMatchingNode(Node, Result);
	
	EPoseSearchInterruptMode InterruptMode = GetMotionMatchingInterruptMode();

	TArray<UPoseSearchDatabase*> DatabaseList;

	for(auto& Element:Assets)
	{
		UPoseSearchDatabase* Database = Cast<UPoseSearchDatabase>(Element);

		if(Database != nullptr)
		{
			DatabaseList.Add(Database);
		}
	}
		
	UMotionMatchingAnimNodeLibrary::SetDatabasesToSearch(MotionMatchingNode, DatabaseList, InterruptMode);
	
}

void UGAMotionMatchingAnimInstance::UpdateMotionMatchingPoseSelection(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult Result = EAnimNodeReferenceConversionResult::Failed;
	
	const FMotionMatchingAnimNodeReference& MotionMatchingNode = UMotionMatchingAnimNodeLibrary::ConvertToMotionMatchingNode(Node, Result);

	bool IsValidResult = false;
	
	FPoseSearchBlueprintResult SearchResult;
	
	UMotionMatchingAnimNodeLibrary::GetMotionMatchingSearchResult(MotionMatchingNode, SearchResult, IsValidResult);

	const UPoseSearchDatabase* Selected = SearchResult.SelectedDatabase.Get();
	
	CurrentSelectDatabase = const_cast<UPoseSearchDatabase*>(Selected);
}


EOffsetRootBoneMode UGAMotionMatchingAnimInstance::GetOffsetRootRotationMode() const
{
	if(IsSlotActive(FName(TEXT("DefaultSlot"))))
	{
		return EOffsetRootBoneMode::Release;
	} else
	{
		return EOffsetRootBoneMode::Accumulate;
	}
}

float UGAMotionMatchingAnimInstance::GetMotionMatchingBlendTime() const
{
	float Result = 0.0f;

	if(MovementMode == EMotionMatchingMovementMode::OnGround)
	{
		if(MovementModeLastFrame == EMotionMatchingMovementMode::OnGround)
		{
			Result = 0.4f;
		} else
		{
			Result = 0.2f;
		}
	} else
	{
		if(Velocity.Z > 100.0f)
		{
			Result = 0.15f;
		} else
		{
			Result = 0.5f;
		}
	}
	
	return Result;
}

FVector2D UGAMotionMatchingAnimInstance::GetAOValue() const
{
	if(OwnerCharacter == nullptr)
	{
		return FVector2D::ZeroVector;
	}

	FRotator CharacterRotate(OwnerCharacter->GetControlRotation());

	FRotator Normalized(UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotate, RootTransform.Rotator()));

	return FVector2D(Normalized.Yaw, Normalized.Pitch);
}

bool UGAMotionMatchingAnimInstance::IsEnableAO() const
{
	FVector2D AOValue(GetAOValue());

	if((FMath::Abs(AOValue.X) <= 115.0f)
	&& (RotationMode == EMotionMatchingRotationMode::Strafe)
	&& (GetSlotMontageLocalWeight(FName(TEXT("DefaultSlot"))) < 0.5f))
	{
		return true;
	}

	return false;
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

FQuat UGAMotionMatchingAnimInstance::GetTrajectoryFacing() const
{
	FPoseSearchQueryTrajectorySample TrajectorySample;
	
	UPoseSearchTrajectoryLibrary::GetTrajectorySampleAtTime(Trajectory, 0.5f, TrajectorySample);

	return TrajectorySample.Facing;
}

void UGAMotionMatchingAnimInstance::UpdateState()
{
	if((OwnerCharacter == nullptr) || (OwnerMovementComponent == nullptr))
	{
		SNPLUGIN_ERROR(TEXT("Owner Character is nullptr."));

		return;
	}

	MovementModeLastFrame = MovementMode;

	switch(OwnerMovementComponent->MovementMode)
	{
	case MOVE_None:
	case MOVE_Walking:
	case MOVE_NavWalking:
		MovementMode = EMotionMatchingMovementMode::OnGround; break;
	case MOVE_Falling:
		MovementMode = EMotionMatchingMovementMode::InAir; break;
	default:
		MovementMode = EMotionMatchingMovementMode::OnGround;
		SNPLUGIN_ERROR(TEXT("Invalid Movement Mode."));
		break;
	}
	
	RotationModeLastFrame = RotationMode;

	if(OwnerMovementComponent->bOrientRotationToMovement == true)
	{
		RotationMode = EMotionMatchingRotationMode::OrientToMovement;
	} else
	{
		RotationMode = EMotionMatchingRotationMode::Strafe;
	}

	MovementStateLastFrame = MovementState;

	if(IsMoving() == true)
	{
		MovementState = EMotionMatchingMovementState::Moving;
	} else
	{
		MovementState = EMotionMatchingMovementState::Idle;
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


