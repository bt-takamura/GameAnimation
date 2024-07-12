// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GAClimbCharacter.generated.h"

class UMotionWarpingComponent;

DECLARE_LOG_CATEGORY_EXTERN(Climb, Log, All);

UCLASS()
class GAMEANIMATIONSAMPLE_API AGAClimbCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGAClimbCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//! @brief Climbフラグの取得
	UFUNCTION(BlueprintCallable)
	bool GetIsClimb() const { return IsClimb; }
	//! @brief Climbを試みる
	UFUNCTION(BlueprintCallable)
	UPARAM(DisplayName = "ClimbSuccess")bool TryClimbAction(UMotionWarpingComponent* MotionWarping = nullptr);
	UFUNCTION(BlueprintCallable)
	bool CanselClimb();

private:
	//! @brief 前方にオブジェクトがあるか判定する
	bool FindObjectInFront();
	//! @brief Climbを行う
	bool ClimbAction(UMotionWarpingComponent* MotionWarping);

	//! @brief トレースするタイプ
	UPROPERTY(EditDefaultsOnly, Category="Climb")
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;
	//! @brief トレース判定する距離
	UPROPERTY(EditDefaultsOnly, Category="Climb")
	float TraceDistance = 50.0f;
	//! @brief MotionWarpingのTargetName
	UPROPERTY(EditDefaultsOnly,Category="Climb")
	FName TargetName = TEXT("");
	//! @brief 掴まる位置の高さ
	UPROPERTY(EditDefaultsOnly,Category="Climb")
	float Height = 100.0f;
	//! @brief 掴まる位置の調整値
	UPROPERTY(EditDefaultsOnly, Category = "Climb")
	float AdjustValue = 20.0f;
	//! @brief AnimationMontage
	UPROPERTY(EditDefaultsOnly,Category="Climb")
	TObjectPtr<UAnimMontage> ClimbAnimMontage = nullptr;

	//!@brief Climb中のフラグ
	bool IsClimb;
	//! @brief 掴まる時の姿勢
	FTransform ClimbTransform;
	

};
