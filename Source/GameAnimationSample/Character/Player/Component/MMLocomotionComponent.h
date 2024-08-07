// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAnimationSample/Character/Animation/GAMotionMatchingDef.h"
#include "Components/ActorComponent.h"
#include "MMLocomotionComponent.generated.h"

class UChooserTable;

/** Chooserに設定する、評価に必要なパラメーター */
USTRUCT(BlueprintType)
struct FGATraversalChooserParams
{
	GENERATED_BODY()

public:
	//!< トラバーサルアクションのタイプ
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TEnumAsByte<ETraversalActionType> ActionType = ETraversalActionType::None;

	//!< 移動状態
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EStride> Stride = EStride::Walk;

	//!< 移動速度
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Speed = 0.0f;

	//!< 障害物の高さ
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ObstacleHeight = 0.0f;

	//!< 奥にある障害物との距離
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ObstacleDepth = 0.0f;
	
};

/** トラバーサルアクション判定の結果 */
USTRUCT(BlueprintType)
struct FTraversalCheckResult
{
	GENERATED_BODY()

public:
	//!< トラバーサルアクションのタイプ
	TEnumAsByte<ETraversalActionType> ActionType = ETraversalActionType::None;

	//!< 障害物にフロントレッジがあるか
	bool HasFrontLedge = false;

	//!< フロントレッジの位置
	FVector FrontLedgeLocation = FVector::ZeroVector;

	//!< フロントレッジの法線
	FVector FrontLedgeNormal = FVector::ZeroVector;

	//!< 障害物にバックレッジがあるか
	bool HasBackLedge = false;

	//!< バックレッジの位置
	FVector BackLedgeLocation = FVector::ZeroVector;

	//!< バックレッジの法線
	FVector BackLedgeNormal = FVector::ZeroVector;

	//!< 障害物にバックフロアがあるか
	bool HasBackFloor = false;

	//!< バックフロアの位置
	FVector BackFloorLocation = FVector::ZeroVector;

	//!< 障害物の高さ
	float ObstacleHeight = 0.0f;

	//!< 奥にある障害物との距離
	float ObstacleDepth = 0.0f;

	//!< バックレッジの高さ
	float BackLedgeHeight = 0.0f;

	//!< 衝突したコンポーネント
	TObjectPtr<UPrimitiveComponent> HitComponent;

	//!< 選択されたモンタージュ
	TObjectPtr<UAnimMontage> ChosenMontage;

	//!< 再生開始時間
	float StartTime = 0.0f;

	//!< 再生速度倍率
	float PlayRate = 1.0f;

};

