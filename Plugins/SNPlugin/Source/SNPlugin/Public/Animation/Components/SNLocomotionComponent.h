// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SNLocomotionComponent.generated.h"

class UCurveVector;

UCLASS( Blueprintable, meta=(BlueprintSpawnableComponent) )
class SNPLUGIN_API USNLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USNLocomotionComponent();

	float GetMaxSpeed() const ;
	
	void MoveGround(const FVector2D& Vector);
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	//!< 最大スピードの設定(細かく設定するほど滑りがなくなる。結局はブレンドスペースの設定に合わせてやるべき)
	UPROPERTY(EditAnywhere, Category="Locomotion|Settings")
	TArray<float> MaxSpeed;

	UPROPERTY(EditAnywhere, Category="Locomotion|Settings")
	TObjectPtr<UCurveVector> MovementCurve = nullptr;
};
