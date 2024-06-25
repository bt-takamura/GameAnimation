// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action/SNActionBase.h"
#include "GAAimAction.generated.h"

/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UGAAimAction : public USNActionBase
{
	GENERATED_BODY()

public:
	
	virtual void ExecAction(const FInputActionValue& InputActionValue) override;
};
