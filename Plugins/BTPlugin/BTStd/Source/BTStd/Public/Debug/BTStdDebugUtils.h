#pragma once

#include "CoreMinimal.h"
/**
 * @brief	ログ出力マクロ定義
**/
#if NO_LOGGING
	#define BT_ENABLE_DEBUGOUTPUT	(0)
#else
	#define BT_ENABLE_DEBUGOUTPUT	(1)
#endif // NO_LOGGING

#if BT_ENABLE_DEBUGOUTPUT
	#define BT_DISPLAY_LOG(Color, __FORMAT__, ...)												FBTStdDebugUtils::DrawString(GetWorld(), FString::Printf(__FORMAT__, ##__VA_ARGS__), Color, true, true, 3.0f);
	#define BT_DISPLAY_LOGT(Color, DurationSeconds, __FORMAT__, ...)							FBTStdDebugUtils::DrawString(GetWorld(), FString::Printf(__FORMAT__, ##__VA_ARGS__), Color, true, true, DurationSeconds);

	#define BT_DISPLAY_LOG_WORLD(World, Color, IsPrintLog, __FORMAT__, ...)						FBTStdDebugUtils::DrawString(World, FString::Printf(__FORMAT__, ##__VA_ARGS__), Color, true, IsPrintLog, 3.0f);
	#define BT_DISPLAY_LOGT_WORLD(World, Color, IsPrintLog, DurationSeconds, __FORMAT__, ...)	FBTStdDebugUtils::DrawString(World, FString::Printf(__FORMAT__, ##__VA_ARGS__), Color, true, IsPrintLog, DurationSeconds);
#else
	#define BT_DISPLAY_LOG(Color, __FORMAT__, ...)
	#define BT_DISPLAY_LOGT(Color, DurationSeconds, __FORMAT__, ...)

	#define BT_DISPLAY_LOG_WORLD(World, Color, IsPrintLog, __FORMAT__, ...)
	#define BT_DISPLAY_LOGT_WORLD(World, Color, IsPrintLog, DurationSeconds, __FORMAT__, ...)
#endif	// BT_ENABLE_DEBUGOUTPUT

#if WITH_EDITOR
	#define BT_SIMPLE_NOTIFICATION(__FORMAT__, ...)							FBTStdDebugUtils::ShowSimpleNotification(FString::Printf(__FORMAT__, ##__VA_ARGS__), true);
	#define BT_SIMPLE_NOTIFICATIONT(DurationSeconds, __FORMAT__, ...)		FBTStdDebugUtils::ShowSimpleNotification(FString::Printf(__FORMAT__, ##__VA_ARGS__), true, DurationSeconds);

	#define BT_SIMPLE_NOTIFICATION_FAIL(__FORMAT__, ...)					FBTStdDebugUtils::ShowSimpleNotification(FString::Printf(__FORMAT__, ##__VA_ARGS__), false);
	#define BT_SIMPLE_NOTIFICATIONT_FAIL(DurationSeconds, __FORMAT__, ...)	FBTStdDebugUtils::ShowSimpleNotification(FString::Printf(__FORMAT__, ##__VA_ARGS__), false, DurationSeconds);
#else
	#define BT_SIMPLE_NOTIFICATION(__FORMAT__, ...)
	#define BT_SIMPLE_NOTIFICATIONT(DurationSeconds, __FORMAT__, ...)

	#define BT_SIMPLE_NOTIFICATION_FAIL(__FORMAT__, ...)
	#define BT_SIMPLE_NOTIFICATIONT_FAIL(DurationSeconds, __FORMAT__, ...)
#endif // WITH_EDITOR

/**
 * @brief	デバック用便利機能
**/
class BTSTD_API FBTStdDebugUtils
{
public:
#if WITH_EDITOR
	//! シンプル通知表示
	static void ShowSimpleNotification(const FString& InMessage, bool IsSuccess = true, float DurationSeconds = 5.0f);
#endif // WITH_EDITOR

#if BT_ENABLE_DEBUGOUTPUT
	//! 文字列描画
	static void DrawString(const UWorld* InWorld, const FString& InString, const FLinearColor& InColor, bool IsPrintToScreen, bool IsPrintToLog, float DurationSeconds);
#endif	// BT_ENABLE_DEBUGOUTPUT
};
