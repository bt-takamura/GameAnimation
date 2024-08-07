// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Component/MMLocomotionComponent.h"

#include "SNDef.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameAnimationSample/Environment/GALevelBlock.h"
#include "GameAnimationSample/Environment/GATraversalBlock.h"
#include "Components/CapsuleComponent.h"
#include "GameAnimationSample/Character/Animation/GAMotionMatchingAnimInstance.h"
#include "Chooser/Public/ChooserFunctionLibrary.h"
#include "PlayMontageCallbackProxy.h"
#include "MotionWarping/Public/MotionWarpingComponent.h"
#include "AnimationWarpingLibrary.h"

// Sets default values for this component's properties
UMMLocomotionComponent::UMMLocomotionComponent(const FObjectInitializer& Initializer)
: Super(Initializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UMMLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	//! 落下時のイベント関数を追加
	ACharacter* Character = GetOwner<ACharacter>();
	if (Character != nullptr)
	{
		Character->LandedDelegate.AddDynamic(this, &UMMLocomotionComponent::OnLandedDelegate);
	}
}

//----------------------------------------------------------------------//
//
//! @brief 着地時の処理、デリゲートに追加する関数
//
//! @param Hit	着地を示す結果
//
//----------------------------------------------------------------------//
void UMMLocomotionComponent::OnLandedDelegate(const FHitResult& Hit)
{
	ACharacter* Character = GetOwner<ACharacter>();
	if (Character == nullptr)
	{
		return;
	}

	LandSpeed = Character->GetCharacterMovement()->Velocity;

	bJustLanded = true;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(TimerHandle, this, &UMMLocomotionComponent::TurnOffJustLandded, 0.3f, false);
}

//----------------------------------------------------------------------//
//
//! @brief 直前に着地したフラグを下す、SetTimer関数で0.3秒後に呼び出す関数
//
//----------------------------------------------------------------------//
void UMMLocomotionComponent::TurnOffJustLandded()
{
	bJustLanded = false;
}


// Called every frame
void UMMLocomotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

}

void UMMLocomotionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//! タイマーの解放
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.ClearTimer(TimerHandle);
	TimerManager.ClearAllTimersForObject(this);
}

//----------------------------------------------------------------------//
//
//! @brief 最大スピード値を算出
//
//! @retval 最大スピード値
//
//! @note キャラクターの向きと進む方向が違う場合に移動スピードを変えてます。
//! @note 各スピード値はX：最大 / Y : 中間 / Z : 最小値を設定する必要があります。
//
//----------------------------------------------------------------------//
float UMMLocomotionComponent::CalculateMaxSpeed() const
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));
		return false;
	}
	FVector Velocity = Character->GetCharacterMovement()->Velocity;
	//! 速度とキャラクターの進行方向(ForwardVector)との角度を算出
	float Direction = FMath::Abs(UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation()));
	//! 向きに合ったスピード値を取得
	if (StrafeSpeedMapCurveObject == nullptr)
	{
		SNPLUGIN_LOG(TEXT("StrafeSpeedMapCurveObject is nullptr."));
		return false;
	}
	float StrafeSpeedMap = StrafeSpeedMapCurveObject->GetFloatValue(Direction);

	FVector Speed = FVector::ZeroVector;
	//! 屈み状態かチェック：実装なし
	//if (Character->GetCharacterMovement()->IsCrouching() == true)
	//{
	//	Speed = CrouchSpeed;
	//}
	//else
	{
		switch (Stride)
		{
		case EStride::Walk:		Speed = WalkSpeed;	 break;
		case EStride::Run:		Speed = RunSpeed;	 break;
		case EStride::Sprint:	Speed = SprintSpeed; break;
		default:
			break;
		}
	}

	float Result = 0.0f;
	if (StrafeSpeedMap < 1.0f)
	{
		Result = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 0.0f, 1.0f, Speed.X, Speed.Y);
	}
	else
	{
		Result = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 1.0f, 2.0f, Speed.Y, Speed.Z);
	}

	return Result;
}

//----------------------------------------------------------------------//
//
//! @brief スティックモードに合わせた入力値の取得
//
//! @retval	移動の入力値
//
//----------------------------------------------------------------------//
FVector2D UMMLocomotionComponent::GetMovementInputScaleValue(const FVector2D& Input) const
{
	FVector2D PostNormal = Input.GetSafeNormal();

	FVector2D Result = PostNormal;

	switch (MovementStickMode)
	{
	case EAnalogueMovementBehavior::VariableSpeed_SingleStride:
	case EAnalogueMovementBehavior::VariableSpeed_WalkRun:
		Result = Input;
		break;
	default:
		break;
	}
	return Result;
}

