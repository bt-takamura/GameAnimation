#include "Debug/BTStdDebugUtils.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Notifications/SNotificationList.h"

#if WITH_EDITOR
/**
 * @brief	シンプル通知表示
 * @param	InMessage		表示するメッセージ
 * @param	IsSuccess		表示時の動作（true 成功としてメッセージを表示（default）、false 失敗としてメッセージを表示）
 * @param	DurationSeconds	メッセージ表示時間（default 5.0f）
**/
void FBTStdDebugUtils::ShowSimpleNotification(const FString& InMessage, bool IsSuccess, float DurationSeconds)
{
#if WITH_EDITORONLY_DATA
	// 表示メッセージ設定
	FNotificationInfo info(FText::FromString(InMessage));
	info.ExpireDuration = DurationSeconds;
	info.bFireAndForget = true;				// 時間経過で自動で閉じる（※falseの場合は明示的に閉じる必要があります）

	TSharedPtr<SNotificationItem> Item = FSlateNotificationManager::Get().AddNotification(info);
	if (Item.IsValid())
	{
		SNotificationItem::ECompletionState State = IsSuccess ? SNotificationItem::CS_Success : SNotificationItem::CS_Fail;
		Item->SetCompletionState(State);
	}

	// ログ出力
	if (IsSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("ShowNotification: %s"), *InMessage);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShowNotification: %s"), *InMessage);
	}
#endif // WITH_EDITORONLY_DATA
}
#endif // WITH_EDITOR

#if BT_ENABLE_DEBUGOUTPUT
/**
 * @brief	文字列描画
 * @param	InWorld			描画先
 * @param	InString		描画文字列
 * @param	InColor			描画色
 * @param	IsPrintToScreen	画面に描画するか
 * @param	IsPrintToLog	ログに出力するか
 * @return	DurationSeconds	描画時間
**/
void FBTStdDebugUtils::DrawString(const UWorld* InWorld, const FString& InString, const FLinearColor& InColor, bool IsPrintToScreen, bool IsPrintToLog, float DurationSeconds)
{
	UKismetSystemLibrary::PrintString(InWorld, InString, IsPrintToScreen, IsPrintToLog, InColor, DurationSeconds);
}
#endif	// BT_ENABLE_DEBUGOUTPUT
