// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define BTCORE_DECLARE_JOIN_AGAIN(x, y) x ## y
#define BTCORE_DECLARE_STRINGIFY(x) #x
#define BTCORE_DECLARE_TOSTRING(x) BTCORE_DECLARE_STRINGIFY(x)
#define BTCORE_DECLARE_POSTFIX BTCORE_DECLARE_JOIN_AGAIN(BTCORE_DECLARE_TOSTRING(__func__), BTCORE_DECLARE_TOSTRING(__LINE__))
#define BTCORE_DECLARE_TICKABLE_ID(TickableClass) \
public: \
	static FORCEINLINE FName GetStaticInterfaceName() { static const FName Name(#TickableClass); return Name; } \
	static FORCEINLINE FName GetStaticId() { FName FuncName(__func__); static const FName NameId(FString::Printf(TEXT("%s%s%d"), TEXT(#TickableClass), *FuncName.ToString(), __LINE__)); return NameId; } \
	virtual FName GetName() const override { return GetStaticInterfaceName(); } \
	virtual FName GetId() const override { return GetStaticId(); } \
private: \

class IBTCoreTickableInterface
{
public:
	static const int32 DEFAULT_PRIORITY = 128;
public:

	virtual ~IBTCoreTickableInterface()
	{}

	class UWorld* GetOwnerWorld() const
	{
		return m_OwnerWorld.Get();
	}

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

	virtual void Tick(float DeltaTime, bool bIsPaused) = 0;	

	virtual FName GetName() const = 0;
	virtual FName GetId() const = 0;
	int32 GetPriority() const
	{
		return m_Priority;
	}

protected:
	explicit IBTCoreTickableInterface(TObjectPtr<class UWorld> InOwnerWorld, int32 InPriority = DEFAULT_PRIORITY)
		: m_OwnerWorld(InOwnerWorld)
		, m_Priority(InPriority)
	{}

	TWeakObjectPtr<class UWorld> m_OwnerWorld;
	int32 m_Priority = 0;
};
