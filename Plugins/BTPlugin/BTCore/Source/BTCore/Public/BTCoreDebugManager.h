// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BTCoreTickableInterface.h"

#if !UE_BUILD_SHIPPING

class BTCORE_API FBTCoreDebugManager final : public IBTCoreTickableInterface
{
	BTCORE_DECLARE_TICKABLE_ID(FBTCoreDebugManager);
public:
	explicit FBTCoreDebugManager(TObjectPtr<class UWorld> InOwnerWorld, int32 InPriority)
		: IBTCoreTickableInterface(InOwnerWorld, InPriority)
	{}
public:
	virtual bool Initialize() override;
	virtual void Finalize() override;

	virtual void Tick(float DeltaTime, bool bIsPaused) override;
private:
};

#endif