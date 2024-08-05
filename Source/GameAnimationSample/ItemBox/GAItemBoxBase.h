// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAItemDef.h"
#include "GameFramework/Actor.h"
#include "GAItemBoxBase.generated.h"

class UArrowComponent;

UCLASS()
class GAMEANIMATIONSAMPLE_API AGAItemBoxBase : public AActor
{
	GENERATED_BODY()
	
public:	
	//! @{@name �f�t�H���g�R���X�g���N�^
	AGAItemBoxBase();
	//! @}
	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OpenRequest();

	UFUNCTION(BlueprintCallable)
	void CloseRequest();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Open();

	UFUNCTION()
	void Close();

	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, Category="ItemBox")
	float Speed = 1.0f;
	
	UPROPERTY()
	FName Id = NAME_None;
	
	UPROPERTY()
	bool IsOpen = false;
	
	UPROPERTY()
	TEnumAsByte<EItemType> ItemType = EItemType_None;

	UPROPERTY()
	int Score = -1;

	UPROPERTY(EditAnywhere, Category="ItemBox")
	TObjectPtr<UArrowComponent> LidPivotComponent = nullptr;

	UPROPERTY(EditAnywhere, Category="ItemBox")
	TObjectPtr<UStaticMeshComponent> ItemBoxBaseComponent = nullptr;

	UPROPERTY(EditAnywhere, Category="ItemBox")
	TObjectPtr<UStaticMeshComponent> ItemBoxLidComponent = nullptr;
};
