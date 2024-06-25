// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action/SNActionBase.h"
#include "GALookGamepadAction.generated.h"

/**
 * Camera Input - Gamepad
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UGALookGamepadAction : public USNActionBase
{
	GENERATED_BODY()

public:
	virtual void ExecAction(const FInputActionValue& InputActionValue) override;
};
