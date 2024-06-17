// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BTCoreTickableInterface.h"

class FBTCoreTickableEntry : public TSharedFromThis<FBTCoreTickableEntry>
{
public:
	DECLARE_DELEGATE_RetVal_TwoParams(TSharedPtr<IBTCoreTickableInterface>, FOnSpawnTickableInterface, TObjectPtr<UWorld>, int32/*InPriority*/);
public:
	FBTCoreTickableEntry(const FName& InInterfaceType, const FOnSpawnTickableInterface& InSpawnMethod, int32 InPriority)
		: m_InterfaceType(InInterfaceType)
		, m_OnSpawnTickableInterface(InSpawnMethod)
		, m_Priority(InPriority)
	{}

	TSharedPtr<FBTCoreTickableEntry> AsSpawnerEntry()
	{
		return SharedThis(this);
	}

	const FName GetInterfaceType() const
	{
		return m_InterfaceType;
	};

	FOnSpawnTickableInterface& GetSpawnAction()
	{
		return m_OnSpawnTickableInterface;
	}

	int32 GetPriority() const
	{
		return m_Priority;
	}

private:
	FName m_InterfaceType;
	FOnSpawnTickableInterface m_OnSpawnTickableInterface;
	int32 m_Priority = -1;
};

class BTCORE_API FBTCoreTickableSpawnerManager : public TSharedFromThis<FBTCoreTickableSpawnerManager>
{
	friend class IBTCoreTickableInterface;
	friend class UBTCoreWorldSubsystem;
public:
	static const TSharedRef<FBTCoreTickableSpawnerManager>& Get();

private:
	static TSharedRef<FBTCoreTickableSpawnerManager> New()
	{
		return MakeShareable(new FBTCoreTickableSpawnerManager());
	}

public:
	template<typename TTickableInterfaceClass = IBTCoreTickableInterface>
	FBTCoreTickableEntry& RegisterTickableInterface(const FBTCoreTickableEntry::FOnSpawnTickableInterface& OnSpawnInterface, int32 InPriority = IBTCoreTickableInterface::DEFAULT_PRIORITY)
	{
		return RegisterTickableInterfaceInternal(TTickableInterfaceClass::GetStaticId(), OnSpawnInterface, InPriority);
	}

	template<typename TTickableInterfaceClass = IBTCoreTickableInterface>
	bool UnregisterTickableInterface()
	{
		return UnregisterTickableInterfaceInternal(TTickableInterfaceClass::GetStaticId());
	}

	void UnregisterAllTickableInterfaces();

private:
	FBTCoreTickableEntry& RegisterTickableInterfaceInternal(const FName& InInterfaceType, const FBTCoreTickableEntry::FOnSpawnTickableInterface& OnSpawnInterface, int32 InPriority = 0);
	bool UnregisterTickableInterfaceInternal(FName InInterfaceType);
private:
	TSharedPtr<IBTCoreTickableInterface> CreateTickableInterface(FName InInterfaceType, TObjectPtr<UWorld> InWorld);
private:
	typedef TMap<FName, TSharedRef<FBTCoreTickableEntry>> FTickableSpawner;
	FTickableSpawner TickableSpawner;

	friend class UBTCoreWorldSubsystem;
};