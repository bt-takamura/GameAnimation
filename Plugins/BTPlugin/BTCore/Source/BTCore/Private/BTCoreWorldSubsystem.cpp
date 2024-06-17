// Copyright Epic Games, Inc. All Rights Reserved.


#include "BTCoreWorldSubsystem.h"

#include "GameFramework/WorldSettings.h"
#include "GameFramework/PlayerState.h"

#include "BTCoreTickableManager.h"
#include "BTCoreAgentComponent.h"

#if WITH_EDITOR

bool UBTCoreWorldSubsystem::bAllowSubsystemOnPreviewWorld = true;

#endif // WITH_EDITOR

UBTCoreWorldSubsystem::UBTCoreWorldSubsystem()
{
}

UBTCoreWorldSubsystem::~UBTCoreWorldSubsystem()
{
}

bool UBTCoreWorldSubsystem::AddPreActorTickComponent(TObjectPtr<class UBTCoreAgentComponent> InComp)
{
	int32 CurrentNum = m_PreActorTickComponents.Num();
	m_PreActorTickComponents.Add(InComp);
	return m_PreActorTickComponents.Num() > CurrentNum;
}

bool UBTCoreWorldSubsystem::AddPostActorTickComponent(TObjectPtr<class UBTCoreAgentComponent> InComp)
{
	int32 CurrentNum = m_PostActorTickComponents.Num();
	m_PostActorTickComponents.Add(InComp);
	return m_PostActorTickComponents.Num() > CurrentNum;
}

bool UBTCoreWorldSubsystem::RemovePreActorTickComponent(TObjectPtr<class UBTCoreAgentComponent> InComp)
{
	int32 CurrentNum = m_PreActorTickComponents.Num();
	if (CurrentNum <= 0)
	{
		return false;
	}

	m_PreActorTickComponents.Remove(InComp);
	return m_PreActorTickComponents.Num() < CurrentNum;
}

bool UBTCoreWorldSubsystem::RemovePostActorTickComponent(TObjectPtr<class UBTCoreAgentComponent> InComp)
{
	int32 CurrentNum = m_PostActorTickComponents.Num();
	if (CurrentNum <= 0)
	{
		return false;
	}

	m_PostActorTickComponents.Remove(InComp);
	return m_PostActorTickComponents.Num() < CurrentNum;
}

void UBTCoreWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const TSharedRef<FBTCoreTickableSpawnerManager>& SpawnerManager = FBTCoreTickableSpawnerManager::Get();
	TArray<FName> Keys;
	SpawnerManager->TickableSpawner.GenerateKeyArray(Keys);
	for (FName Key : Keys)
	{
		TSharedPtr<IBTCoreTickableInterface> Interface = SpawnerManager->CreateTickableInterface(Key, GetWorld());
		if (Interface.IsValid())
		{
			check(!m_NameToInterfaceMap.Contains(Key));

			if (Interface->Initialize())
			{
				m_NameToInterfaceMap.Add(Key, Interface);
				TSet<FName>& InterfaceNames = m_PriorityToInterfaceNamesMap.FindOrAdd(Interface->GetPriority());
				check(!InterfaceNames.Contains(Interface->GetId()));
				InterfaceNames.Add(Interface->GetId());
			}
		}
	}

	m_PriorityToInterfaceNamesMap.GenerateKeyArray(m_PrioritySorted);
	m_PrioritySorted.Sort([](const int32& A, const int32& B) {
		return A > B;
		});

	WorldPreActorTickHandle = FWorldDelegates::OnWorldPreActorTick.AddUObject(this, &UBTCoreWorldSubsystem::OnWorldPreActorTick);
	WorldPostActorTickHandle = FWorldDelegates::OnWorldPostActorTick.AddUObject(this, &UBTCoreWorldSubsystem::OnWorldPostActorTick);
}

void UBTCoreWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnWorldPreActorTick.Remove(WorldPreActorTickHandle);
	FWorldDelegates::OnWorldPostActorTick.Remove(WorldPostActorTickHandle);

	for (TMap<FName, TSharedPtr<IBTCoreTickableInterface>>::TIterator Iter(m_NameToInterfaceMap); Iter; ++Iter)
	{
		if (Iter->Value.IsValid())
		{
			Iter->Value->Finalize();
		}
	}

	m_NameToInterfaceMap.Reset();
}

void UBTCoreWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	check(GetWorld() != nullptr);

	bool bPaused = false;
	if (const UWorld* const World = GetWorld())
	{
		bPaused = World->bDebugPauseExecution || (World->GetWorldSettings() && World->GetWorldSettings()->GetPauserPlayerState());
	}

	for (const int32 CurrentPriority : m_PrioritySorted)
	{
		check(m_PriorityToInterfaceNamesMap.Contains(CurrentPriority));
		const TSet<FName>& InterfaceNames = m_PriorityToInterfaceNamesMap[CurrentPriority];
		for (const FName InterfaceName : InterfaceNames)
		{
			check(m_NameToInterfaceMap.Contains(InterfaceName));
			TSharedPtr<IBTCoreTickableInterface>& Interface = m_NameToInterfaceMap[InterfaceName];
			if (Interface.IsValid())
			{
				Interface->Tick(DeltaTime, bPaused);
			}
		}
	}
}

TStatId UBTCoreWorldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBTCoreWorldSubsystem, STATGROUP_Tickables);
}

bool UBTCoreWorldSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
#if WITH_EDITOR
	if (WorldType == EWorldType::EditorPreview)
	{
		return bAllowSubsystemOnPreviewWorld;
	}
#endif // WITH_EDITOR

	return WorldType == EWorldType::Game || WorldType == EWorldType::Editor || WorldType == EWorldType::PIE;
}

UBTCoreWorldSubsystem* UBTCoreWorldSubsystem::GetWorldSubsystemInternal(const UWorld* InWorld)
{
	if (InWorld)
	{
		return InWorld->GetSubsystem<UBTCoreWorldSubsystem>();
	}

	return nullptr;
}

void UBTCoreWorldSubsystem::OnWorldPreActorTick(UWorld* InWorld, ELevelTick InLevelTick, float InDeltaSeconds)
{
	TSet<TWeakObjectPtr<UBTCoreAgentComponent>> RemovedComps;
	if (GetWorld() == InWorld && InLevelTick == LEVELTICK_All)
	{
		for (TSet<TWeakObjectPtr<UBTCoreAgentComponent>>::TIterator It(m_PreActorTickComponents); It; ++It)
		{
			if ((*It).IsValid())
			{
				UBTCoreAgentComponent* CurrentComp = (*It).Get();
				CurrentComp->PreActorTickComponent(InDeltaSeconds, InLevelTick);
			}
			else
			{
				RemovedComps.Add((*It));
			}
		}
	}

	for (TSet<TWeakObjectPtr<UBTCoreAgentComponent>>::TIterator It(RemovedComps); It; ++It)
	{
		m_PreActorTickComponents.Remove((*It));
	}	
}

void UBTCoreWorldSubsystem::OnWorldPostActorTick(UWorld* InWorld, ELevelTick InLevelTick, float InDeltaSeconds)
{
	TSet<TWeakObjectPtr<UBTCoreAgentComponent>> RemovedComps;
	if (GetWorld() == InWorld && InLevelTick == LEVELTICK_All)
	{
		for (TSet<TWeakObjectPtr<UBTCoreAgentComponent>>::TIterator It(m_PostActorTickComponents); It; ++It)
		{
			if ((*It).IsValid())
			{
				UBTCoreAgentComponent* CurrentComp = (*It).Get();
				CurrentComp->PostActorTickComponent(InDeltaSeconds, InLevelTick);
			}
			else
			{
				RemovedComps.Add((*It));
			}
		}
	}

	for (TSet<TWeakObjectPtr<UBTCoreAgentComponent>>::TIterator It(RemovedComps); It; ++It)
	{
		m_PostActorTickComponents.Remove((*It));
	}
}