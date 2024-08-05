// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClimbActionComponent.generated.h"

class UMotionWarpingComponent;

DECLARE_LOG_CATEGORY_EXTERN(ClimbComp, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEANIMATIONSAMPLE_API UClimbActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClimbActionComponent(const FObjectInitializer& Initializer);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//! @{@name Climbを試みる、壁に掴まる
	UFUNCTION(BlueprintCallable)
	UPARAM(DisplayName = "ClimbSuccess")bool TryAction(UMotionWarpingComponent* MotionWarping = nullptr);
	//! @}

	//! @{@name Climb中の移動
	UFUNCTION(BlueprintCallable)
	void MoveAction(const FVector2D& InputValue);
	//! @}

	//! @{@name Climb動作を終了する
	UFUNCTION(BlueprintCallable)
	UPARAM(DisplayName = "CanselSuccess")bool CanselAction();
	//! @}

	//! @{@name Climbアクション中か確認
	UFUNCTION(BlueprintCallable)
	bool GetIsClimb() const;
	//! @}

	//! @{@name EMovementMode::MOVE_Falling時の最高移動速度を求める
	UFUNCTION(BlueprintCallable)
	float CalcMaxFlySpeed() const;
	//! @}

	//! @{@name Climb中のCharacterの回転設定
	UFUNCTION(BlueprintCallable)
	void SetClimbRotation() const;
	//! @}
	
	//! @{@name
	UFUNCTION(BlueprintCallable)
	void ResetVelocity();
	//! @}
	
private:
	//! @{@name 前方にオブジェクトがあるか判定する
	bool FindObjectInFront();
	//! @}

	//! @{@name 壁を掴む
	bool GraspWall(UMotionWarpingComponent* MotionWarping);
	//! @}

	//! @{@name Climb中の入力による移動
	void InputMovement(const FVector2D& InputValue);
	//! @}

	//! @{@name Climb中の移動先の判定
	bool CheckWallEdge(const FVector2D& InputValue);
	//! @}

	//! @{@name 壁の端までの距離を確認
	bool CheckDistanceToWall();
	//! @}

	//! @{@name 壁の向きを確認
	bool CheckWallDirection();
	//! @}

	//! @{@name 角を曲がる
	void Turn();
	//! @}

	//! @{@name 上に登る
	void GoUp();
	//! @}


	//!< トレース判定する距離
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance = 50.0f;

	//!< トレースするタイプ
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;

	//!< 掴まる位置の調整値
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float AdjustValue = 45.0f;

	//!< 次の壁に移動するまでの距離
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float TurnDistance = 50.0f;

	//!< 曲がる必要がある壁の向きと掴まっている壁の向きの差の角度(Degree)
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SideDifferenceAngle = 15.0f;

	//!< 上に登る壁の向きとPlayerのUpVectorの差の角度(Degree)
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float UpDifferenceAngle = 15.0f;

	//!< AnimationMontage
	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	const TObjectPtr<UAnimMontage> ClimbAnimMontage = nullptr;

	//!< Climb中の速度のマッピングカーブ
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	TObjectPtr<UCurveFloat> SpeedMapCurve = nullptr;

	//!< Climb中の最高速度設定値
	//! @brief X = 最大値 / Y = 中間値 / Z = 最小値
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector ClimbSpeeds = FVector::ZeroVector;

	//!< MotionWarpingのTargetName
	const FName TargetName = TEXT("Climb");

	//!< Climb中のフラグ
	bool IsClimb = false;

	//!< 壁の端があるかのフラグ
	bool IsClimbWallEdge = false;

	//!< 掴まった壁の法線
	FVector ClimbWallNormal = FVector::ZeroVector;

	//!< 掴まる時の姿勢
	FTransform ClimbTransform;

	//!< 衝突判定の取れたの壁の座標
	FVector ImpactWallLocation = FVector::ZeroVector;

	//!< 衝突判定の取れたの壁の法線
	FVector ImpactWallNormal = FVector::ZeroVector;

};

//----------------------------------------------------------------------//
//
//! @brief Climbアクション中か確認
//
//! @retval true	Climb中
//! @retval false	Climbしていない
//
//----------------------------------------------------------------------//
FORCEINLINE bool UClimbActionComponent::GetIsClimb() const
{
	return IsClimb;
}

