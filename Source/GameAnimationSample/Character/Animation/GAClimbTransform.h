// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GAClimbTransform.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UGAClimbTransform : public UInterface
{
	GENERATED_BODY()
};

/**
 * Climbで掴まり位置を扱う
 */
class GAMEANIMATIONSAMPLE_API IGAClimbTransform
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//! @brief Climbで掴まる位置のTransformを設定
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (BlueprintThreadSafe))
	void SetClimbTransform(const FTransform& Transform);

	//! @brief Climbで掴まる位置のTransformを取得
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (BlueprintThreadSafe))
	const FTransform GetClimbTransform() const;

};
