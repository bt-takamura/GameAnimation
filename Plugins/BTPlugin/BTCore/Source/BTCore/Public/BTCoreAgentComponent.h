// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BTCoreTickableInterface.h"

#include "BTCoreAgentComponent.generated.h"

#define BTCORE_DECLARE_AGENTACTION_ID(AgentActionClass) \
public: \
	static FORCEINLINE FName GetStaticInterfaceName() { static const FName Name(#AgentActionClass); return Name; } \
	static FORCEINLINE FName GetStaticId() { FName FuncName(__func__); static const FName NameId(FString::Printf(TEXT("%s%s%d"), TEXT(#AgentActionClass), *FuncName.ToString(), __LINE__)); return NameId; } \
	virtual FName GetName() const override { return GetStaticInterfaceName(); } \
	virtual FName GetId() const override { return GetStaticId(); } \
private: \

class UBTCoreAgentComponent;

enum class EBTCoreAgentActionTickFlags : int32
{
	None = 0,

	ComponentTick = 1 << 1,
	PreActorTick = 1 << 2,
	PostActorTick = 1 << 3,
	TickableInterfaceTick = 1 << 4,// Invalid

	AllTickFlags = ComponentTick | PreActorTick | PostActorTick | TickableInterfaceTick
};
ENUM_CLASS_FLAGS(EBTCoreAgentActionTickFlags);

class IBTCoreAgentActionInterface
{
public:
	static const int32 DEFAULT_PRIORITY = 128;
public:

	virtual ~IBTCoreAgentActionInterface()
	{}

	TWeakObjectPtr<UBTCoreAgentComponent> GetOwner() const
	{
		return m_Owner.Get();
	}

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

	virtual void Tick(float DeltaTime) = 0;

	virtual void TickableInterfaceTick(float DeltaTime) {}
	virtual void PreActorTick(float DeltaTime) {}
	virtual void PostActorTick(float DeltaTime) {}

	virtual FName GetName() const = 0;
	virtual FName GetId() const = 0;

	EBTCoreAgentActionTickFlags GetTickFlags() const
	{
		return m_TickFlags;
	}

	int32 GetPriority() const
	{
		return m_Priority;
	}

protected:
	explicit IBTCoreAgentActionInterface(TObjectPtr<UBTCoreAgentComponent> InOwner, EBTCoreAgentActionTickFlags InFlags = EBTCoreAgentActionTickFlags::ComponentTick, int32 InPriority = DEFAULT_PRIORITY)
		: m_Owner(InOwner)
		, m_TickFlags(InFlags)
		, m_Priority(InPriority)
	{}

	TWeakObjectPtr<UBTCoreAgentComponent> m_Owner;
	EBTCoreAgentActionTickFlags m_TickFlags;
	int32 m_Priority = 0;
};

class FBTCoreAgentActionEntry : public TSharedFromThis<FBTCoreAgentActionEntry>
{
public:
	DECLARE_DELEGATE_RetVal_TwoParams(TSharedPtr<IBTCoreAgentActionInterface>, FOnSpawnInterface, TObjectPtr<UBTCoreAgentComponent>, int32/*InPriority*/);
public:
	FBTCoreAgentActionEntry(const FName& InInterfaceType, const FOnSpawnInterface& InSpawnMethod, int32 InPriority)
		: m_InterfaceType(InInterfaceType)
		, m_OnSpawnInterface(InSpawnMethod)
		, m_Priority(InPriority)
	{}

	TSharedPtr<FBTCoreAgentActionEntry> AsSpawnerEntry()
	{
		return SharedThis(this);
	}

	const FName GetInterfaceType() const
	{
		return m_InterfaceType;
	};

	FOnSpawnInterface& GetSpawnAction()
	{
		return m_OnSpawnInterface;
	}

	int32 GetPriority() const
	{
		return m_Priority;
	}

private:
	FName m_InterfaceType;
	FOnSpawnInterface m_OnSpawnInterface;
	int32 m_Priority = -1;
};


UCLASS(NotBlueprintable, NotPlaceable, NotBlueprintType, Transient, HideDropdown)
class BTCORE_API UBTCoreAgentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBTCoreAgentComponent();

	bool InitializeAgentActionInterfaces();
	bool FinalizeAgentActionInterfaces();

	template<typename TActionInterfaceClass = IBTCoreAgentActionInterface>
	FBTCoreAgentActionEntry& RegisterActionInterface(const FBTCoreAgentActionEntry::FOnSpawnInterface& OnSpawnInterface, int32 InPriority = IBTCoreAgentActionInterface::DEFAULT_PRIORITY)
	{
		return RegisterInterfaceInternal(TActionInterfaceClass::GetStaticId(), OnSpawnInterface, InPriority);
	}

	template<typename TActionInterfaceClass = IBTCoreAgentActionInterface>
	bool UnregisterActionInterface()
	{
		return UnregisterInterfaceInternal(TActionInterfaceClass::GetStaticId());
	}

	void UnregisterAllInterfaces();

	template<typename TActionInterfaceClass = IBTCoreAgentActionInterface>
	TActionInterfaceClass* GetActionInterface()
	{
		return GetActionInterface(TActionInterfaceClass::GetStaticId());
	}

	template<typename TActionInterfaceClass = IBTCoreAgentActionInterface>
	const TActionInterfaceClass* GetActionInterface() const
	{
		return GetActionInterface(TActionInterfaceClass::GetStaticId());
	}

private:
	IBTCoreAgentActionInterface* GetActionInterface(const FName& InInterfaceType)
	{
		if (m_NameToInterfaceMap.Contains(InInterfaceType))
		{
			TSharedPtr<IBTCoreAgentActionInterface>& Interface = m_NameToInterfaceMap[InInterfaceType];
			if (Interface.IsValid())
			{
				return Interface.Get();
			}
		}

		return nullptr;
	}

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void PreActorTickComponent(float DeltaTime, enum ELevelTick TickType);
	virtual void PostActorTickComponent(float DeltaTime, enum ELevelTick TickType);

protected:
	TMap<int32, TSet<FName>> m_PriorityToInterfaceNamesMap;
	TMap<FName, TSharedPtr<IBTCoreAgentActionInterface>> m_NameToInterfaceMap;
	TArray<int32> m_PrioritySorted;
	TSet<FName> m_ComponentActionInterfaces;

	TArray<int32> m_PreActorPrioritySorted;
	TArray<int32> m_PostActorPrioritySorted;
	TMap<int32, TSet<FName>> m_PreActorPriorityToInterfaceNamesMap;
	TMap<int32, TSet<FName>> m_PostActorPriorityToInterfaceNamesMap;

private:
	FBTCoreAgentActionEntry& RegisterInterfaceInternal(const FName& InInterfaceType, const FBTCoreAgentActionEntry::FOnSpawnInterface& OnSpawnInterface, int32 InPriority = 0);
	bool UnregisterInterfaceInternal(FName InInterfaceType);
private:
	TSharedPtr<IBTCoreAgentActionInterface> CreateInterface(FName InInterfaceType, TObjectPtr<UBTCoreAgentComponent> InOwner);
private:
	typedef TMap<FName, TSharedRef<FBTCoreAgentActionEntry>> FInterfaceSpawner;
	FInterfaceSpawner m_InterfaceSpawner;

	friend class UBTCoreWorldSubsystem;
};