//----------------------------------------------------------------------//
//
//! @brief 入力状況に合わせて歩幅状態を設定する
//
//! @param Input	入力値
//
//----------------------------------------------------------------------//
void UMMLocomotionComponent::SetupStride(const FVector2D& Input)
{
	bool FullMovementInput = false;

	float InputMag = Input.Length();

	if (InputMag >= AnalogWalkRunThreshold)
	{
		FullMovementInput = true;
	}
	else
	{
		if ((MovementStickMode == EAnalogueMovementBehavior::FixedSpeed_SingleStride) || (MovementStickMode == EAnalogueMovementBehavior::VariableSpeed_SingleStride))
		{
			FullMovementInput = true;
		}
	}

	if (bWantsToSprint == true)
	{
		Stride = EStride::Run;
	}
	else 
	{
		if (bWantsToWalk == true)
		{
			Stride = EStride::Walk;
		}
		else
		{
			if (FullMovementInput == true)
			{
				Stride = EStride::Run;
			}
			else
			{
				Stride = EStride::Walk;
			}
		}
	}
}

//----------------------------------------------------------------------//
//
//! @brief トラバーサルアクションを実行
//
//! @param TraceForwardDistance		トレースする前方への距離
//! @param TraversalCheckFailed		Output,Traversalのチェックが失敗か
//! @param MontageSelectionFailed	Output,Montageの選択に失敗したか
//
//----------------------------------------------------------------------//
void UMMLocomotionComponent::ExecTraversalAction(float TraceForwardDistance, bool& TraversalCheckFailed, bool& MontageSelectionFailed)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));
		return;
	}

	//! Debug表示の設定を取得
	int DrawDebugLevel = UKismetSystemLibrary::GetConsoleVariableIntValue(TEXT("DDCvar.Traversal.DrawDebugLevel"));

	int DrawDebugDuration = UKismetSystemLibrary::GetConsoleVariableIntValue(TEXT("DDCvar.Traversal.DrawDebugDuration"));

	FTraversalCheckResult TraversalCheckResult;

	if (PerformForwardBlocks(TraversalCheckResult, TraceForwardDistance, DrawDebugLevel, DrawDebugDuration) == false)
	{
		TraversalCheckFailed = true;
		MontageSelectionFailed = false;

		return;
	}

	if (PerformDecisionOnActorToEachEdge(TraversalCheckResult, DrawDebugLevel) == false)
	{
		TraversalCheckFailed = true;
		MontageSelectionFailed = false;

		return;
	}

	DetermineTraversalAction(TraversalCheckResult);

	DebugPrintTraversalResult(DrawDebugLevel, TraversalCheckResult);

	if (TraversalCheckResult.ActionType != ETraversalActionType::None)
	{
		SetInteractTransform(TraversalCheckResult);

		TArray<UObject*> Assets = EvaluateChooser(TraversalCheckResult);

		if (PerformMotionMatch(Assets, TraversalCheckResult) == false)
		{
			TraversalCheckFailed = false;
			MontageSelectionFailed = true;

			return;
		}
	}
	else
	{
		TraversalCheckFailed = true;
		MontageSelectionFailed = false;
	}

}

//----------------------------------------------------------------------//
//
//! @brief トラバーサル判定のための前方にトレースする距離を算出 
//
//! @retval トレースする距離
//
//----------------------------------------------------------------------//
float UMMLocomotionComponent::GetTraversalForwardTraceDistance() const
{
	float Result = 0.0f;
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));
		return Result;
	}
	//! キャラクターの移動速度を取得
	FVector Velocity = Character->GetMovementComponent()->Velocity;
	//! アクターの向き分逆回転させる
	FVector Vector = Character->GetActorRotation().Quaternion().UnrotateVector(Velocity);
	//! 正面方向のベクトルを0～500レンジから75～350レンジにマッピング
	Result = UKismetMathLibrary::MapRangeClamped(Vector.X, 0.0f, 500.0f, 75.0f, 350.0f);

	return Result;
}

//----------------------------------------------------------------------//
//
//! @brief 進行方向にBlockがあるかチェック
//
//! @retval true	進行方向にBlockあり
//! @retval false	進行方向にBlockなし
//
//! @param TraversalCheckResult		トラバーサルアクションの判定結果
//! @param TraceForwardDistance		前方にトレースする距離
//! @param DrawDebugLevel			デバッグ表示のレベル
//! @param DrawDebugDuration		デバッグの表示期間
//
//----------------------------------------------------------------------//
bool UMMLocomotionComponent::PerformForwardBlocks(FTraversalCheckResult& TraversalCheckResult, float TraceForwardDistance, int DrawDebugLevel, float DrawDebugDuration)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	FVector ActorLocation = Character->GetActorLocation();

	FVector ActorForwardVector = Character->GetActorForwardVector();