/**
 *	モーションマッチングを使用した移動処理
 *	MM = MotionMatching
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEANIMATIONSAMPLE_API UMMLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMMLocomotionComponent(const FObjectInitializer& Initializer);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	//! @{@name 歩幅状態の取得
	EStride GetStride()const;
	//! @}

	//! @{@name 最大スピード値を算出
	float CalculateMaxSpeed() const;
	//! @}

	//! @{@name 歩き動作への移行フラグの取得
	bool GetWantsToWalk() const;
	//! @}
	
	//! @{@name 歩き動作への移行フラグの設定
	void SetWantsToWalk(bool Flag);
	//! @}
	 
	//! @{@name スプリント動作への移行フラグの取得
	bool GetWantsToSprint() const;
	//! @}
	
	//! @{@name スプリント動作への移行フラグの設定
	void SetWantsToSprint(bool Flag);
	//! @}
	
	//! @{@name ストレイフモードへの移行フラグの取得
	bool GetWantsToStrafe() const;
	//! @}
	
	//! @{@name ストレイフモードへの移行フラグの設定
	void SetWantsToStrafe(bool Flag);
	//! @}
	
	//! @{@name 視線を合わせる動作への移行フラグの取得
	bool GetWantsToAim() const;
	//! @}
	
	//! @{@name 視線を合わせる動作への移行フラグの設定
	void SetWantsToAim(bool Flag);
	//! @}
	
	//! @{@name 直前に着地したかチェック
	UFUNCTION(BlueprintCallable, Category = "MMLocomotion | Movement")
	bool IsJustLanded() const;
	//! @}
	
	//! @{@name 直前に着地したかのフラグ設定
	UFUNCTION(BlueprintCallable, Category = "MMLocomotion | Movement")
	void SetJustLanded(bool Flag);
	//! @}

	//! @{@name 着地速度を取得
	UFUNCTION(BlueprintCallable, Category = "MMLocomotion | Movement", meta = (BlueprintThreadSafe))
	FVector GetLandSpeed() const;
	//! @}
	
	//! @{@name 着地速度の設定
	UFUNCTION(BlueprintCallable, Category = "MMLocomotion | Movement", meta = (BlueprintThreadSafe))
	void SetLandSpeed(const FVector& Speed);
	//! @}

	//! @{@name スティックモードに合わせた入力値の取得
	FVector2D GetMovementInputScaleValue(const FVector2D& Input) const;
	//! 

	//! @{@name 歩幅状態を設定する
	void SetupStride(const FVector2D& Input);
	//! @}



	//! @{@name トラバーサルアクション中かチェック
	UFUNCTION(BlueprintCallable, Category = "MMLocomotion | Traversal")
	bool IsEnableTraversalAction() const;
	//! @}

	//! @{@name トラバーサルアクションを実行
	void ExecTraversalAction(float TraceForwardDistance, bool& TraversalCheckFailed, bool& MontageSelectionFailed);
	//! @}
	
	//! @{@name 前方にトレースする距離を算出
	float GetTraversalForwardTraceDistance() const;
	//! @}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	//! @{@name 着地時の処理
	UFUNCTION()
	void OnLandedDelegate(const FHitResult& Hit);
	//! @}

	//! @{@name 着地フラグを下す
	void TurnOffJustLandded();
	//! @}



	//! @{@name 進行方向にBlockがあるかチェック
	bool PerformForwardBlocks(FTraversalCheckResult& TraversalCheckResult, float TraceForwardDistance, int DrawDebugLevel, float DrawDebugDuration);
	//! @}

	//! @{@name レッジの座標に球を描画
	void DrawDebugShapesAtLedgeLocation(const FTraversalCheckResult& TraversalCheckResult, int DrawDebugLevel, float DrawDebugDuration);
	//! @}

	//! @{@name 障害物を乗り越えた場合の障害物の先の状態をチェックする
	bool PerformDecisionOnActorToEachEdge(FTraversalCheckResult& TraversalCheckResult, int DrawDebugLevel);
	//! @}

	//! @{@name アクタ位置からフロントレッジの位置までに移動スペースがあるかチェック
	bool PerformActorToFrontEdge(FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckFromLedgeLocation, int DrawDebugLevel);
	//! @}

	//! @{@name 障害物の上を手前から奥までに移動スペースがあるかチェック
	void PerformObstacleDepth(FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckFrontLedgeLocation, const FVector& HasRoomCheckBackLedgeLocation, int DrawDebugLevel);
	//! @}

	//! @{@name バックレッジの位置から下に向かって床を探す
	void PerformBackLedgeFloor(FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckBackLedgeLocation, int DrawDebugLevel);
	//! @}

	//! @{@name トラバーサルアクションのタイプを設定
	void DetermineTraversalAction(FTraversalCheckResult& TraversalCheckResult);
	//! @}

	//! @{@name トラバーサルアクションの判定結果をテキスト表示
	void DebugPrintTraversalResult(int DrawDebugLevel, FTraversalCheckResult& TraversalCheckResult);
	//! @}

	//! @{@name InterfaceでAnimBPにフロントレッジの情報を送る
	void SetInteractTransform(FTraversalCheckResult& TraversalCheckResult);
	//! @}

	//! @{@name トラバーサルアクションの判定結果に基づきチューザーを評価する
	TArray<UObject*> EvaluateChooser(FTraversalCheckResult& TraversalCheckResult);
	//! @}

	//! @{@name チューザーで選択されたモンタージュから最良のモンタージュを検索する
	bool PerformMotionMatch(TArray<UObject*> SearchAssets, FTraversalCheckResult& TraversalCheckResult);
	//! @}

	//! @{@name トラバーサルアクションを実行(モンタージュの再生)
	void ExecPerformTraversalAction(const FTraversalCheckResult& TraversalCheckResult);
	//! @}
	
	//! @{@name モンタージュの終了処理
	UFUNCTION()
	void EndPlayMontage(FName NotifyName);
	//! @}
	
	//! @{@name モーションワーピングの移動先を更新
	void UpdateWarpTarget();
	//! @}

private:
	//!< 歩幅設定
	TEnumAsByte<EStride> Stride = EStride::Run;

	//!< アナログスティック入力閾値
	UPROPERTY(EditDefaultsOnly, Category = "MMLocomotion | Input")
	float AnalogWalkRunThreshold = 0.7f;
	
	//!< 歩き動作への移行フラグ
	bool bWantsToWalk = false;
	
	//!< スプリント動作への移行フラグ
	bool bWantsToSprint = false;

	//!< ストレイフモードへの移行フラグ
	bool bWantsToStrafe = true;

	//!< 視線を合わせる動作への移行フラグ
	bool bWantsToAim = false;
	
	//!< 直前に着地したかどうか
	bool bJustLanded = false;

	//!< 移動のスティック入力モード
	TEnumAsByte<EAnalogueMovementBehavior> MovementStickMode = EAnalogueMovementBehavior::FixedSpeed_SingleStride;
	
	//!< カーブ
	UPROPERTY(EditDefaultsOnly, Category="MMLocomotion | Movement")
	TObjectPtr<UCurveFloat> StrafeSpeedMapCurveObject = nullptr;
	
	//!< 屈み状態の移動速度設定値
	//! @note X：最大 / Y : 中間 / Z : 最小
	//UPROPERTY(EditDefaultsOnly, Category="Movement")
	//FVector CrouchSpeed = FVector::ZeroVector;
	
	//!< 歩き状態の移動速度設定値
	//! @note X：最大 / Y : 中間 / Z : 最小
	UPROPERTY(EditDefaultsOnly, Category = "MMLocomotion | Movement")
	FVector WalkSpeed = FVector::ZeroVector;
	
	//!< 走り状態の移動速度設定値
	//! @note X：最大 / Y : 中間 / Z : 最小
	UPROPERTY(EditDefaultsOnly, Category = "MMLocomotion | Movement")
	FVector RunSpeed = FVector::ZeroVector;
	
	//!< スプリント状態の移動速度設定値
	//! @note X：最大 / Y : 中間 / Z : 最小
	UPROPERTY(EditDefaultsOnly, Category = "MMLocomotion | Movement")
	FVector SprintSpeed = FVector::ZeroVector;

	//!< 着地速度
	UPROPERTY(EditDefaultsOnly, Category = "MMLocomotion | Movement", meta = (BlueprintThreadSafe))
	FVector LandSpeed = FVector::ZeroVector;

	//!< タイマー用のハンドル
	FTimerHandle TimerHandle;



	//!< トラバーサルアクション中か
	bool bDoingTraversalAction = false;
	//!< トラバーサルアクション判定の結果
	FTraversalCheckResult TraversalResult;
	//!< チューザーテーブル
	UPROPERTY(EditDefaultsOnly, Category = "MMLocomotion | Traversal")
	TObjectPtr<UChooserTable> TraversalAnimationChooser = nullptr;

};

//----------------------------------------------------------------------//
//
//! @brief 歩幅設定の取得
//
//! @retval 歩幅設定の列挙型
//
//----------------------------------------------------------------------//
FORCEINLINE EStride UMMLocomotionComponent::GetStride() const
{
	return Stride;
}

//----------------------------------------------------------------------//
//
//! @brief 歩き動作への移行フラグの取得
//
//! @retval true	有効
//! @retval false	無効
//
//----------------------------------------------------------------------//
FORCEINLINE bool UMMLocomotionComponent::GetWantsToWalk() const
{
	return bWantsToWalk;
}

//----------------------------------------------------------------------//
//
//! @brief 歩き動作への移行フラグの設定
//
//! @param	Flag	設定するbool値
//
//----------------------------------------------------------------------//
FORCEINLINE void UMMLocomotionComponent::SetWantsToWalk(bool Flag)
{
	bWantsToWalk = Flag;
}

//----------------------------------------------------------------------//
//
//! @brief スプリント動作への移行フラグの取得
//
//! @retval true	有効
//! @retval false	無効
//
//----------------------------------------------------------------------//
FORCEINLINE bool UMMLocomotionComponent::GetWantsToSprint() const
{
	return bWantsToSprint;
}

//----------------------------------------------------------------------//
//
//! @brief スプリント動作への移行フラグの設定
//
//! @param	Flag	設定するbool値
//
//----------------------------------------------------------------------//
FORCEINLINE void UMMLocomotionComponent::SetWantsToSprint(bool Flag)
{
	bWantsToSprint = Flag;
}

//----------------------------------------------------------------------//
//
//! @brief ストレイフモードへの移行フラグの取得
//
//! @retval true	有効
//! @retval false	無効
//
//----------------------------------------------------------------------//
FORCEINLINE bool UMMLocomotionComponent::GetWantsToStrafe() const
{
	return bWantsToStrafe;
}

//----------------------------------------------------------------------//
//
//! @brief ストレイフモードへの移行フラグの設定
//
//! @param	Flag	設定するbool値
//
//----------------------------------------------------------------------//
FORCEINLINE void UMMLocomotionComponent::SetWantsToStrafe(bool Flag)
{
	bWantsToStrafe = Flag;
}

//----------------------------------------------------------------------//
//
//! @brief 視線を合わせる動作への移行フラグの取得
//
//! @retval true	有効
//! @retval false	無効
//
//----------------------------------------------------------------------//
FORCEINLINE bool UMMLocomotionComponent::GetWantsToAim() const
{
	return bWantsToAim;
}

//----------------------------------------------------------------------//
//
//! @brief 視線を合わせる動作への移行フラグの設定
//
//! @param	Flag	設定するbool値
//
//----------------------------------------------------------------------//
FORCEINLINE void UMMLocomotionComponent::SetWantsToAim(bool Flag)
{
	bWantsToAim = Flag;
}

//----------------------------------------------------------------------//
//
//! @brief 直前に着地したかチェック
//
//! @retval true	直前に着地した
//! @retval false	直前に着地していない
//
//----------------------------------------------------------------------//
FORCEINLINE bool UMMLocomotionComponent::IsJustLanded() const
{
	return bJustLanded;
}

//----------------------------------------------------------------------//
//
//! @brief 直前に着地したかのフラグ設定
//
//! @param	Flag	設定するbool値
//
//----------------------------------------------------------------------//
FORCEINLINE void UMMLocomotionComponent::SetJustLanded(bool Flag)
{
	bJustLanded = Flag;
}

//----------------------------------------------------------------------//
//
//! @brief 着地速度の取得
//
//! @retval 着地速度
//
//----------------------------------------------------------------------//
FORCEINLINE FVector UMMLocomotionComponent::GetLandSpeed() const
{
	return LandSpeed;
}

//----------------------------------------------------------------------//
//
//! @brief 着地速度の設定
//
//! @param Speed	設定する着地速度
//
//----------------------------------------------------------------------//
FORCEINLINE void UMMLocomotionComponent::SetLandSpeed(const FVector& Speed)
{
	LandSpeed = Speed;
}

//----------------------------------------------------------------------//
//
//! @brief トラバーサルアクション中かチェック
//
//! @retval true  トラバーサル中
//! @retval false トラバーサルしていない
//
//----------------------------------------------------------------------//
FORCEINLINE bool UMMLocomotionComponent::IsEnableTraversalAction() const
{
	return bDoingTraversalAction;
}
