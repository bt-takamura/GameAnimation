// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/DeveloperSettings.h"
#include "BTStdSlackSetting.generated.h"

/**
 * Slackへの送信設定
 */
UCLASS(config=Game, defaultconfig)
class UBTStdSlackSetting : public UDeveloperSettings {
	GENERATED_BODY()

public:
	/** デフォルトのWebhookURL */
	UPROPERTY(EditAnywhere, config, Category=Settings)
	FString DefaultWebhookURL;

	/** クラッシュ時にSlackへログを送信するかどうか(送信はDefaultWebhookURLを使用) */
	UPROPERTY(EditAnywhere, config, Category=Settings, meta=( ConfigRestartRequired = true ))
	bool bSendCrashLog;

public:
	UBTStdSlackSetting(): bSendCrashLog(true)
	{
		CategoryName = TEXT("Plugins");
		SectionName = TEXT("SlackSender");
	}
#if WITH_EDITOR
	/** セクション名を取得 */
	virtual FText GetSectionText() const override;
#endif
};