#if 0
	FVector EndPoint = ActorLocation + ActorForwardVector * TraceForwardDistance;
#else
	FVector Velocity = Character->GetMovementComponent()->Velocity;

	FVector Remap(
		UKismetMathLibrary::MapRangeClamped(Velocity.X, 0.0f, 500.0f, 75.0f, 350.0f),
		UKismetMathLibrary::MapRangeClamped(Velocity.Y, 0.0f, 500.0f, 75.0f, 350.0f),
		UKismetMathLibrary::MapRangeClamped(Velocity.Z, 0.0f, 500.0f, 75.0f, 350.0f)
	);

	FVector EndPoint = ActorLocation + Velocity;
#endif
	EDrawDebugTrace::Type DebugTrace = (DrawDebugLevel >= 2) ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	//! 進行方向にBlockがあるかチェック
	FHitResult HitResult;
	UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(), ActorLocation, EndPoint,
		30.0f, 60.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
		TArray<AActor*>(), DebugTrace, HitResult, true,
		FLinearColor::Black, FLinearColor::Yellow
	);

	bool Result = HitResult.bBlockingHit;

	if (Result == true)
	{
		AGALevelBlock* LevelBlock = Cast<AGALevelBlock>(HitResult.GetActor());

		if (LevelBlock != nullptr)
		{
			TraversalCheckResult.HitComponent = HitResult.GetComponent();

			LevelBlock->GetLedgeTransform(HitResult.ImpactPoint, ActorLocation, TraversalCheckResult);

			DrawDebugShapesAtLedgeLocation(TraversalCheckResult, DrawDebugLevel, DrawDebugDuration);
		}
		else
		{
			AGATraversalBlock* TraversalBlock = Cast<AGATraversalBlock>(HitResult.GetActor());
			if (TraversalBlock != nullptr)
			{
				TraversalCheckResult.HitComponent = HitResult.GetComponent();

				TraversalBlock->GetLedgeTransform(HitResult.ImpactPoint, ActorLocation, TraversalCheckResult);

				DrawDebugShapesAtLedgeLocation(TraversalCheckResult, DrawDebugLevel, DrawDebugDuration);
			}
			else
			{
				Result = false;
			}
		}
	}

	return Result;
}

//----------------------------------------------------------------------//
//
//! @brief レッジの座標に球を描画
//
//! @param TraversalCheckResult トラバーサルアクションの判定結果
//! @param DrawDebugLevel		デバッグ表示のレベル
//
//----------------------------------------------------------------------//
void UMMLocomotionComponent::DrawDebugShapesAtLedgeLocation(const FTraversalCheckResult& TraversalCheckResult, int DrawDebugLevel, float DrawDebugDuration)
{
	if (DrawDebugLevel >= 1)
	{
		if (TraversalCheckResult.HasFrontLedge == true)
		{
			DrawDebugSphere(GetWorld(), TraversalCheckResult.FrontLedgeLocation, 10.0f, 12, FColor::Green, false, DrawDebugDuration);
			DrawDebugSphere(GetWorld(), TraversalCheckResult.BackLedgeLocation, 10.0f, 12, FColor::Blue, false, DrawDebugDuration);
		}
	}
}

//----------------------------------------------------------------------//
//
//! @brief 障害物を乗り越えた場合の障害物の先の状態をチェックする
//
//! @retval true	トラバーサル可能
//! @retval false	トラバーサルできない
//
//! @param TraversalCheckResult トラバーサルアクション判定の結果
//! @param DrawDebugLevel		デバッグ表示のレベル
//
//----------------------------------------------------------------------//
bool UMMLocomotionComponent::PerformDecisionOnActorToEachEdge(FTraversalCheckResult& TraversalCheckResult, int DrawDebugLevel)
{
	if (TraversalCheckResult.HasFrontLedge == false)
	{
		return false;
	}

	FVector FrontLedgeLocation = TraversalCheckResult.FrontLedgeLocation;
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	//! コリジョンの半径を取得
	float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	//! コリジョンの高さの半分を取得
	float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FVector FrontLedgeNormalOffset = TraversalCheckResult.FrontLedgeNormal * (CapsuleRadius + 2.0f);
	float HeightOffset = CapsuleHalfHeight + 2.0f;
	//! 障害物との空間チェック用の位置情報を算出
	FVector HasRoomCheckFromLedgeLocation = FrontLedgeLocation + FrontLedgeNormalOffset + FVector(0.0f, 0.0f, HeightOffset);

	bool Result = PerformActorToFrontEdge(TraversalCheckResult, HasRoomCheckFromLedgeLocation, DrawDebugLevel);
	if (Result == false)
	{
		return false;
	}

	//! BlockActorのスプラインから得た障害物の反対側のロケーション情報を取得
	FVector BackLedgeLocation(TraversalCheckResult.BackLedgeLocation);
	//! 法線の方向にコリジョンの半径分オフセットさせる
	FVector BackLedgeDepthOffset = TraversalCheckResult.BackLedgeNormal * (CapsuleRadius * 2.0f);

	FVector HasRoomCheckBackLedgeLocation = BackLedgeLocation + BackLedgeDepthOffset + FVector(0.0f, 0.0f, HeightOffset);

	PerformObstacleDepth(TraversalCheckResult, HasRoomCheckFromLedgeLocation, HasRoomCheckBackLedgeLocation, DrawDebugLevel);

	if (TraversalCheckResult.HasBackLedge == true)
	{
		PerformBackLedgeFloor(TraversalCheckResult, HasRoomCheckBackLedgeLocation, DrawDebugLevel);
	}

	return true;
}

