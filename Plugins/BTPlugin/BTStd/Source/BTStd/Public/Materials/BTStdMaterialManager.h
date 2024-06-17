// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BTCoreTickableInterface.h"
#include "Materials/BTStdMaterialParamChanger.h"

class FBTStdMaterialManager final : public IBTCoreTickableInterface
{
	BTCORE_DECLARE_TICKABLE_ID(FBTStdMaterialManager);
public:
	static const int32 PRIORITY = IBTCoreTickableInterface::DEFAULT_PRIORITY - 1;
public:
	explicit FBTStdMaterialManager(TObjectPtr<class UWorld> InOwnerWorld, int32 InPriority)
		: IBTCoreTickableInterface(InOwnerWorld, InPriority)
	{}
public:
	FBTStdMaterialParamChangerBase* FindMaterialParamChanger(TObjectPtr<UObject> InTarget, const FName& InParameterName) const
	{
		if (const TMap<FName, TUniquePtr<FBTStdMaterialParamChangerBase>>* const NameToChangerMap = MaterialChangers.Find(InTarget))
		{
			if (const TUniquePtr<FBTStdMaterialParamChangerBase>* const Changer = NameToChangerMap->Find(InParameterName))
			{
				return Changer->Get();
			}
		}

		return nullptr;
	}

	void GetTargetList(TArray<TWeakObjectPtr<UObject>>& OutTargetList) const
	{
		MaterialChangers.GenerateKeyArray(OutTargetList);
	}

	const TMap<FName, TUniquePtr<FBTStdMaterialParamChangerBase>>* FindMaterialParamChangerMap(TObjectPtr<UObject> InTarget)
	{
		return MaterialChangers.Find(InTarget);
	}

	bool RestoreMaterialParam(TObjectPtr<UObject> Target, const FName& InParameterName)
	{
		if (FBTStdMaterialParamChangerBase* const Changer = FindMaterialParamChanger(Target, InParameterName))
		{
			Changer->Restore();
			return true;
		}
		return false;
	}

	bool StopMaterialParam(TObjectPtr<UObject> Target, const FName& InParameterName)
	{
		if (FBTStdMaterialParamChangerBase* const Changer = FindMaterialParamChanger(Target, InParameterName))
		{
			Changer->Abort();
			return true;
		}
		return false;
	}
public:
	BTSTD_API virtual bool Initialize() override;
	BTSTD_API virtual void Finalize() override;

	BTSTD_API virtual void Tick(float DeltaTime, bool bIsPaused) override;

private:
	FBTStdMaterialParamChangerHandle GetHandle(const FBTStdMaterialParamChangerBase* InChanger)
	{
		if (const uint64* Id = ChangerHandleMap.Find(InChanger))
		{
			return FBTStdMaterialParamChangerHandle(const_cast<FBTStdMaterialParamChangerBase*>(InChanger), *Id);
		}
		else
		{
			const uint64 NewId = MakeUniqueId();
			ChangerHandleMap.Emplace(const_cast<FBTStdMaterialParamChangerBase*>(InChanger), NewId);
			return FBTStdMaterialParamChangerHandle(const_cast<FBTStdMaterialParamChangerBase*>(InChanger), NewId);
		}
	}

	BTSTD_API FBTStdMaterialParamChangerBase* GetChanger(const FBTStdMaterialParamChangerHandle& InHandle);

	bool RemoveHandle(const FBTStdMaterialParamChangerBase* InChanger)
	{
		return ChangerHandleMap.Remove(InChanger) > 0;
	}

	void Reset()
	{
		ChangerHandleMap.Reset();
	}
private:
	BTSTD_API uint64 MakeUniqueId();
	uint64 UniqueId = 0;
private:

	using FNameToChangerMap = TMap<FName, TUniquePtr<FBTStdMaterialParamChangerBase>>;
	using FMaterialChangerMap = TMap <TWeakObjectPtr<UObject>, FNameToChangerMap>;
	FMaterialChangerMap MaterialChangers;
	TMap<FBTStdMaterialParamChangerBase*, uint64> ChangerHandleMap;
};