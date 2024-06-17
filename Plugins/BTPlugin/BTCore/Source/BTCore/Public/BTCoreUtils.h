// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BTCoreTickableInterface.h"

class BTCORE_API FBTCoreUtils
{
public:
	template<typename TTickableInterfaceClass = IBTCoreTickableInterface>
	static TTickableInterfaceClass* GetTickableInterface(TObjectPtr<UObject> WorldContextObject)
	{
		return static_cast<TTickableInterfaceClass*>(GetTickableInterface(WorldContextObject, TTickableInterfaceClass::GetStaticId()));
	}
private:
	static IBTCoreTickableInterface* GetTickableInterface(TObjectPtr<UObject> WorldContextObject, const FName& InInterfaceType);
};
