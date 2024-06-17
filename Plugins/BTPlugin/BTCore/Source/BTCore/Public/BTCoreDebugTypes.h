// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

class FBTCoreConsoleVariableRef;
class FBTCoreConsoleWithWorldAndArgs;

struct BTCORE_API IBTCoreConsoleManager
{
public:
	static const int32 INVISIBILITY_PRIORITY = -1;
public:
	FORCEINLINE static IBTCoreConsoleManager& Get()
	{
		if (!Singleton)
		{
			SetupSingleton();
			check(Singleton != nullptr);
		}
		return *Singleton;
	}

	virtual bool RegisterConsoleVariable(const FString& InName, FBTCoreConsoleVariableRef* InVariable) = 0;
	virtual bool RegisterConsoleWithWorldAndArgs(const FString& InName, FBTCoreConsoleWithWorldAndArgs* InConsole) = 0;

protected:
	virtual ~IBTCoreConsoleManager() { }

	virtual bool GetConsoleVariableNames(TArray<FString>& OutNames) const = 0;
	virtual FBTCoreConsoleVariableRef* GetConsoleVariableRef(const FString& InName) const = 0;

	virtual bool GetConsoleWithWorldAndArgsNames(TArray<FString>& OutNames) const = 0;
	virtual FBTCoreConsoleWithWorldAndArgs* GetConsoleWithWorldAndArgs(const FString& InName) const = 0;
private:
	static IBTCoreConsoleManager* Singleton;
	static void SetupSingleton();

	friend class UBTCoreEngineSubsystem;
};

class BTCORE_API FBTCoreConsoleManager final : public IBTCoreConsoleManager
{
public:
	FBTCoreConsoleManager()
	{
	}

	~FBTCoreConsoleManager()
	{
	}

	virtual bool RegisterConsoleVariable(const FString& InName, FBTCoreConsoleVariableRef* InVariable) override;
	virtual bool RegisterConsoleWithWorldAndArgs(const FString& InName, FBTCoreConsoleWithWorldAndArgs* InConsole) override;
protected:
	virtual bool GetConsoleVariableNames(TArray<FString>& OutNames) const override;
	virtual FBTCoreConsoleVariableRef* GetConsoleVariableRef(const FString& InName) const override;

	virtual bool GetConsoleWithWorldAndArgsNames(TArray<FString>& OutNames) const override;
	virtual FBTCoreConsoleWithWorldAndArgs* GetConsoleWithWorldAndArgs(const FString& InName) const override;
private:
	TMap<FString, FBTCoreConsoleVariableRef*> m_NameToConsoleVariableMap;
	TMap<FString, FBTCoreConsoleWithWorldAndArgs*> m_NameToConsoleWithWorldAndArgsMap;
};

class FBTCoreConsoleVariableRef final : public FAutoConsoleVariableRef
{
public:
	enum class ConsoleVariableType
	{
		Bool,
		Int,
		Float,
		MAX
	};

	typedef union console_value
	{
		bool bValue;
		int32 iValue;
		float fValue;
	} ConsoleValue;

	ConsoleVariableType m_Type;
	ConsoleValue m_Value;
	ConsoleValue m_MinValue;
	ConsoleValue m_MaxValue;

	FString m_Name;
	FString m_Help;

	int32 m_Priority = 0;
public:
	BTCORE_API FBTCoreConsoleVariableRef(const TCHAR* Name, bool& RefValue, bool ValueMin, bool  ValueMax, const TCHAR* Help, int32 Priority = 0, uint32 Flags = ECVF_Default);
	BTCORE_API FBTCoreConsoleVariableRef(const TCHAR* Name, int32& RefValue, int32 ValueMin, int32 ValueMax, const TCHAR* Help, int32 Priority = 0, uint32 Flags = ECVF_Default);
	BTCORE_API FBTCoreConsoleVariableRef(const TCHAR* Name, float& RefValue, float ValueMin, float ValueMax, const TCHAR* Help, int32 Priority = 0, uint32 Flags = ECVF_Default);

private:
};

class FBTCoreConsoleWithWorldAndArgs final : public FAutoConsoleCommandWithWorldAndArgs
{
public:
	FString m_Name;
	FString m_Help;

	int32 m_Priority = 0;
public:
	BTCORE_API FBTCoreConsoleWithWorldAndArgs(const TCHAR* Name, const TCHAR* Help, const FConsoleCommandWithWorldAndArgsDelegate& Command, int32 Priority = 0, uint32 Flags = ECVF_Default);

private:
};

#endif

