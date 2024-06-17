// Copyright Epic Games, Inc. All Rights Reserved.


#include "BTCoreLocalSettings.h"

#define LOCTEXT_NAMESPACE "BTCoreLocal"

#if WITH_EDITOR

FText UBTCoreLocalSettings::GetSectionText() const
{
	return LOCTEXT("SettingsDisplayName", "BeeTribe Core(Local)");
}
#endif

#undef LOCTEXT_NAMESPACE