// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GATraversalBlock.generated.h"

class USplineComponent;

UCLASS()
class GAMEANIMATIONSAMPLE_API AGATraversalBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGATraversalBlock();
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="Default")
	USplineComponent* FindClosestLedgeToActor(const FVector& ActorLocation);
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable)
	void GetLedgeTransform(FVector HitLocation, FVector ActorLocation, UPARAM(ref) FTraversalCheckResult& TraversalTraceResultInOut);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Ledge")
	TArray<USplineComponent*> Ledges;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Ledge")
	TMap<USplineComponent*,USplineComponent*> OppositeLedges;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	float MinLedgeWidth = 60.0f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	
	//!< メッシュ用アクターコンポーネント
	UPROPERTY(EditAnywhere, Category="GA|Mesh")
	UStaticMeshComponent* StaticMeshComponent = nullptr;
};
