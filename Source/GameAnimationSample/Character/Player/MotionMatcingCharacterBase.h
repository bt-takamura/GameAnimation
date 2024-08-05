// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MotionMatcingCharacterBase.generated.h"

class UMMLocomotionComponent;
class UMotionWarpingComponent;

/**
 *	モーションマッチング機能を使用するキャラクターのベースクラス
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API AMotionMatcingCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMotionMatcingCharacterBase(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//! @{@name モーションマッチング移動機能コンポーネントの取得
	UMMLocomotionComponent* GetMMLocomotionComponent() const;
	//! @}

	//! @{@name モーションワーピングコンポーネントの取得
	UMotionWarpingComponent* GetMotionWarpingComponent() const;
	//! @}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//! @{@name 移動速度を更新
	virtual void UpdateMovement();
	//! @}

private:	
	//! @{@name 回転設定を更新
	void UpdateRotation();
	//! @}


	//!< モーションワーピングコンポーネント
	UPROPERTY(EditDefaultsOnly, Category = "Default")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent = nullptr;

	//!< モーションマッチングによる移動機能のコンポーネント
	UPROPERTY(EditDefaultsOnly, Category = "Default")
	TObjectPtr<UMMLocomotionComponent> MMLocomotionComponent = nullptr;
};

//----------------------------------------------------------------------//
//
//! @brief モーションマッチング移動機能コンポーネントを取得
//
//! @retval モーションマッチング移動機能コンポーネント
//
//----------------------------------------------------------------------//
FORCEINLINE UMMLocomotionComponent* AMotionMatcingCharacterBase::GetMMLocomotionComponent() const
{
	return MMLocomotionComponent;
}

//----------------------------------------------------------------------//
//
//! @brief モーションワーピングコンポーネントを取得
//
//! @retval モーションワーピングコンポーネント
//
//----------------------------------------------------------------------//
FORCEINLINE UMotionWarpingComponent* AMotionMatcingCharacterBase::GetMotionWarpingComponent() const
{
	return MotionWarpingComponent;
}

