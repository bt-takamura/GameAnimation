//--------------------------------------------------
//! @file   BTStdSlackCrashReportSubsystem.h
//! @brief  クラッシュ時にSlackに通知するサブシステム機能
//--------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "BTStdSlackCrashReportSubsystem.generated.h"

// ReSharper disable once IdentifierTypo
/** クラッシュ通知機能の有効/無効マクロ */
#define ENABLE_BTSTD_SLACK_CRASH_REPORT (WITH_EDITOR || !UE_BUILD_SHIPPING)


/**
 * クラッシュ時にSlackに通知するサブシステム機能
 */
UCLASS()
class UBTStdSlackCrashReportSubsystem : public UEngineSubsystem {
	GENERATED_BODY()

	// Begin USubsystem Interface
public:
	virtual void Deinitialize() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// End USubsystem Interface

private:
#if ENABLE_BTSTD_SLACK_CRASH_REPORT
	/** クラッシュ報告機能を有効にするか */
	static bool IsEnableCrashReport();
	/** 現在のスレッド識別名を取得する */
	static FString GetThreadName();
	/** MCデリゲードのバインドを解除する */
	template<typename TMCType>
	static void UnbindMCDelegate(TMCType& InMCDelegate, FDelegateHandle& InOutDelegateHandle);
	/** クラッシュ報告を行う */
	void SendCrashReport();
#else
	static constexpr bool IsEnableCrashReport() { return false; }
#endif// ENABLE_BTSTD_SLACK_CRASH_REPORT
	
	//////////////////////////////////////////////////////////////////////////
	//  コールバック関数
	//////////////////////////////////////////////////////////////////////////

#if ENABLE_BTSTD_SLACK_CRASH_REPORT
	/** システムエラー時イベント */
	void OnSystemError();
	/** ensure時イベント */
	void OnSystemEnsure();
	/** エラー発生シャットダウン時イベント */
	void OnShutdownAfterError();
	/** アプリケーション終了時イベント */
	void OnApplicationWillTerminateDelegate();
#endif // ENABLE_BTSTD_SLACK_CRASH_REPORT

private:
#if ENABLE_BTSTD_SLACK_CRASH_REPORT
	/** システムエラー時イベント */
	FDelegateHandle OnSystemErrorDelegateHandle;
	/** ensure時デリゲートハンドル */
	FDelegateHandle OnSystemEnsureDelegateHandle;
	/** エラーシャットダウン時デリゲートハンドル */
	FDelegateHandle OnShutdownAfterErrorHandle;
	/** OSシャットダウン時デリゲートハンドル */
	FDelegateHandle OnApplicationWillTerminateDelegateHandle;
	/** 既にSystemErrorが呼び出されたか */
	bool bCalledSystemError;
	/** HandleCrashReportが呼び出されたかど��?�� */
	bool bCalledHandleCrashReport;
	/** HandleCrashReportのMutex */
	FCriticalSection CrashReportMutex;
#endif //  ENABLE_BTSTD_SLACK_CRASH_REPORT
};

#if ENABLE_BTSTD_SLACK_CRASH_REPORT
/**
 * @brief MCデリゲードのバインドを解除する
 */
template<typename TMCType>
void UBTStdSlackCrashReportSubsystem::UnbindMCDelegate(TMCType& InMCDelegate, FDelegateHandle& InOutDelegateHandle)
{
	if( InOutDelegateHandle.IsValid() )
	{
		InMCDelegate.Remove(InOutDelegateHandle);
		InOutDelegateHandle.Reset();
	}
}
#endif// ENABLE_BTSTD_SLACK_CRASH_REPORT

