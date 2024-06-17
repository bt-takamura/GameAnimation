// Copyright Epic Games, Inc. All Rights Reserved.

#include "Materials/BTStdMaterialUtils.h"

#include "BTCoreUtils.h"

#include "Materials/BTStdMaterialManager.h"

FBTStdMaterialManager* FBTStdMaterialUtils::GetMaterialManager(TObjectPtr<UObject> WorldContextObject)
{
	return FBTCoreUtils::GetTickableInterface<FBTStdMaterialManager>(WorldContextObject);
}