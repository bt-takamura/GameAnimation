// Fill out your copyright notice in the Description page of Project Settings.

#include "SlackSender/BTStdSlackSetting.h"

#define LOCTEXT_NAMESPACE "SlackSender"

#if WITH_EDITOR
/** セクション名を取得 */
FText UBTStdSlackSetting::GetSectionText() const
{
	return LOCTEXT("SettingsDisplayName", "Slack Sender");
}
#endif	

#undef LOCTEXT_NAMESPACE