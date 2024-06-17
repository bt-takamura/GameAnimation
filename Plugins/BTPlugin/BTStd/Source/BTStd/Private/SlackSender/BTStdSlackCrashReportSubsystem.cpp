//--------------------------------------------------
//! @file   BTStdSlackCrashReportSubsystem.cpp
//! @brief  クラッシュ時にSlackに通知するサブシステム機能
//--------------------------------------------------
#include "BTStdSlackCrashReportSubsystem.h"

#include "BTStdPrivate.h"

#include "HAL/ThreadManager.h"

#include "SlackSender/BTStdSlackSender.h"
#include "SlackSender/BTStdSlackSetting.h"


/**
 * @brief Implement this for deinitialization of instances of the system
 */
void UBTStdSlackCrashReportSubsystem::Deinitialize()
{
#if ENABLE_BTSTD_SLACK_CRASH_REPORT
	// バインド解除
	UnbindMCDelegate(FCoreDelegates::OnHandleSystemError, OnSystemErrorDelegateHandle);
	UnbindMCDelegate(FCoreDelegates::OnHandleSystemEnsure, OnSystemEnsureDelegateHandle);
	UnbindMCDelegate(FCoreDelegates::OnShutdownAfterError, OnShutdownAfterErrorHandle);
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3
	UnbindMCDelegate(FCoreDelegates::GetApplicationWillTerminateDelegate(), OnApplicationWillTerminateDelegateHandle);
#else
	UnbindMCDelegate(FCoreDelegates::ApplicationWillTerminateDelegate, OnApplicationWillTerminateDelegateHandle);
#endif // ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3

#endif // ENABLE_BTSTD_SLACK_CRASH_REPORT
	Super::Deinitialize();
}

/**
 * @brief Implement this for initialization of instances of the system
 */
void UBTStdSlackCrashReportSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
#if ENABLE_BTSTD_SLACK_CRASH_REPORT
	// 機能無効時は初期化処理を行わない
	if( IsEnableCrashReport() == false ) { return; }

	// エラー時のコールバックをバインド
	OnSystemErrorDelegateHandle = FCoreDelegates::OnHandleSystemError.AddUObject(this, &UBTStdSlackCrashReportSubsystem::OnSystemError);
	OnSystemEnsureDelegateHandle = FCoreDelegates::OnHandleSystemEnsure.AddUObject(this, &UBTStdSlackCrashReportSubsystem::OnSystemEnsure);
	OnShutdownAfterErrorHandle = FCoreDelegates::OnShutdownAfterError.AddUObject(this, &UBTStdSlackCrashReportSubsystem::OnShutdownAfterError);
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3
	OnApplicationWillTerminateDelegateHandle = FCoreDelegates::GetApplicationWillTerminateDelegate().AddUObject(this, &UBTStdSlackCrashReportSubsystem::OnApplicationWillTerminateDelegate);
#else
	OnApplicationWillTerminateDelegateHandle = FCoreDelegates::ApplicationWillTerminateDelegate.AddUObject(this, &UBTStdSlackCrashReportSubsystem::OnApplicationWillTerminateDelegate);
#endif // ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3

	UE_LOG(LogBTStd, Log, TEXT("UBTStdSlackCrashReportSubsystem Initialized Report System"));
#endif // ENABLE_BTSTD_SLACK_CRASH_REPORT
}

/**
 * @brief Override to control if the Subsystem should be created at all.
 */
bool UBTStdSlackCrashReportSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if( Super::ShouldCreateSubsystem(Outer) == false )
	{
		return false;
	}
	return IsEnableCrashReport();;
}

#if ENABLE_BTSTD_SLACK_CRASH_REPORT
/**
 * @brief クラッシュ報告機能を有効にするか
 */
bool UBTStdSlackCrashReportSubsystem::IsEnableCrashReport()
{
#if !UE_BUILD_SHIPPING
	// デバッカーアタッチの場合はクラッシュ報告しない
	if( FPlatformMisc::IsDebuggerPresent() )
	{
		return false;
	}
#endif // !UE_BUILD_SHIPPING
	if( const UBTStdSlackSetting* pSlackSetting = GetDefault<UBTStdSlackSetting>() )
	{
		return pSlackSetting->bSendCrashLog;
	}
	return false;
}
#endif // ENABLE_BTSTD_SLACK_CRASH_REPORT

