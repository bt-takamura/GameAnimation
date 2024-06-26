// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GAInterfactionTransform.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UGAInterfactionTransform : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEANIMATIONSAMPLE_API IGAInterfactionTransform
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** Please add a function description */
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	virtual void SetInteractTransform(const FTransform& InteractionTransform) = 0;

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	virtual const FTransform& GetInteractTransform() const = 0;
};
