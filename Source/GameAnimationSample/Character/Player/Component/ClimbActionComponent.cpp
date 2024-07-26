// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Component/ClimbActionComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "MotionWarpingComponent.h"
#include "GameAnimationSample/Character/Player/Interface/ClimbActionInterface.h"
#include "KismetAnimationLibrary.h"

DEFINE_LOG_CATEGORY(ClimbComp);

// Sets default values for this component's properties
UClimbActionComponent::UClimbActionComponent()
: TargetName(TEXT("Climb"))
, IsClimb(false)
, IsClimbWallEdge(false)
, ClimbWallNormal(0.0f, 0.0f, 0.0f)
, ClimbTransform()
, ImpactWallLocation(0.0f, 0.0f, 0.0f)
, ImpactWallNormal(0.0f, 0.0f, 0.0f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UClimbActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UClimbActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//----------------------------------------------------------------------//
//
//! @brief Climbを試みる、壁に掴まる
//
//! @retval true	Climbに成功
//! @retval false	Climbに失敗
//
//! @param MotionWarping MotionWarpingのコンポーネント
//
//----------------------------------------------------------------------//
UPARAM(DisplayName = "ClimbSuccess") bool UClimbActionComponent::TryAction(UMotionWarpingComponent* MotionWarping)
{
	bool Result = FindObjectInFront();
	if (Result == false)
	{
		return false;
	}

	Result = GraspWall(MotionWarping);
	if (Result == false)
	{
		return false;
	}

	IsClimb = true;
	return true;
}

//----------------------------------------------------------------------//
//
//! @brief Climb中の移動
//
//! @param InputValue コントローラー3Dスティック入力値
//
//----------------------------------------------------------------------//
void UClimbActionComponent::MoveAction(const FVector2D& InputValue)
{
	//! 掴まる動作時(Montage再生中)は移動しない
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return;
	}
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance->Montage_IsPlaying(ClimbAnimMontage))
	{
		return;
	}

	bool PreIsClimbWallEdge = IsClimbWallEdge;
	//! 壁の端の判定を取る
	IsClimbWallEdge = CheckWallEdge(InputValue);

	//! 壁端までの距離を確認
	bool IsCloseToEdge = CheckDistanceToWall();

	//! 前方のCollisionが取れなくなった時、壁との距離が近づいた時 = 壁の端まで来た判定
	if ((IsClimbWallEdge == false && PreIsClimbWallEdge == true) || IsCloseToEdge == true)
	{
		//! 端に来たら動作を止める
		Character->GetCharacterMovement()->StopMovementImmediately();

		//! 壁の向きを確認する
		bool UpSideWall = CheckWallDirection();
		//! Transformで移動する
		if (UpSideWall == true)
		{
			GoUp();
		}
		else
		{
			Turn();
		}

		IsClimbWallEdge = false;
	}
	else
	{
		//! 壁端までは、入力で移動する
		InputMovement(InputValue);
	}

}

//----------------------------------------------------------------------//
//
//! @brief Climb動作を終了する
//
//! @retval true	Climb動作をキャンセル
//
//! @param
//
//----------------------------------------------------------------------//
UPARAM(DisplayName = "CanselSuccess") bool UClimbActionComponent::CanselAction()
{
	if (IsClimb == false)
	{
		return false;
	}

	//! Climb中のフラグを下ろし、MovementModeをFallingにして落下させる
	IsClimb = false;
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return false;
	}
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	Character->GetCharacterMovement()->GravityScale = 1.0f;

	return true;
}

//----------------------------------------------------------------------//
//
//! @brief EMovementMode::MOVE_Falling時の最高移動速度を求める
//
//! @retval float EMovementMode::MOVE_Falling時の最高移動速度
//
//----------------------------------------------------------------------//
float UClimbActionComponent::CalcMaxFlySpeed() const
{
	//! FlyMaxSpeedデフォルト値
	float Speed = 600.0f;
	if (IsClimb == false)
	{
		return Speed;
	}

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return Speed;
	}
	float Value = UKismetAnimationLibrary::CalculateDirection(Character->GetCharacterMovement()->Velocity, Character->GetActorRotation());
	Value = UKismetMathLibrary::Abs(Value);
	
	if (SpeedMapCurve == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("SpeedMapCurve is nullptr."));
		return Speed;
	}
	float CurveValue = SpeedMapCurve->GetFloatValue(Value);
	if (CurveValue < 1.0f)
	{
		//! マッピングカーブに応じた速度を算出
		Speed = UKismetMathLibrary::MapRangeClamped(CurveValue, 0.0f, 1.0f, ClimbSpeeds.X, ClimbSpeeds.Y);
	}
	else
	{
		Speed = UKismetMathLibrary::MapRangeClamped(CurveValue, 1.0f, 2.0f, ClimbSpeeds.Y, ClimbSpeeds.Z);
	}

	return Speed;
}