//----------------------------------------------------------------------//
//
//! @brief アクタが移動できるスペースがあるかどうかをチェック
//
//! @retval true	スペースがあるので続行
//! @retval false	スペースがないので早期終了
//
//! @param TraversalCheckResult				トラバーサルアクション判定の結果
//! @param HasRoomCheckFromLedgeLocation	障害物との空間チェック用の位置情報
//! @param DrawDebugLevel					デバッグ表示のレベル
// 
//----------------------------------------------------------------------//
bool UMMLocomotionComponent::PerformActorToFrontEdge(FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckFromLedgeLocation, int DrawDebugLevel)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	//! コリジョンの半径を取得
	float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	//! コリジョンの高さの半分を取得
	float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	EDrawDebugTrace::Type DebugTrace = (DrawDebugLevel >= 3) ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	FHitResult HitResult = {};
	FVector ActorLocation = Character->GetActorLocation();

	//! 障害物との空間をチェック
	UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(), ActorLocation, HasRoomCheckFromLedgeLocation,
		CapsuleRadius, CapsuleHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
		TArray<AActor*>(), DebugTrace, HitResult, true,
		FLinearColor::Blue, FLinearColor::Blue
	);

	bool Result = ((HitResult.bBlockingHit == false) && (HitResult.bStartPenetrating == false)) ? true : false;

	if (Result == true)
	{
		TraversalCheckResult.ObstacleHeight = FMath::Abs(((ActorLocation - FVector(0.0f, 0.0f, CapsuleHalfHeight)) - TraversalCheckResult.FrontLedgeLocation).Z);
	}
	else
	{
		TraversalCheckResult.HasFrontLedge = false;
	}

	return Result;
}

//----------------------------------------------------------------------//
//
//! @brief 障害物の上にアクターが移動できるスペースがあるかチェック
//
//! @param TraversalCheckResult				トラバーサルアクション判定の結果
//! @param HasRoomCheckFrontLedgeLocation	障害物との空間チェック用の手前の位置情報
//! @param HasRoomCheckBackLedgeLocation	障害物との空間チェック用の奥の位置情報
//! @param DrawDebugLevel					デバッグ表示のレベル
// 
//----------------------------------------------------------------------//
void UMMLocomotionComponent::PerformObstacleDepth(FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckFrontLedgeLocation, const FVector& HasRoomCheckBackLedgeLocation, int DrawDebugLevel)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	//! コリジョンの半径を取得
	float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	//! コリジョンの高さの半分を取得
	float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FHitResult HitResult = {};

	EDrawDebugTrace::Type DebugTrace = (DrawDebugLevel >= 3) ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	//! 後ろに障害物があるかチェック
	//! 障害物の上を手前から奥にトレース
	bool Result = UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(), HasRoomCheckFrontLedgeLocation, HasRoomCheckBackLedgeLocation,
		CapsuleRadius, CapsuleHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
		TArray<AActor*>(), DebugTrace, HitResult, true,
		FLinearColor::White, FLinearColor::White
	);
	if (Result != true)
	{
		//! 物がない場合、ObstacleDepthに障害物の奥行(XY平面)を設定
		TraversalCheckResult.ObstacleDepth = (TraversalCheckResult.FrontLedgeLocation - TraversalCheckResult.BackLedgeLocation).Size2D();
	}
	else
	{
		//! 奥に別の障害物がある場合、HasBackLedgeをfalseに設定
		TraversalCheckResult.HasBackLedge = false;
		//! ObstacleDepthに奥にある障害物との距離(XY平面)を設定
		TraversalCheckResult.ObstacleDepth = (HitResult.ImpactPoint - TraversalCheckResult.FrontLedgeLocation).Size2D();
	}
}

