// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ClimbActionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UClimbActionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEANIMATIONSAMPLE_API IClimbActionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//! @{@name Climb開始時の掴まる姿勢の設定
	//! @param HitResult	TryAction時の衝突判定の結果
	//! @param OutTransform 設定する掴まる姿勢情報
	virtual void SetClimbGraspTransform(FHitResult& HitResult, FTransform& OutTransform) = 0;
	//! @}

	//! @{@name Climb中の移動処理
	//! @param InputValue	コントローラー3Dスティック入力値
	virtual void ClimbMove(const FVector2D& InputValue) = 0;
	//! @}
};
