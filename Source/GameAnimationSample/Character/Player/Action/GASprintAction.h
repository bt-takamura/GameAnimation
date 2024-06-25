// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action/SNActionBase.h"
#include "GASprintAction.generated.h"

/**
 * Sprint Input (Coming in 5.5)
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UGASprintAction : public USNActionBase
{
	GENERATED_BODY()

public:
	virtual void ExecAction(const FInputActionValue& InputActionValue) override;
};
