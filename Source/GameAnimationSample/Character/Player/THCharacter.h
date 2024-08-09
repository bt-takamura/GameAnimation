// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAnimationSample/Character/Player/MotionMatcingCharacterBase.h"
#include "GameAnimationSample/Character/Player/Interface/ClimbActionInterface.h"
#include "THCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UClimbActionComponent;

/** カメラ設定パラメータ */
USTRUCT(BlueprintType)
struct FCameraParams
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SpringArmLength = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector SocketOffset = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TranslationLagSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FieldOfView = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TransitionSpeed = 0.0f;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GAMEANIMATIONSAMPLE_API ATHCharacter : public AMotionMatcingCharacterBase, public IClimbActionInterface
{
	GENERATED_BODY()
	
public:
	//! @{@name デフォルトコンストラクタ
	ATHCharacter(const FObjectInitializer& ObjectInitializer);
	//! @}
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//! @{@name スプリングアームコンポーネントの取得
	USpringArmComponent* GetSpringArmComponent() const;
	//! @}
	
	//! @{@name カメラコンポーネントの取得
	UCameraComponent* GetCameraComponent() const;
	//! @}

	//! @{@name クライムアクション機能コンポーネントの取得
	UClimbActionComponent* GetClimbActionComponent() const;
	//! @}

	//! @note IClimbActionInterface
	virtual void SetClimbGraspTransform(FHitResult& HitResult, FTransform& OutTransform) override;

	//! @note IClimbActionInterface
	virtual void ClimbMove(const FVector2D& InputValue) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//! @{@name カメラの更新
	void UpdateCamera(bool bInterpolate);
	//! @}

	//!< スプリングアームコンポーネント
	UPROPERTY(EditDefaultsOnly, Category = "TH | Default")
	TObjectPtr<USpringArmComponent> SpringArmComponent = nullptr;

	//!< カメラコンポーネント
	UPROPERTY(EditDefaultsOnly, Category = "TH | Default")
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;

	//!< 遠くのカメラ設定
	UPROPERTY(EditDefaultsOnly, Category = "TH | Camera")
	FCameraParams CamStyleFar;
	
	//!< 近くのカメラ設定
	UPROPERTY(EditDefaultsOnly, Category = "TH | Camera")
	FCameraParams CamStyleClose;
	
	//!< 注視のカメラ設定
	UPROPERTY(EditDefaultsOnly, Category = "TH | Camera")
	FCameraParams CamStyleAim;

	//!< カメラ距離の大きさ
	UPROPERTY(EditDefaultsOnly, Category = "TH | Camera")
	float CameraDistanceMag = 1.0f;

	//!< クライム(よじ登る)アクション機能のコンポーネント
	UPROPERTY(EditDefaultsOnly, Category = "TH | Climb")
	TObjectPtr<UClimbActionComponent> ClimbActionComponent = nullptr;

	//!< 掴まる位置の高さ
	UPROPERTY(EditDefaultsOnly, Category = "TH | Climb")
	float ClimbHeight = 100.0f;

};

//----------------------------------------------------------------------//
//
//! @brief スプリングアームコンポーネントを取得
//
//! @retval スプリングアームコンポーネント
//
//----------------------------------------------------------------------//
FORCEINLINE USpringArmComponent* ATHCharacter::GetSpringArmComponent() const
{
	return SpringArmComponent;
}

//----------------------------------------------------------------------//
//
//! @brief カメラコンポーネントを取得
//
//! @retval カメラコンポーネント
//
//----------------------------------------------------------------------//
FORCEINLINE UCameraComponent* ATHCharacter::GetCameraComponent() const
{
	return CameraComponent;
}

//----------------------------------------------------------------------//
//
//! @brief クライムアクション機能コンポーネントを取得
//
//! @retval クライムアクション機能コンポーネント
//
//----------------------------------------------------------------------//
FORCEINLINE UClimbActionComponent* ATHCharacter::GetClimbActionComponent() const
{
	return ClimbActionComponent;
}
