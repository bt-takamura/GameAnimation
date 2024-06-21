// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GALevelBlock.generated.h"

class USplineComponent;

UCLASS(Blueprintable, BlueprintType)
class GAMEANIMATIONSAMPLE_API AGALevelBlock : public AActor
{
	GENERATED_BODY()
	public:
	// Sets default values for this actor's properties
	AGALevelBlock();

		/** Please add a function description */
		UFUNCTION(BlueprintCallable, Category="Default")
		USplineComponent* FindClosestLedgeToActor(const FVector& ActorLocation);

		/** Please add a function description */
		UFUNCTION(BlueprintCallable)
		void GetLedgeTransform(FVector HitLocation, FVector ActorLocation, UPARAM(ref) FTraversalCheckResult& TraversalTraceResultInOut);
		
		/** Please add a variable description */
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Default")
		TObjectPtr<USplineComponent> Ledge4;

		/** Please add a variable description */
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Default")
		TObjectPtr<USplineComponent> Ledge3;

		/** Please add a variable description */
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Default")
		TObjectPtr<USplineComponent> Ledge2;

		/** Please add a variable description */
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Default")
		TObjectPtr<USplineComponent> Ledge1;

		/** Please add a variable description */
		UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
		TArray<USplineComponent*> Ledges;

		/** Please add a variable description */
		UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
		TMap<USplineComponent*,USplineComponent*> OppositeLedges;

		/** Please add a variable description */
		UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
		float MinLedgeWidth = 60.0f;
};
