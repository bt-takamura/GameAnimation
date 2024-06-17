// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/BTStdEffectUtils.h"

#include "BTCoreUtils.h"

#include "Effects/BTStdEffectManager.h"

FBTStdEffectManager* FBTStdEffectUtils::GetEffectManager(TObjectPtr<UObject> WorldContextObject)
{
	return FBTCoreUtils::GetTickableInterface<FBTStdEffectManager>(WorldContextObject);
}