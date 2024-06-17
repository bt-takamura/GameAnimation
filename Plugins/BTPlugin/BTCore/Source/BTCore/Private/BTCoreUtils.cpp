// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTCoreUtils.h"

#include "BTCoreWorldSubsystem.h"

IBTCoreTickableInterface* FBTCoreUtils::GetTickableInterface(TObjectPtr<UObject> WorldContextObject, const FName& InInterfaceType)
{
	if (!IsValid(WorldContextObject))
	{
		return nullptr;
	}

	if (UBTCoreWorldSubsystem* Subsystem = UBTCoreWorldSubsystem::GetWorldSubsystemInternal(WorldContextObject->GetWorld()))
	{
		return Subsystem->GetTickableInterface(InInterfaceType);
	}

	return nullptr;
}