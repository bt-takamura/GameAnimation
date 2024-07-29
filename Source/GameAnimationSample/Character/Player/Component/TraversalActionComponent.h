// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAnimationSample/Character/Animation/GAMotionMatchingDef.h"
#include "Components/ActorComponent.h"
//! 一時的
#include "GameAnimationSample/Character/Player/GAPlayer.h"
//! 
#include "TraversalActionComponent.generated.h"

//USTRUCT()
//struct FGATraversalChooserParams
//{
//	GENERATED_BODY()
//
//public:
//	//!< トラバーサルアクションのタイプ
//	TEnumAsByte<ETraversalActionType> ActionType = ETraversalActionType::None;
//
//	//!< 移動状態
//	TEnumAsByte<EStride> Stride = EStride::Walk;
//
//	//!< 移動速度
//	float Speed = 0.0f;
//
//	//!< 障害物の高さ
//	float ObstacleHeight = 0.0f;
//
//	//!< 奥にある障害物との距離
//	float ObstacleDepth = 0.0f;
//	
//};
//
//USTRUCT()
//struct FTraversalCheckResult
//{
//	GENERATED_BODY()
//
//public:
//	//!< トラバーサルアクションのタイプ
//	TEnumAsByte<ETraversalActionType> ActionType = ETraversalActionType::None;
//
//	//!< 
//	bool HasFrontLedge = false;
//
//	//!< フロントレッジの位置
//	FVector FrontLedgeLocation = FVector::ZeroVector;
//
//	//!< フロントレッジの法線
//	FVector FrontLedgeNormal = FVector::ZeroVector;
//
//	//!< 
//	bool HasBackLedge = false;
//
//	//!< バックレッジの位置
//	FVector BackLedgeLocation = FVector::ZeroVector;
//
//	//!< バックレッジの法線
//	FVector BackLedgeNormal = FVector::ZeroVector;
//
//	//!< 
//	bool HasBackFloor = false;
//
//	//!< 
//	FVector BackFloorLocation = FVector::ZeroVector;
//
//	//!< 障害物の高さ
//	float ObstacleHeight = 0.0f;
//
//	//!< 奥にある障害物との距離
//	float ObstacleDepth = 0.0f;
//
//	//!< バックレッジの高さ
//	float BackLedgeHeight = 0.0f;
//
//	//!< 
//	TObjectPtr<UPrimitiveComponent> HitComponent;
//
//	//!< 選択されたモンタージュ
//	TObjectPtr<UAnimMontage> ChosenMontage;
//
//	//!< 再生開始時間
//	float StartTime = 0.0f;
//
//	//!< 再生速度倍率
//	float PlayRate = 1.0f;
//
//};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEANIMATIONSAMPLE_API UTraversalActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTraversalActionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//! @{@name トラバーサルアクション中かチェック
	UFUNCTION(BlueprintCallable, Category = "GA|Traversal")
	bool IsEnableTraversalAction() const;
	//! @}
	
	//! @{@name トラバーサルアクションを実行
	UFUNCTION(BlueprintCallable, Category = "GA|Traversal")
	void ExecTraversalAction(float TraceForwardDistance, bool& TraversalCheckFailed, bool& MontageSelectionFailed);
	//! @}
		
private:
	//! @{@name 進行方向にBlockがあるかチェック
	bool PerformFowardBlocks(FTraversalCheckResult& TraversalCheckResult, float TraceForwardDistance, int DrawDebugLevel, float DrawDebugDuration);
	//! @}
	
	//! @{@name レッジの座標に球を描画
	void DrawDebugShapesAtLedgeLocation(const FTraversalCheckResult& TraversalCheckResult, int DrawDebugLevel, float DrawDebugDuration);
	//! @}

	//!< トラバーサルアクション中か
	bool bDoingTraversalAction;
};

FORCEINLINE bool UTraversalActionComponent::IsEnableTraversalAction() const
{
	return bDoingTraversalAction;
}