//----------------------------------------------------------------------//
//
//! @brief バックレッジの位置から下に向かって床を探す
//
//! @param TraversalCheckResult				トラバーサルアクション判定の結果
//! @param HasRoomCheckBackLedgeLocation	障害物との空間チェック用の奥の位置情報
//! @param DrawDebugLevel					デバッグ表示のレベル
// 
//----------------------------------------------------------------------//
void UMMLocomotionComponent::PerformBackLedgeFloor(FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckBackLedgeLocation, int DrawDebugLevel)
{
	FVector BackLedgeLocation = TraversalCheckResult.BackLedgeLocation;

	FVector BackLedgeNormal = TraversalCheckResult.BackLedgeNormal;

	ACharacter* Character = Cast<ACharacter>(GetOwner());

	//! コリジョンの半径を取得
	float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	//! コリジョンの高さの半分を取得
	float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float HeightOffset = TraversalCheckResult.ObstacleHeight - CapsuleHalfHeight + 50.0f;

	FVector EndPoint = (BackLedgeLocation + BackLedgeNormal * (CapsuleRadius + 2.0f)) - FVector(0.0f, 0.0f, HeightOffset);

	FHitResult HitResult = {};

	EDrawDebugTrace::Type DebugTrace = (DrawDebugLevel >= 3) ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(), HasRoomCheckBackLedgeLocation, EndPoint,
		CapsuleRadius, CapsuleHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
		TArray<AActor*>(), DebugTrace, HitResult, true,
		FLinearColor::Red, FLinearColor::Red
	);

	if (HitResult.bBlockingHit == true)
	{
		//! 床があればその位置とバッグレッジの高さを格納
		TraversalCheckResult.HasBackFloor = true;
		TraversalCheckResult.BackFloorLocation = HitResult.ImpactPoint;
		TraversalCheckResult.BackLedgeHeight = FMath::Abs((HitResult.ImpactPoint - TraversalCheckResult.BackLedgeLocation).Z);
	}
	else
	{
		//! 床がなければバックフロアを無効にする
		TraversalCheckResult.HasBackFloor = false;
	}
}

//----------------------------------------------------------------------//
//
//! @brief トラバーサルアクションのタイプを設定
//
//! @param TraversalCheckResult トラバーサルアクション判定の結果
//
//----------------------------------------------------------------------//
void UMMLocomotionComponent::DetermineTraversalAction(FTraversalCheckResult& TraversalCheckResult)
{
	if ((TraversalCheckResult.HasFrontLedge == true)
		&& (TraversalCheckResult.HasBackLedge == true)
		&& (TraversalCheckResult.HasBackFloor != true)
		&& (UKismetMathLibrary::InRange_FloatFloat(TraversalCheckResult.ObstacleHeight, 50.0f, 125.0f))
		&& (TraversalCheckResult.ObstacleDepth < 59.0f))
	{
		TraversalCheckResult.ActionType = ETraversalActionType::Vault;
	}
	else if ((TraversalCheckResult.HasFrontLedge == true)
		&& (TraversalCheckResult.HasBackLedge == true)
		&& (TraversalCheckResult.HasBackFloor == true)
		&& (UKismetMathLibrary::InRange_FloatFloat(TraversalCheckResult.ObstacleHeight, 50.0f, 125.0f))
		&& (TraversalCheckResult.ObstacleDepth < 59.0f)
		&& (TraversalCheckResult.BackLedgeHeight > 50.0f))
	{
		TraversalCheckResult.ActionType = ETraversalActionType::Hurdle;
	}
	else if ((TraversalCheckResult.HasFrontLedge == true)
		&& (UKismetMathLibrary::InRange_FloatFloat(TraversalCheckResult.ObstacleHeight, 50.0f, 275.0f))
		&& (TraversalCheckResult.ObstacleDepth >= 59.0f))
	{
		TraversalCheckResult.ActionType = ETraversalActionType::Mantle;
	}
	else
	{
#if 1	// @@Satoshi Nishimura 2024/07/10
		ACharacter* Character = Cast<ACharacter>(GetOwner());

		if (Character->GetCharacterMovement()->IsMovingOnGround() == false)
		{
			if ((TraversalCheckResult.HasFrontLedge == true)
				&& (UKismetMathLibrary::InRange_FloatFloat(TraversalCheckResult.ObstacleHeight, 25.0f, 275.0f))
				&& (TraversalCheckResult.ObstacleDepth >= 59.0f))
			{
				TraversalCheckResult.ActionType = ETraversalActionType::Mantle;
			}
			else
			{
				TraversalCheckResult.ActionType = ETraversalActionType::None;
			}
		}
		else
		{
			TraversalCheckResult.ActionType = ETraversalActionType::None;
		}

#else
		TraversalCheckResult.ActionType = ETraversalActionType::None;
#endif
	}
}

