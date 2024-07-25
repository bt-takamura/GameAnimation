// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameAnimationSample/Character/Player/Interface/ClimbActionInterface.h"
#include "GAClimbCharacter.generated.h"

class UClimbActionComponent;

UCLASS()
class GAMEANIMATIONSAMPLE_API AGAClimbCharacter : public ACharacter, public IClimbActionInterface
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

	//!< Climbの機能コンポーネント
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Climb")
	TObjectPtr<UClimbActionComponent> ClimbActionComponent = nullptr;

	//! @{@name Climb開始時に掴む姿勢の設定
	virtual void SetClimbGraspTransform(FHitResult& HitResult, FTransform& OutTransform) override;
	//! @}
	
	//! @{@name Climb中の移動処理
	virtual void ClimbMove(const FVector2D& InputValue) override;
	//! @}

private:
	//!< 掴まる位置の高さ
	UPROPERTY(EditDefaultsOnly, Category = "Climb")
	float ClimbHeight = 100.0f;
};
