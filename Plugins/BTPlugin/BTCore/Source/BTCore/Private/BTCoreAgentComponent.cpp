// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTCoreAgentComponent.h"

#include "BTCoreWorldSubsystem.h"

UBTCoreAgentComponent::UBTCoreAgentComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

bool UBTCoreAgentComponent::InitializeAgentActionInterfaces()
{
	TArray<FName> Keys;
	m_InterfaceSpawner.GenerateKeyArray(Keys);
	for (FName Key : Keys)
	{
		TSharedPtr<IBTCoreAgentActionInterface> Interface = CreateInterface(Key, this);
		if (Interface.IsValid())
		{
			check(!m_NameToInterfaceMap.Contains(Key));

			if (Interface->Initialize())
			{
				m_NameToInterfaceMap.Add(Key, Interface);

				if (EnumHasAnyFlags(Interface->GetTickFlags(), EBTCoreAgentActionTickFlags::ComponentTick))
				{
					TSet<FName>& InterfaceNames = m_PriorityToInterfaceNamesMap.FindOrAdd(Interface->GetPriority());
					check(!InterfaceNames.Contains(Interface->GetId()));
					InterfaceNames.Add(Interface->GetId());
				}

				if (EnumHasAnyFlags(Interface->GetTickFlags(), EBTCoreAgentActionTickFlags::PreActorTick))
				{
					TSet<FName>& InterfaceNames = m_PreActorPriorityToInterfaceNamesMap.FindOrAdd(Interface->GetPriority());
					check(!InterfaceNames.Contains(Interface->GetId()));
					InterfaceNames.Add(Interface->GetId());
				}

				if (EnumHasAnyFlags(Interface->GetTickFlags(), EBTCoreAgentActionTickFlags::PostActorTick))
				{
					TSet<FName>& InterfaceNames = m_PostActorPriorityToInterfaceNamesMap.FindOrAdd(Interface->GetPriority());
					check(!InterfaceNames.Contains(Interface->GetId()));
					InterfaceNames.Add(Interface->GetId());
				}
			}
		}
	}

	m_PriorityToInterfaceNamesMap.GenerateKeyArray(m_PrioritySorted);
	m_PrioritySorted.Sort([](const int32& A, const int32& B) {
		return A > B;
		});

	m_PreActorPriorityToInterfaceNamesMap.GenerateKeyArray(m_PreActorPrioritySorted);
	m_PreActorPrioritySorted.Sort([](const int32& A, const int32& B) {
		return A > B;
		});

	m_PostActorPriorityToInterfaceNamesMap.GenerateKeyArray(m_PostActorPrioritySorted);
	m_PostActorPrioritySorted.Sort([](const int32& A, const int32& B) {
		return A > B;
		});

	if (UBTCoreWorldSubsystem* Subsystem = UBTCoreWorldSubsystem::GetWorldSubsystemInternal(GetWorld()))
	{
		{
			bool AddResult = Subsystem->AddPreActorTickComponent(this);
			check(AddResult);
		}

		{
			bool AddResult = Subsystem->AddPostActorTickComponent(this);
			check(AddResult);
		}
	}

	return true;
}

bool UBTCoreAgentComponent::FinalizeAgentActionInterfaces()
{
	int32 NumInterface = m_NameToInterfaceMap.Num();
	for (TMap<FName, TSharedPtr<IBTCoreAgentActionInterface>>::TIterator Iter(m_NameToInterfaceMap); Iter; ++Iter)
	{
		if (Iter->Value.IsValid())
		{
			Iter->Value->Finalize();
		}
	}

	m_NameToInterfaceMap.Reset();

	if (UBTCoreWorldSubsystem* Subsystem = UBTCoreWorldSubsystem::GetWorldSubsystemInternal(GetWorld()))
	{
		{
			bool RemoveResult = Subsystem->RemovePreActorTickComponent(this);
			check(RemoveResult);
		}

		{
			bool RemoveResult = Subsystem->RemovePostActorTickComponent(this);
			check(RemoveResult);
		}
	}

	return NumInterface > 0;
}

void UBTCoreAgentComponent::UnregisterAllInterfaces()
{
	m_InterfaceSpawner.Reset();
}

void UBTCoreAgentComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TickType != ELevelTick::LEVELTICK_PauseTick)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(UBTCoreAgentComponent::TickComponent)
		for (const int32 CurrentPriority : m_PrioritySorted)
		{
			check(m_PriorityToInterfaceNamesMap.Contains(CurrentPriority));
			const TSet<FName>& InterfaceNames = m_PriorityToInterfaceNamesMap[CurrentPriority];
			for (const FName InterfaceName : InterfaceNames)
			{
				check(m_NameToInterfaceMap.Contains(InterfaceName));
				TSharedPtr<IBTCoreAgentActionInterface>& Interface = m_NameToInterfaceMap[InterfaceName];
				if (Interface.IsValid())
				{
					Interface->Tick(DeltaTime);
				}
			}
		}
	}
}

void UBTCoreAgentComponent::PreActorTickComponent(float DeltaTime, enum ELevelTick TickType)
{
	if (TickType != ELevelTick::LEVELTICK_PauseTick)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(UBTCoreAgentComponent::PreActorTickComponent)
		for (const int32 CurrentPriority : m_PreActorPrioritySorted)
		{
			check(m_PreActorPriorityToInterfaceNamesMap.Contains(CurrentPriority));
			const TSet<FName>& InterfaceNames = m_PreActorPriorityToInterfaceNamesMap[CurrentPriority];
			for (const FName InterfaceName : InterfaceNames)
			{
				check(m_NameToInterfaceMap.Contains(InterfaceName));
				TSharedPtr<IBTCoreAgentActionInterface>& Interface = m_NameToInterfaceMap[InterfaceName];
				if (Interface.IsValid())
				{
					Interface->PreActorTick(DeltaTime);
				}
			}
		}
	}
}

void UBTCoreAgentComponent::PostActorTickComponent(float DeltaTime, enum ELevelTick TickType)
{
	if (TickType != ELevelTick::LEVELTICK_PauseTick)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(UBTCoreAgentComponent::PostActorTickComponent)
		for (const int32 CurrentPriority : m_PostActorPrioritySorted)
		{
			check(m_PostActorPriorityToInterfaceNamesMap.Contains(CurrentPriority));
			const TSet<FName>& InterfaceNames = m_PostActorPriorityToInterfaceNamesMap[CurrentPriority];
			for (const FName InterfaceName : InterfaceNames)
			{
				check(m_NameToInterfaceMap.Contains(InterfaceName));
				TSharedPtr<IBTCoreAgentActionInterface>& Interface = m_NameToInterfaceMap[InterfaceName];
				if (Interface.IsValid())
				{
					Interface->PostActorTick(DeltaTime);
				}
			}
		}
	}
}

FBTCoreAgentActionEntry& UBTCoreAgentComponent::RegisterInterfaceInternal(const FName& InInterfaceType, const FBTCoreAgentActionEntry::FOnSpawnInterface& OnSpawnInterface, int32 InPriority)
{
	ensure(InInterfaceType != NAME_None);
	ensure(!m_InterfaceSpawner.Contains(InInterfaceType));

	TSharedRef<FBTCoreAgentActionEntry> NewSpawnerEntry = MakeShareable(new FBTCoreAgentActionEntry(InInterfaceType, OnSpawnInterface, InPriority));
	m_InterfaceSpawner.Emplace(InInterfaceType, NewSpawnerEntry);
	return NewSpawnerEntry.Get();
}

bool UBTCoreAgentComponent::UnregisterInterfaceInternal(FName InInterfaceType)
{
	ensure(InInterfaceType != NAME_None);
	return m_InterfaceSpawner.Remove(InInterfaceType) > 0;
}

TSharedPtr<IBTCoreAgentActionInterface> UBTCoreAgentComponent::CreateInterface(FName InInterfaceType, TObjectPtr<UBTCoreAgentComponent> InOwner)
{
	ensure(InInterfaceType != NAME_None);

	if (TSharedRef<FBTCoreAgentActionEntry>* EntryPtr = m_InterfaceSpawner.Find(InInterfaceType))
	{
		TSharedRef<FBTCoreAgentActionEntry>& Entry = *EntryPtr;
		if (Entry->GetSpawnAction().IsBound())
		{
			return Entry->GetSpawnAction().Execute(InOwner, Entry->GetPriority());
		}
	}

	return nullptr;
}