//----------------------------------------------------------------------//
//
//! @brief トラバーサルアクションの判定結果をテキスト表示
//
//! @param DrawDebugLevel		デバッグ表示のレベル
//! @param TraversalCheckResult	トラバーサルアクション判定の結果
//
//----------------------------------------------------------------------//
void UMMLocomotionComponent::DebugPrintTraversalResult(int DrawDebugLevel, FTraversalCheckResult& TraversalCheckResult)
{
	if (DrawDebugLevel >= 1)
	{
		FString Str(TEXT("Has Front Ledge : "));
		Str += (TraversalCheckResult.HasFrontLedge == true) ? TEXT("True") : TEXT("False");
		Str += "\n";

		Str += TEXT("Has Back Ledge : ");
		Str += (TraversalCheckResult.HasBackLedge == true) ? TEXT("True") : TEXT("False");
		Str += "\n";

		Str += TEXT("Has Back Floor : ");
		Str += (TraversalCheckResult.HasBackLedge == true) ? TEXT("True") : TEXT("False");
		Str += "\n";

		Str += TEXT("Obstacle Height : ");
		Str += (TraversalCheckResult.HasBackLedge == true) ? TEXT("True") : TEXT("False");
		Str += "\n";

		Str += TEXT("Obstacle Depth : ");
		Str += (TraversalCheckResult.HasBackLedge == true) ? TEXT("True") : TEXT("False");
		Str += "\n";

		Str += TEXT("Back Ledge Height : ");
		Str += (TraversalCheckResult.HasBackLedge == true) ? TEXT("True") : TEXT("False");
		Str += "\n";

		Str += FString::SanitizeFloat(TraversalCheckResult.BackLedgeHeight);

		UKismetSystemLibrary::PrintString(GetWorld(), Str, true, false);

		FString CurrentAction = StaticEnum<ETraversalActionType>()->GetValueAsString(TraversalCheckResult.ActionType);

		UKismetSystemLibrary::PrintString(GetWorld(), CurrentAction, true, false);
	}
}

//----------------------------------------------------------------------//
//
//! @brief InterfaceでAnimBPにフロントレッジの情報を送る
//
//! @param TraversalCheckResult  トラバーサルアクション判定の結果
//
//----------------------------------------------------------------------//
void UMMLocomotionComponent::SetInteractTransform(FTraversalCheckResult& TraversalCheckResult)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	UGAMotionMatchingAnimInstance* AnimInstance = Cast<UGAMotionMatchingAnimInstance>(Character->GetMesh()->GetAnimInstance());
	if (AnimInstance == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Animamtion Instance is nullptr."));
		return;
	}

	FRotator Rotator = UKismetMathLibrary::MakeRotFromZ(TraversalCheckResult.FrontLedgeNormal);
	FTransform Transform = UKismetMathLibrary::MakeTransform(TraversalCheckResult.FrontLedgeLocation, Rotator);

	AnimInstance->SetInteractTransform(Transform);
}

//----------------------------------------------------------------------//
//
//! @brief トラバーサル・チェックの条件に一致するすべてのモンタージュを選択するために、チューザーを評価する。
//
//! @param TraversalCheckResult トラバーサルアクション判定の結果
//
//! @retval 評価後、該当したObject
//
//----------------------------------------------------------------------//
TArray<UObject*> UMMLocomotionComponent::EvaluateChooser(FTraversalCheckResult& TraversalCheckResult)
{
	FGATraversalChooserParams Params;
	//! 現状のプレイヤーのアクションを設定
	Params.ActionType = TraversalCheckResult.ActionType;
	//! 歩幅を設定
	Params.Stride = Stride;

	//! 移動速度を取得
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));
		return TArray<UObject*>();
	}
	FVector Velocity = Character->GetCharacterMovement()->Velocity;
	//! 移動速度(XY平面)を設定
	Params.Speed = Velocity.Size2D();

	Params.ObstacleHeight = TraversalCheckResult.ObstacleHeight;
	Params.ObstacleDepth = TraversalCheckResult.ObstacleDepth;

	//! チューザーを評価する
	FChooserEvaluationContext Context = UChooserFunctionLibrary::MakeChooserEvaluationContext();
	//! ChooserTable評価用の構造体を設定(ChooserTable側に設定している構造体の型と同じもの)
	Context.AddStructParam(Params);
	//! 評価するChooserを設定
	FInstancedStruct ChooserInstance = UChooserFunctionLibrary::MakeEvaluateChooser(TraversalAnimationChooser);
	//! 評価を実行
	TArray<UObject*> Assets(UChooserFunctionLibrary::EvaluateObjectChooserBaseMulti(Context, ChooserInstance, UAnimMontage::StaticClass()));

	return Assets;
}