//----------------------------------------------------------------------//
//
//! @brief Climb中のCharacterの回転設定
//
//----------------------------------------------------------------------//
void UClimbActionComponent::SetClimbRotation() const
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return;
	}
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
}

void UClimbActionComponent::ResetVelocity()
{
	//! EMovementMode::MOVE_Fallingにより減衰が弱いためVelocityを0.0fにする
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return;
	}
	Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
}

//----------------------------------------------------------------------//
//
//! @brief 前方にオブジェクトがあるか判定する
//
//! @retval true	前方にオブジェクトがある
//! @retval false	前方にオブジェクトがない
//
//----------------------------------------------------------------------//
bool UClimbActionComponent::FindObjectInFront()
{
	//! 今のアクターの座標(Start)からアクターの前方方向ベクトル * 判定距離(End)までトレース
	FVector ActorLocation = GetOwner()->GetActorLocation();
	FVector ActorForwardVec = GetOwner()->GetActorForwardVector();
	
	//! CapsuleCompの情報取得
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return false;
	}
	float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FHitResult HitResult = {};
	bool Result = UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(), ActorLocation, ActorLocation + (ActorForwardVec * TraceDistance),
		CapsuleRadius, CapsuleHalfHeight,
		TraceChannel, false, TArray<AActor*>(),
		/*EDrawDebugTrace::ForDuration*/EDrawDebugTrace::None,
		HitResult, true
	);
	if (Result == false)
	{
		return false;
	}
	
	ClimbWallNormal = HitResult.ImpactNormal;
	//! 掴まる姿勢を設定
	IClimbActionInterface* ClimbInterface = Cast<IClimbActionInterface>(GetOwner());
	if (ClimbInterface == nullptr)
	{
		return false;
	}
	ClimbInterface->SetClimbGraspTransform(HitResult, ClimbTransform);

	return true;
}

//----------------------------------------------------------------------//
//
//! @brief 壁を掴む
//
//! @retval true	行動に成功
//! @retval false	行動失敗
//
//! @param MotionWarping MotionWarpingのコンポーネント
//
//----------------------------------------------------------------------//
bool UClimbActionComponent::GraspWall(UMotionWarpingComponent* MotionWarping)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return false;
	}
	//! 重力に反した動作のためMovementModeをFlyingに設定
	//! 動作的にはMovementModeのみでいいがTrajectoryのためにGravityScaleを0.fにする
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	Character->GetCharacterMovement()->GravityScale = 0.0f;

	//! 掴まり動作のMontageを再生
	if (ClimbAnimMontage == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("ClimbAnimMontage is nullptr."));
		return false;
	}
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ClimbAnimMontage);

	//! MotionWarpingで移動
	if (MotionWarping == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("MotionWarping is nullptr."));
		return false;
	}
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(TargetName, ClimbTransform.GetLocation() + (ClimbWallNormal * AdjustValue), ClimbTransform.Rotator());

	return true;
}

//----------------------------------------------------------------------//
//
//! @brief Climb中の入力による移動
//
//! @param InputValue コントローラー3Dスティック入力値
//
//----------------------------------------------------------------------//
void UClimbActionComponent::InputMovement(const FVector2D& InputValue)
{
	IClimbActionInterface* ClimbInterface = Cast<IClimbActionInterface>(GetOwner());
	if (ClimbInterface == nullptr)
	{
		return;
	}
	ClimbInterface->ClimbMove(InputValue);
}

