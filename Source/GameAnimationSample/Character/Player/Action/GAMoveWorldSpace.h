// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action/SNActionBase.h"
#include "GAMoveWorldSpace.generated.h"

/**
 * Movement Input - World Space (Useful for locomotion testing)
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UGAMoveWorldSpace : public USNActionBase
{
	GENERATED_BODY()

public:
	virtual void ExecAction(const FInputActionValue& InputActionValue) override;
};
