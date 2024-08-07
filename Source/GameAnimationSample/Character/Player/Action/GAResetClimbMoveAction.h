// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action/SNActionBase.h"
#include "GAResetClimbMoveAction.generated.h"

/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UGAResetClimbMoveAction : public USNActionBase
{
	GENERATED_BODY()
	
public:
	virtual void ExecAction(const FInputActionValue& InputActionValue) override;

};
