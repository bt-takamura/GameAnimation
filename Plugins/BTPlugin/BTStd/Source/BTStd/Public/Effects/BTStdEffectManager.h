// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BTCoreTickableInterface.h"

class BTSTD_API FBTStdEffectManager final : public IBTCoreTickableInterface
{
	BTCORE_DECLARE_TICKABLE_ID(FBTStdEffectManager);

public:
	static const int32 PRIORITY = IBTCoreTickableInterface::DEFAULT_PRIORITY - 2;
public:
	explicit FBTStdEffectManager(TObjectPtr<class UWorld> InOwnerWorld, int32 InPriority)
		: IBTCoreTickableInterface(InOwnerWorld, InPriority)
	{}
public:
	virtual bool Initialize() override;
	virtual void Finalize() override;

	virtual void Tick(float DeltaTime, bool bIsPaused) override;
private:
};