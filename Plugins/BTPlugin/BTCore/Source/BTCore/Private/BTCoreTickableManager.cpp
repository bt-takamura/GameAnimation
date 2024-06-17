// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTCoreTickableManager.h"

const  TSharedRef<FBTCoreTickableSpawnerManager>& FBTCoreTickableSpawnerManager::Get()
{
	static const TSharedRef<FBTCoreTickableSpawnerManager> Instance = FBTCoreTickableSpawnerManager::New();
	static const TSharedRef<FBTCoreTickableSpawnerManager>* NeverDestroyManager = new TSharedRef<FBTCoreTickableSpawnerManager>(Instance);
	return Instance;
}

void FBTCoreTickableSpawnerManager::UnregisterAllTickableInterfaces()
{
	TickableSpawner.Empty();
}

FBTCoreTickableEntry& FBTCoreTickableSpawnerManager::RegisterTickableInterfaceInternal(const FName& InInterfaceType, const FBTCoreTickableEntry::FOnSpawnTickableInterface& OnSpawnInterface, int32 InPriority)
{
	ensure(InInterfaceType != NAME_None);
	ensure(!TickableSpawner.Contains(InInterfaceType));

	TSharedRef<FBTCoreTickableEntry> NewSpawnerEntry = MakeShareable(new FBTCoreTickableEntry(InInterfaceType, OnSpawnInterface, InPriority));
	TickableSpawner.Emplace(InInterfaceType, NewSpawnerEntry);
	return NewSpawnerEntry.Get();
}

bool FBTCoreTickableSpawnerManager::UnregisterTickableInterfaceInternal(FName InInterfaceType)
{
	ensure(InInterfaceType != NAME_None);
	return TickableSpawner.Remove(InInterfaceType) > 0;
}

TSharedPtr<IBTCoreTickableInterface> FBTCoreTickableSpawnerManager::CreateTickableInterface(FName InInterfaceType, TObjectPtr<UWorld> InWorld)
{
	ensure(InInterfaceType != NAME_None);

	if (TSharedRef<FBTCoreTickableEntry>* EntryPtr = TickableSpawner.Find(InInterfaceType))
	{
		TSharedRef<FBTCoreTickableEntry>& Entry = *EntryPtr;
		if (Entry->GetSpawnAction().IsBound())
		{
			return Entry->GetSpawnAction().Execute(InWorld, Entry->GetPriority());
		}
	}

	return nullptr;
}
