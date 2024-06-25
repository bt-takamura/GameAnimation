// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action/SNActionBase.h"
#include "GALookAction.generated.h"

/**
 * Camera Input
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UGALookAction : public USNActionBase
{
	GENERATED_BODY()

public:
	virtual void ExecAction(const FInputActionValue& InputActionValue) override;
};