//----------------------------------------------------------------------//
//
//! @brief Climb中の移動先の判定
//
//! @retval true	進行方向に壁の端がある
//! @retval false	進行方向に進める
//
//! @param InputValue コントローラー3Dスティック入力値
//
//----------------------------------------------------------------------//
bool UClimbActionComponent::CheckWallEdge(const FVector2D& InputValue)
{
	//! CapsuleCompの情報取得
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return false;
	}
	float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	//! Playerから進行方向に判定
	FVector Start = GetOwner()->GetActorLocation();
	FVector InputVector = (GetOwner()->GetActorRightVector() * (InputValue.X * 100.0f)) + (GetOwner()->GetActorUpVector() * (InputValue.Y * 100.0f));
	FVector End = Start + InputVector;

	FHitResult HitResult = {};

	//! 掴まっている壁を判定しないように半径は小さくする(-5.0f)
	bool Result = UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(), Start, End,
		CapsuleRadius - 5.0f, CapsuleHalfHeight, TraceChannel,
		false, TArray<AActor*>(), /*EDrawDebugTrace::ForDuration*/EDrawDebugTrace::None,
		HitResult, true);
	if (Result == true)
	{
		//! 進行方向の直線状ある障害物の情報を格納
		ImpactWallLocation = HitResult.ImpactPoint;
		ImpactWallNormal = HitResult.ImpactNormal;
		return false;
	}

	//! Playerの前(Capsule1個分)を進行方向先から判定
	End = Character->GetActorLocation() + (Character->GetActorForwardVector() * (CapsuleRadius * 2.0f));
	Start = End + InputVector;

	Result = UKismetSystemLibrary::CapsuleTraceSingle(
		this, Start, End,
		CapsuleRadius, CapsuleHalfHeight, TraceChannel,
		false, TArray<AActor*>(), /*EDrawDebugTrace::ForDuration*/EDrawDebugTrace::None,
		HitResult, true
	);
	if (Result == true)
	{
		//! 折り返し先の壁の情報を格納
		ImpactWallLocation = HitResult.ImpactPoint;
		ImpactWallNormal = HitResult.ImpactNormal;
	}

	return Result;
}

//----------------------------------------------------------------------//
//
//! @brief 壁の端までの距離を確認
//
//! @retval true	壁端に近づいた
//! @retval false	壁の端から距離がある
//
//----------------------------------------------------------------------//
bool UClimbActionComponent::CheckDistanceToWall()
{
	//! CapsuleCompの情報取得
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return false;
	}
	float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	
	//! 壁端までの距離を計算する
	FVector EdgeLocation = ImpactWallLocation + ((Character->GetActorForwardVector() * -1.0f) * (CapsuleRadius * 2.0f));
	FVector VectorToWall = EdgeLocation - Character->GetActorLocation();

	float DistanceToWall = VectorToWall.Length();

	if (DistanceToWall > TurnDistance)
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------//
//
//! @brief 壁の向きを確認
//
//! @retval true	次の壁が上向き
//! @retval false	次の壁が上以外を向いている
//
//----------------------------------------------------------------------//
bool UClimbActionComponent::CheckWallDirection()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return false;
	}
	//! 衝突した壁の法線とPlayerの上方向のVectorのとの内積から角度を求める
	float Dot = UKismetMathLibrary::Dot_VectorVector(ImpactWallNormal, Character->GetActorUpVector());
	float Degree = UKismetMathLibrary::DegAcos(Dot);

	if (Degree > UpDifferenceAngle)
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------//
//
//! @brief 角を曲がる
//
//----------------------------------------------------------------------//
void UClimbActionComponent::Turn()
{
	//! 掴んでいる法線を更新
	ClimbWallNormal = ImpactWallNormal;

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return;
	}
	float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	//! 衝突判定の取れた位置にTransformで移動
	FRotator Rotator = UKismetMathLibrary::MakeRotFromX(ImpactWallNormal * -1.0f);
	//! 壁の法線方向にCapsule1個分 + 掴まる際調整した分移動させる
	FVector Location = ImpactWallLocation + (ImpactWallNormal * AdjustValue);
	Character->SetActorTransform(FTransform(Rotator, Location, FVector(1.0f, 1.0f, 1.0f)), false, nullptr, ETeleportType::TeleportPhysics);
}

//----------------------------------------------------------------------//
//
//! @brief 上に登る
//
//----------------------------------------------------------------------//
void UClimbActionComponent::GoUp()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(ClimbComp, Warning, TEXT("Could not cast in ACharacter."));
		return;
	}
	float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	Character->SetActorLocation(ImpactWallLocation + (ImpactWallNormal * CapsuleHalfHeight));

	//! Cilmbを終了する
	CanselAction();
}

