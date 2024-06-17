// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Scene/SNSceneBase.h"
#include "SNMultiTestSceneDirector.generated.h"

UCLASS()
class SNMULTITEST_API ASNMultiTestSceneDirector : public ASNSceneBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASNMultiTestSceneDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
