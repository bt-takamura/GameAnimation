// Fill out your copyright notice in the Description page of Project Settings.


#include "System/SNBlueprintFunctionLibrary.h"

#include "SNDef.h"
#include "Input/SNInputManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Online/SNOnlineSystem.h"
#include "System/SNGameInstance.h"

#include "Interfaces/OnlineSessionDelegates.h"

void USNBlueprintFunctionLibrary::SetInputMappingContext(FName Name, bool bClear)
{
	USNGameInstance* GameInstance(Cast<USNGameInstance>(UGameplayStatics::GetGameInstance(GetPrimaryWorld())));

	if(GameInstance == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Failed to Start Online System because SNGameInstance is nullptr."));

		return;
	}

	USNInputManagerSubsystem* InputManagerSubsystem(GameInstance->GetSubsystem<USNInputManagerSubsystem>());

	if(InputManagerSubsystem != nullptr)
	{
		InputManagerSubsystem->EnableInputMapping(Name, bClear);
	}
}

void USNBlueprintFunctionLibrary::Login()
{
	USNOnlineSystem* OnlineSystem(GetOnlineSystem());

	if(OnlineSystem != nullptr)
	{
		OnlineSystem->Login();	
	}
	
}

void USNBlueprintFunctionLibrary::HostSession()
{
	USNOnlineSystem* OnlineSystem(GetOnlineSystem());

	if(OnlineSystem != nullptr)
	{
		OnlineSystem->HostSession();
	}
}

void USNBlueprintFunctionLibrary::FindSession()
{
	USNOnlineSystem* OnlineSystem(GetOnlineSystem());

	if(OnlineSystem != nullptr)
	{
		OnlineSystem->FindSession();
	}
}

void USNBlueprintFunctionLibrary::KillSession(FName SessionName)
{
	USNOnlineSystem* OnlineSystem(GetOnlineSystem());

	if(OnlineSystem != nullptr)
	{
		OnlineSystem->KillSession(SessionName);
	}
}

USNOnlineSystem* USNBlueprintFunctionLibrary::GetOnlineSystem()
{
	USNGameInstance* GameInstance(Cast<USNGameInstance>(UGameplayStatics::GetGameInstance(GetPrimaryWorld())));

	if(GameInstance == nullptr)
	{
		SNPLUGIN_LOG(TEXT("Failed to get OnlineSyste because SNGameInstance is nullptr."));

		return nullptr;
	}

	if(GameInstance->IsEnabledOnlineSystem() == false)
	{
		SNPLUGIN_LOG(TEXT("Failed to get OnlineSyste because OnlneSystem is nullptr."));

		return nullptr;
	}

	return GameInstance->GetOnlineSystem();
}

