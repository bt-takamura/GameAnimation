// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTCoreDebugTypes.h"

#if !UE_BUILD_SHIPPING

IBTCoreConsoleManager* IBTCoreConsoleManager::Singleton;

void IBTCoreConsoleManager::SetupSingleton()
{
	check(!Singleton);
	if (!Singleton)
	{
		Singleton = new FBTCoreConsoleManager; // we will leak this
	}
	check(Singleton);
}

bool FBTCoreConsoleManager::RegisterConsoleVariable(const FString& InName, FBTCoreConsoleVariableRef* InVariable)
{;
	check(InVariable != nullptr);

	if (m_NameToConsoleVariableMap.Contains(InName))
	{
		return false;
	}

	int32 CurrentNum = m_NameToConsoleVariableMap.Num();

	m_NameToConsoleVariableMap.Add(InName, InVariable);

	return CurrentNum < m_NameToConsoleVariableMap.Num();
}

bool FBTCoreConsoleManager::RegisterConsoleWithWorldAndArgs(const FString& InName, FBTCoreConsoleWithWorldAndArgs* InConsole)
{
	check(InConsole != nullptr);

	if (m_NameToConsoleWithWorldAndArgsMap.Contains(InName))
	{
		return false;
	}

	int32 CurrentNum = m_NameToConsoleWithWorldAndArgsMap.Num();

	m_NameToConsoleWithWorldAndArgsMap.Add(InName, InConsole);

	return CurrentNum < m_NameToConsoleWithWorldAndArgsMap.Num();
}

bool FBTCoreConsoleManager::GetConsoleVariableNames(TArray<FString>& OutNames) const
{
	m_NameToConsoleVariableMap.GenerateKeyArray(OutNames);
	return OutNames.Num() > 0;
}

FBTCoreConsoleVariableRef* FBTCoreConsoleManager::GetConsoleVariableRef(const FString& InName) const
{
	if (m_NameToConsoleVariableMap.Contains(InName))
	{
		return m_NameToConsoleVariableMap[InName];
	}

	return nullptr;
}

bool FBTCoreConsoleManager::GetConsoleWithWorldAndArgsNames(TArray<FString>& OutNames) const
{
	m_NameToConsoleWithWorldAndArgsMap.GenerateKeyArray(OutNames);
	return OutNames.Num() > 0;
}

FBTCoreConsoleWithWorldAndArgs* FBTCoreConsoleManager::GetConsoleWithWorldAndArgs(const FString& InName) const
{
	if (m_NameToConsoleWithWorldAndArgsMap.Contains(InName))
	{
		return m_NameToConsoleWithWorldAndArgsMap[InName];
	}

	return nullptr;
}

namespace BeeTribe::Debug
{
	BT_DECLARE_DEBUGCOMMAND_VARIABLE_GLOBAL(bool, ShowGlobalDebugDummy, false, 0, 1, TEXT("Debug."), TEXT("グローバルデバッグダミー使用しません"), 0);

	BT_DECLARE_DEBUGCOMMAND_VARIABLE(int32, ShowGlobalDebugDummyInt, 1, -100, 100, TEXT("Debug."), TEXT("Intデバッグダミー使用しません"), 0);
	BT_DECLARE_DEBUGCOMMAND_VARIABLE(float, ShowGlobalDebugDummyFloat, 0.f, -50.f, 50.f, TEXT("Debug."), TEXT("Floatデバッグダミー使用しません"), 0);
}

FBTCoreConsoleVariableRef::FBTCoreConsoleVariableRef(const TCHAR* Name, bool& RefValue, bool ValueMin, bool  ValueMax, const TCHAR* Help, int32 Priority, uint32 Flags)
	: FAutoConsoleVariableRef(Name, RefValue, Help, Flags)
{
	//FScopeLock RegionScopeLock(&ConsoleVariablesCS);
	FString Key(Name);
	bool Result = IBTCoreConsoleManager::Get().RegisterConsoleVariable(Key, this);
	check(Result);

	m_Name = Key;
	m_Help = Help;
	m_Priority = Priority;
	m_Type = ConsoleVariableType::Bool;
	m_Value.bValue = RefValue;
	m_MinValue.bValue = ValueMin;
	m_MaxValue.bValue = ValueMax;
}

FBTCoreConsoleVariableRef::FBTCoreConsoleVariableRef(const TCHAR* Name, int32& RefValue, int32 ValueMin, int32 ValueMax, const TCHAR* Help, int32 Priority, uint32 Flags)
	: FAutoConsoleVariableRef(Name, RefValue, Help, Flags)
{
	FString Key(Name);
	bool Result = IBTCoreConsoleManager::Get().RegisterConsoleVariable(Key, this);
	check(Result);

	m_Name = Key;
	m_Help = Help;
	m_Priority = Priority;
	m_Type = ConsoleVariableType::Int;
	m_Value.iValue = RefValue;
	m_MinValue.iValue = ValueMin;
	m_MaxValue.iValue = ValueMax;
}

FBTCoreConsoleVariableRef::FBTCoreConsoleVariableRef(const TCHAR* Name, float& RefValue, float ValueMin, float ValueMax, const TCHAR* Help, int32 Priority, uint32 Flags)
	: FAutoConsoleVariableRef(Name, RefValue, Help, Flags)
{
	FString Key(Name);
	bool Result = IBTCoreConsoleManager::Get().RegisterConsoleVariable(Key, this);
	check(Result);

	m_Name = Key;
	m_Help = Help;
	m_Priority = Priority;
	m_Type = ConsoleVariableType::Float;
	m_Value.fValue = RefValue;
	m_MinValue.fValue = ValueMin;
	m_MaxValue.fValue = ValueMax;
}

FBTCoreConsoleWithWorldAndArgs::FBTCoreConsoleWithWorldAndArgs(const TCHAR* Name, const TCHAR* Help, const FConsoleCommandWithWorldAndArgsDelegate& Command, int32 Priority, uint32 Flags)
	: FAutoConsoleCommandWithWorldAndArgs(Name, Help, Command, Flags)
{
	FString Key(Name);
	bool Result = IBTCoreConsoleManager::Get().RegisterConsoleWithWorldAndArgs(Key, this);
	check(Result);

	m_Name = Key;
	m_Help = Help;
	m_Priority = Priority;
}

#endif