//----------------------------------------------------------------------//
//
//! @brief チューザーで選択されたモンタージュから最良のモンタージュを検索する
//
//! @param SearchAssets			チューザーで選択されたモンタージュ
//! @param TraversalCheckResult 
//
//! @retval true	MotionMatchingの成功
//! @retval false	MotionMatchingの失敗
//
//----------------------------------------------------------------------//
bool UMMLocomotionComponent::PerformMotionMatch(TArray<UObject*> SearchAssets, FTraversalCheckResult& TraversalCheckResult)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	UGAMotionMatchingAnimInstance* AnimInstanceBase = Cast<UGAMotionMatchingAnimInstance>(Character->GetMesh()->GetAnimInstance());
	if (AnimInstanceBase == nullptr)
	{
		SNPLUGIN_ERROR(TEXT("Animation Instance is nullptr."));
		return false;
	}

	FPoseSearchBlueprintResult Result;
	//! 選択されたモンタージュから最良のモンタージュを検索
	UPoseSearchLibrary::MotionMatch(AnimInstanceBase, SearchAssets, FName(TEXT("PoseHistory")), FPoseSearchFutureProperties(), Result, 42);

	const UAnimMontage* AnimMontage = Cast<UAnimMontage>(Result.SelectedAnimation);
	if (AnimMontage == nullptr)
	{
		//! データベースが無効かスキーマに問題がある可能性あり
		SNPLUGIN_ERROR(TEXT("Failed To Find Montage!"));
		return false;
	}
	//! const外し
	TraversalCheckResult.ChosenMontage = const_cast<UAnimMontage*>(AnimMontage);

	TraversalCheckResult.StartTime = Result.SelectedTime;

	TraversalCheckResult.PlayRate = Result.WantedPlayRate;

	FString Str = TEXT("Start Time : ");
	Str += FString::SanitizeFloat(TraversalCheckResult.StartTime);
	Str += "/";
	Str += FString::SanitizeFloat(TraversalCheckResult.StartTime);
	UKismetSystemLibrary::PrintString(GetWorld(), Str, true, false);

	//! トラバーサルを実行
	ExecPerformTraversalAction(TraversalCheckResult);

	return true;
}

//----------------------------------------------------------------------//
//
//! @brief トラバーサルアクションを実行(モンタージュの再生)
//
//! @param TraversalCheckResult トラバーサルアクション判定の結果
//
//----------------------------------------------------------------------//
void UMMLocomotionComponent::ExecPerformTraversalAction(const FTraversalCheckResult& TraversalCheckResult)
{
	TraversalResult = TraversalCheckResult;
	//! フロントレッジの位置に球をDebug描画
	DrawDebugSphere(GetWorld(), TraversalResult.FrontLedgeLocation, 10.0f, 12, FColor::Green, false, 30.0f);

	//! ワープターゲットの更新
	UpdateWarpTarget();
	//! パルクール用のモンタージュを再生
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));
		return;
	}
	UPlayMontageCallbackProxy* Proxy = UPlayMontageCallbackProxy::CreateProxyObjectForPlayMontage(Character->GetMesh(), TraversalResult.ChosenMontage, TraversalResult.PlayRate, TraversalResult.StartTime, NAME_None);

	if (Proxy != nullptr)
	{
		// CompleteとInterrupt(途中で他のモーションに差し込まれた)を登録
		// これが正常にされないとコリジョンを無視したままになる。
		Proxy->OnCompleted.AddDynamic(this, &UMMLocomotionComponent::EndPlayMontage);
		Proxy->OnInterrupted.AddDynamic(this, &UMMLocomotionComponent::EndPlayMontage);
		Proxy->OnBlendOut.AddDynamic(this, &UMMLocomotionComponent::EndPlayMontage);
		
		bDoingTraversalAction = true;

		//! コリジョン無視を有効に設定
		Character->GetCapsuleComponent()->IgnoreComponentWhenMoving(TraversalResult.HitComponent, true);
		//! ムーブメントモードを設定
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

		SNPLUGIN_LOG(TEXT("Motage is Playing & Delegate Registed."));
	}
	else
	{
		SNPLUGIN_LOG(TEXT("Montage can't Play."));
	}
}

//----------------------------------------------------------------------//
//
//! @brief モンタージュの終了処理
//
//! @param NotifyName 
//
//----------------------------------------------------------------------//
void UMMLocomotionComponent::EndPlayMontage(FName NotifyName)
{
	SNPLUGIN_LOG(TEXT("End play montage."));
	
	//! コリジョンやムーブメントモード、フラグを戻す。
	bDoingTraversalAction = false;

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));
		return;
	}
	//! コリジョン無視を無効に設定
	Character->GetCapsuleComponent()->IgnoreComponentWhenMoving(TraversalResult.HitComponent, false);

	EMovementMode Mode = {};
	switch (TraversalResult.ActionType)
	{
	case ETraversalActionType::None:	Mode = MOVE_Walking;	break;
	case ETraversalActionType::Hurdle:	Mode = MOVE_Walking;	break;
	case ETraversalActionType::Vault:	Mode = MOVE_Falling;	break;
	case ETraversalActionType::Mantle:	Mode = MOVE_Walking;	break;
	default:
		 Mode = MOVE_Falling;	break;
	}

	//! ムーブメントモードを設定
	Character->GetCharacterMovement()->SetMovementMode(Mode);
}