#if UE_BUILD_SHIPPING
#define BT_DECLARE_INTERNAL_DEBUGCOMMAND_VARIABLE(InternalName__, InternalPrefix__, VariableType__, VariableName__, InitialValue__, ValueMin__, ValueMax__, ConsoleCommandPrefix__, Help__, Priority__) \
	static VariableType__ VariableName__ = InitialValue__
#define BT_DECLARE_INTERNAL_DEBUGCOMMAND_VARIABLE_GLOBAL(InternalName__, InternalPrefix__, VariableType__, VariableName__, InitialValue__, ValueMin__, ValueMax__, ConsoleCommandPrefix__, Help__, Priority__) \
	VariableType__ VariableName__ = InitialValue__

#define BT_DECLARE_INTERNAL_DEBUGCOMMAND_WITH_WORLD_AND_ARGS(InternalName__, InternalPrefix__, VariableName__,  ConsoleCommandPrefix__, Help__, Command__, Priority__)

#else
#define BT_DECLARE_INTERNAL_DEBUGCOMMAND_VARIABLE(InternalName__, InternalPrefix__, VariableType__, VariableName__, InitialValue__, ValueMin__, ValueMax__, ConsoleCommandPrefix__, Help__, Priority__) \
	static VariableType__ VariableName__ = InitialValue__; \
	static FBTCoreConsoleVariableRef CVar##InternalName__##VariableName__(*(FString(InternalPrefix__) + FString(ConsoleCommandPrefix__) + FString(TEXT(#VariableName__))), VariableName__, ValueMin__, ValueMax__, Help__, Priority__)
#define BT_DECLARE_INTERNAL_DEBUGCOMMAND_VARIABLE_GLOBAL(InternalName__, InternalPrefix__, VariableType__, VariableName__, InitialValue__, ValueMin__, ValueMax__, ConsoleCommandPrefix__, Help__, Priority__)	\
	VariableType__ VariableName__ = InitialValue__; \
	static FBTCoreConsoleVariableRef CVar##InternalName__##VariableName__(*(FString(InternalPrefix__) + FString(ConsoleCommandPrefix__) + FString(TEXT(#VariableName__))), VariableName__, ValueMin__, ValueMax__, Help__, Priority__)
#define BT_DECLARE_INTERNAL_DEBUGCOMMAND_WITH_WORLD_AND_ARGS(InternalName__, InternalPrefix__, VariableName__,  ConsoleCommandPrefix__, Help__, Command__, Priority__) \
	static FBTCoreConsoleWithWorldAndArgs CVar##InternalName__##VariableName__(*(FString(InternalPrefix__) + FString(ConsoleCommandPrefix__) + FString(TEXT(#VariableName__))), Help__, Command__, Priority__)
#endif

#define BT_DECLARE_DEBUGCOMMAND_VARIABLE(VariableType__, VariableName__, InitialValue__, ValueMin__, ValueMax__, ConsoleCommandPrefix__, Help__, Priority__) \
	BT_DECLARE_INTERNAL_DEBUGCOMMAND_VARIABLE(BT, TEXT("BT."), VariableType__, VariableName__, InitialValue__, ValueMin__, ValueMax__, ConsoleCommandPrefix__, Help__, Priority__)
#define BT_DECLARE_DEBUGCOMMAND_VARIABLE_GLOBAL(VariableType__, VariableName__, InitialValue__, ValueMin__, ValueMax__, ConsoleCommandPrefix__, Help__, Priority__) \
	BT_DECLARE_INTERNAL_DEBUGCOMMAND_VARIABLE_GLOBAL(BT,TEXT("BT."), VariableType__, VariableName__, InitialValue__, ValueMin__, ValueMax__, ConsoleCommandPrefix__, Help__, Priority__)
#define BT_DECLARE_DEBUGCOMMAND_WITH_WORLD_AND_ARGS(VariableName__, ConsoleCommandPrefix__, Help__, Command__, Priority__) \
	BT_DECLARE_INTERNAL_DEBUGCOMMAND_WITH_WORLD_AND_ARGS(BT,TEXT("BT."), VariableName__, ConsoleCommandPrefix__, Help__, Command__, Priority__)

#define BT_DECLARE_DEBUGCOMMAND_VARIABLE_EXTERN(VariableType__, VariableName__) \
	extern VariableType__ VariableName__;

#if !UE_BUILD_SHIPPING
namespace BeeTribe::Debug
{
	BT_DECLARE_DEBUGCOMMAND_VARIABLE_EXTERN(bool, ShowGlobalDebugDummy);
}
#endif