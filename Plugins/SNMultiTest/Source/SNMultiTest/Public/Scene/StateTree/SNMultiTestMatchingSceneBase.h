// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTree/SNSceneStateTreeTaskBase.h"
#include "SNMultiTestMatchingSceneBase.generated.h"

/**
 * 
 */
UCLASS()
class SNMULTITEST_API USNMultiTestMatchingSceneBase : public USNSceneStateTreeTaskBase
{
	GENERATED_BODY()

public:
	
	static void SetSessionName(const FName& Name);

	static FName GetSessionName();
	
protected:
	
	bool bExit = false;

private:

	static FName MatchingSessionName;
};

FORCEINLINE void USNMultiTestMatchingSceneBase::SetSessionName(const FName& Name)
{
	MatchingSessionName = Name;
}

FORCEINLINE FName USNMultiTestMatchingSceneBase::GetSessionName()
{
	return MatchingSessionName;
}