//----------------------------------------------------------------------//
//
//! @brief モーションワーピングの移動先を更新
//
//----------------------------------------------------------------------//
void UMMLocomotionComponent::UpdateWarpTarget()
{
	//! 初めの手の置く位置
	FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(UKismetMathLibrary::NegateVector(TraversalResult.FrontLedgeNormal));
	//! 指定位置にワープを実行
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Character is nullptr."));
		return;
	}
	UMotionWarpingComponent* MotionWarpingComponent = Character->GetComponentByClass<UMotionWarpingComponent>();
	if (MotionWarpingComponent == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Traversal : Not find MotionWarpingComponent."));
		return;
	}
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName(TEXT("FrontLedge")), TraversalResult.FrontLedgeLocation, TargetRotation);

	float AnimatedDistanceFromFrontLedgeToBackLedge = 0.0f;

	//! 奥の端への移動
	if ((TraversalResult.ActionType == ETraversalActionType::Hurdle) || (TraversalResult.ActionType == ETraversalActionType::Vault))
	{
		TArray<FMotionWarpingWindowData> BackLedgeDataList;
		UMotionWarpingUtilities::GetMotionWarpingWindowsForWarpTargetFromAnimation(TraversalResult.ChosenMontage, FName(TEXT("BackLedge")), BackLedgeDataList);

		if (BackLedgeDataList.IsEmpty() != true)
		{
			const FMotionWarpingWindowData& Data = BackLedgeDataList[0];
			//! アニメーションデータからカーブ情報を取得
			//! 「Distance_From_Ledge」はアニメーションシーケンス内に設定されているカーブの名前
			UAnimationWarpingLibrary::GetCurveValueFromAnimation(TraversalResult.ChosenMontage, FName(TEXT("Distance_From_Ledge")), Data.EndTime, AnimatedDistanceFromFrontLedgeToBackLedge);
			//! 指定位置にワープを実行
			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName(TEXT("BackLedge")), TraversalResult.BackLedgeLocation, FRotator::ZeroRotator);
		}
		else
		{
			//! BackLedgeに関連付けているワープターゲットを削除
			MotionWarpingComponent->RemoveWarpTarget(FName(TEXT("BackLedge")));
		}
	}
	else
	{
		//! BackLedgeに関連付けているワープターゲットを削除
		MotionWarpingComponent->RemoveWarpTarget(FName(TEXT("BackLedge")));
	}

	//! 奥の床に登る移動
	if (TraversalResult.ActionType == ETraversalActionType::Hurdle)
	{
		TArray<FMotionWarpingWindowData> DataList;

		UMotionWarpingUtilities::GetMotionWarpingWindowsForWarpTargetFromAnimation(TraversalResult.ChosenMontage, FName(TEXT("BackFloor")), DataList);

		if (DataList.IsEmpty() != true)
		{
			const FMotionWarpingWindowData& Data = DataList[0];

			float AnimatedDistanceFromFrontLedgeToBackFloor = 0.0f;

			UAnimationWarpingLibrary::GetCurveValueFromAnimation(TraversalResult.ChosenMontage, FName(TEXT("Distance_From_Ledge")), Data.EndTime, AnimatedDistanceFromFrontLedgeToBackFloor);

			float Bias = FMath::Abs(AnimatedDistanceFromFrontLedgeToBackLedge - AnimatedDistanceFromFrontLedgeToBackFloor);

			FVector NormalOffset = TraversalResult.BackLedgeNormal * Bias;

			FVector Tmp = TraversalResult.BackLedgeLocation + NormalOffset;

			FVector TargetLocation(Tmp.X, Tmp.Y, TraversalResult.BackFloorLocation.Z);
			//! 指定位置にワープを実行
			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName(TEXT("BackFloor")), TargetLocation, FRotator::ZeroRotator);
		}
		else
		{
			//! BackFloorに関連付けているワープターゲットを削除
			MotionWarpingComponent->RemoveWarpTarget(FName(TEXT("BackFloor")));
		}
	}
	else
	{
		//! BackFloorに関連付けているワープターゲットを削除
		MotionWarpingComponent->RemoveWarpTarget(FName(TEXT("BackFloor")));
	}
}
