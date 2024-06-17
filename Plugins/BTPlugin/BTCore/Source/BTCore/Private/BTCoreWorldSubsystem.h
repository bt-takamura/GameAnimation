// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "BTCoreTickableInterface.h"

#include "BTCoreWorldSubsystem.generated.h"

UCLASS()
class UBTCoreWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UBTCoreWorldSubsystem();
	virtual ~UBTCoreWorldSubsystem();

public:
#if !(UE_BUILD_SHIPPING)
	void SetDebugMenuWidget(TSharedPtr<class SBTCoreDebugMenu>& InWidget)
	{
		m_DebugMenuWidget = InWidget;
	}

	TSharedPtr<class SBTCoreDebugMenu>& GetDebugMenuWidget()
	{
		return m_DebugMenuWidget;
	}
#endif

	bool AddPreActorTickComponent(TObjectPtr<class UBTCoreAgentComponent> InComp);
	bool AddPostActorTickComponent(TObjectPtr<class UBTCoreAgentComponent> InComp);

	bool RemovePreActorTickComponent(TObjectPtr<class UBTCoreAgentComponent> InComp);
	bool RemovePostActorTickComponent(TObjectPtr<class UBTCoreAgentComponent> InComp);

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual bool IsTickable() const override { return true; }
	virtual bool IsTickableInEditor() const override { return true; }
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

	template<typename TTickableInterfaceClass = IBTCoreTickableInterface>
	TTickableInterfaceClass* GetTickableInterface()
	{
		return GetTickableInterface(TTickableInterfaceClass::GetStaticId());
	}

	template<typename TTickableInterfaceClass = IBTCoreTickableInterface>
	const TTickableInterfaceClass* GetTickableInterface() const
	{
		return GetTickableInterface(TTickableInterfaceClass::GetStaticId());
	}

	IBTCoreTickableInterface* GetTickableInterface(const FName& InInterfaceType)
	{
		if (m_NameToInterfaceMap.Contains(InInterfaceType))
		{
			TSharedPtr<IBTCoreTickableInterface>& Interface = m_NameToInterfaceMap[InInterfaceType];
			if (Interface.IsValid())
			{
				return Interface.Get();
			}
		}
		
		return nullptr;
	}

public:
	static UBTCoreWorldSubsystem* GetWorldSubsystemInternal(const UWorld* InWorld);

private:
	void OnWorldPreActorTick(UWorld* InWorld, ELevelTick InLevelTick, float InDeltaSeconds);
	void OnWorldPostActorTick(UWorld* InWorld, ELevelTick InLevelTick, float InDeltaSeconds);
private:
	FDelegateHandle WorldPreActorTickHandle;
	FDelegateHandle WorldPostActorTickHandle;

	TMap<int32, TSet<FName>> m_PriorityToInterfaceNamesMap;
	TMap<FName, TSharedPtr<IBTCoreTickableInterface>> m_NameToInterfaceMap;
	TArray<int32> m_PrioritySorted;

	TSet<TWeakObjectPtr<class UBTCoreAgentComponent>> m_PreActorTickComponents;
	TSet<TWeakObjectPtr<class UBTCoreAgentComponent>> m_PostActorTickComponents;

#if WITH_EDITOR
	static bool bAllowSubsystemOnPreviewWorld;
#endif


private:
#if !(UE_BUILD_SHIPPING)
	TSharedPtr<class SBTCoreDebugMenu> m_DebugMenuWidget;
#endif
	friend class FBTCoreUtils;
	friend class UBTCoreAgentComponent;
};