#if ENABLE_BTSTD_SLACK_CRASH_REPORT
/**
 * @brief 現在のスレッド識別名を取得する 
 */
FString UBTStdSlackCrashReportSubsystem::GetThreadName()
{
	const uint32 ThreadId = FPlatformTLS::GetCurrentThreadId();
	const FString& ThreadName = FThreadManager::Get().GetThreadName(ThreadId);
	return ThreadName.IsEmpty() ? FString::Printf(TEXT("%d"), ThreadId) : ThreadName;
}
#endif // ENABLE_BTSTD_SLACK_CRASH_REPORT

#if ENABLE_BTSTD_SLACK_CRASH_REPORT
/**
 * @brief クラッシュ報告を行う
 */
void UBTStdSlackCrashReportSubsystem::SendCrashReport()
{
	UE_LOG(LogBTStd, Log, TEXT("UBTStdSlackCrashReportSubsystem::SendCrashReport() Start Thread=%s"), *GetThreadName());

	FScopeLock Lock(&CrashReportMutex);

	// SystemErrorが呼び出されてなければスルーする
	if( !bCalledSystemError )
	{
		UE_LOG(LogBTStd, Log, TEXT("UBTStdSlackCrashReportSubsystem::SendCrashReport() None Error"));
		return;
	}

	// 複数回呼び出されないよう対応
	if( bCalledHandleCrashReport )
	{
		UE_LOG(LogBTStd, Log, TEXT("UBTStdSlackCrashReportSubsystem::SendCrashReport() Called Error"));
		return;
	}

	/** 発生タイミング等の情報 */
	auto GetAppInfo = []()
		{
			FString Info;
			Info += FString::Printf(TEXT("実行者 : %s\r\n"), FPlatformProcess::UserName(false/*bOnlyAlphaNumeric*/));
			Info += FString::Printf(TEXT("発生時間 : %s\r\n"), *FDateTime::Now().ToString(TEXT("%Y/%m/%d %H:%M:%S.%s")));
			Info += FString::Printf(TEXT("Build : %s %s %s\r\n"), *FString(FPlatformProperties::IniPlatformName()), LexToString(FApp::GetBuildConfiguration()), GIsEditor ? TEXT("Editor") : TEXT(""));
			return Info;
		};

	/**
	 *	コールスタック取得
	 *	@param	bCompactCallstack	[in]	trueの場合コンパクト版のコールバックを取得する
	 */
	auto GetProgramCallstack = [](bool bCompactCallstack)
		{
			/** エラー詳細を取得 */
			FString Str = GErrorHist;

			// 通常の場合はそのまま返す
			if( !bCompactCallstack )
			{
				return Str;
			}

			// コンパクト版への変換
			TArray<FString> Lines;
			Str.ParseIntoArrayLines(Lines, false);
			for( FString& Line : Lines )
			{
				//定型チェック
				if( Line.Find(TEXT("0x")) != 0 )
				{
					continue;
				}

				int32 IndexFunctionStart = Line.Find(TEXT("!"));
				if( IndexFunctionStart == INDEX_NONE )
				{
					continue;
				}
				Line.RemoveAt(0, IndexFunctionStart + 1);

				//ファイル名チェック
				const int32 IndexSourceFileSquareBracketsStart = Line.Find(TEXT("["), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				const int32 IndexSourceFileDelimiter = Line.Find(TEXT("\\"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				if( IndexSourceFileSquareBracketsStart == INDEX_NONE || IndexSourceFileDelimiter == INDEX_NONE )
				{
					continue;
				}
				Line.RemoveAt(IndexSourceFileSquareBracketsStart + 1, IndexSourceFileDelimiter - IndexSourceFileSquareBracketsStart);
			}

			Str.Reset();
			for( FString& Line : Lines )
			{
				Str += Line + TEXT("\r\n");
			}
			Str.RemoveFromEnd(TEXT("\r\n"));
			return Str;
		};

	/** BPのコールスタック取得 */
	auto GetBlueprintCallstack = []()
		{
			return FFrame::GetScriptCallstack();
		};

	
	const FString ApplicationInfo = GetAppInfo();
	const FString ProgramCallstack = GetProgramCallstack(true);
	const FString BlueprintCallstack = GetBlueprintCallstack();
	FString Msg;
	Msg += TEXT("\r\n*発生環境*\r\n_\r\n");
	Msg += ApplicationInfo;
	Msg += TEXT("_\r\n");
	Msg += TEXT("\r\n*C++ Call Stack*\r\n>\r\n");
	Msg += ProgramCallstack;
	Msg += TEXT("\r\n");
	Msg += TEXT("\r\n*Blueprint Callstack*\r\n>\r\n");
	Msg += BlueprintCallstack;
	Msg += TEXT("\r\n");

	// 発生タイミング等の情報を表示
	UE_LOG(LogBTStd, Error, TEXT("\r\n\r\n[Application Information]\r\n%s"), *ApplicationInfo);

	// コールスタックを表示
	UE_LOG(LogBTStd, Error, TEXT("\r\n\r\n[Program Callstack]\r\n%s"), *ProgramCallstack);

	// Blueprintコールスタックを表示
	UE_LOG(LogBTStd, Error, TEXT("\r\n\r\n[Blueprint Callstack]\r\n%s"), *BlueprintCallstack);

	// Slackに投稿
	FBTStdSlackSender::Get().SendMessage(Msg);
	UE_LOG(LogBTStd, Log, TEXT("UBTStdSlackCrashReportSubsystem::SendCrashReport() Post Send Message"));
	FPlatformProcess::Sleep(5.f); // 送信完了前にプロセスが終了しないように対策
	UE_LOG(LogBTStd, Log, TEXT("UBTStdSlackCrashReportSubsystem::SendCrashReport() SleepEnd"));
	
	bCalledHandleCrashReport = true;
}
#endif // ENABLE_BTSTD_SLACK_CRASH_REPORT

#if ENABLE_BTSTD_SLACK_CRASH_REPORT
/**
 * @brief システムエラー時イベント
 */
void UBTStdSlackCrashReportSubsystem::OnSystemError()
{
	UE_LOG(LogBTStd, Log, TEXT("OnSystemError Called. Thread=%s"), *GetThreadName());

	FScopeLock Lock(&CrashReportMutex);
	bCalledSystemError = true;
}
#endif // ENABLE_BTSTD_SLACK_CRASH_REPORT

#if ENABLE_BTSTD_SLACK_CRASH_REPORT
/**
 * @brief ensure時イベント
 */
// ReSharper disable once CppMemberFunctionMayBeStatic
void UBTStdSlackCrashReportSubsystem::OnSystemEnsure()
{
	UE_LOG(LogBTStd, Log, TEXT("OnSystemEnsure Called. Thread=%s"), *GetThreadName());

#if 0 // ensureは報告しない(しても今の処理ではスタックを取得できないため)
	FScopeLock Lock(&CrashReportMutex);
	bCalledSystemError = true;
	
	// クラッシュ報告を行う
	SendCrashReport();
#endif// 0 
}
#endif // ENABLE_BTSTD_SLACK_CRASH_REPORT

#if ENABLE_BTSTD_SLACK_CRASH_REPORT
/**
 * @brief エラー発生シャットダウン時イベント
 */
void UBTStdSlackCrashReportSubsystem::OnShutdownAfterError()
{
	UE_LOG(LogBTStd, Log, TEXT("OnShutdownAfterError Called. Thread=%s"), *GetThreadName());

	// クラッシュ報告を行う
	SendCrashReport();
}
#endif // ENABLE_BTSTD_SLACK_CRASH_REPORT

#if ENABLE_BTSTD_SLACK_CRASH_REPORT
/**
 * @brief アプリケーション終了時イベント
 */
void UBTStdSlackCrashReportSubsystem::OnApplicationWillTerminateDelegate()
{
	UE_LOG(LogBTStd, Log, TEXT("OnApplicationWillTerminateDelegate Called. Thread=%s"), *GetThreadName());

	// クラッシュ報告を行う
	SendCrashReport();
}
#endif // ENABLE_BTSTD_SLACK_CRASH_REPORT
