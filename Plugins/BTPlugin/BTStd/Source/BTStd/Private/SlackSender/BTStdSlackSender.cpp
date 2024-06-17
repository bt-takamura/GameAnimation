#include "SlackSender/BTStdSlackSender.h"
#include "HttpModule.h"
#include "HttpManager.h"
#include "Interfaces/IHttpResponse.h"
#include "SlackSender/BTStdSlackSetting.h"


DEFINE_LOG_CATEGORY_STATIC(LogSlackSender, Log, All);

FBTStdSlackSender*	FBTStdSlackSender::Instance	= nullptr;

/** シングルトンインスタンスを取得 */
FBTStdSlackSender&	FBTStdSlackSender::Get()
{
	if (!Instance)
	{
		UE_LOG(LogSlackSender, Log, TEXT("FBTStdSlackSender Instance SlackSender"));
		Instance = new FBTStdSlackSender();
	}
	return *Instance;
}

/**
 *	Slackにメッセージ送信
 */
bool FBTStdSlackSender::SendMessage(const FString& MessageText, const FString& WebhookURL/*=FString::EmptyString*/)
{
	return SendMessage( MessageText, FSendCompleteDelegate::CreateLambda([](bool){}), WebhookURL );
}

bool FBTStdSlackSender::SendMessage(const FString& MessageText,
	const FSendCompleteDelegate& CompleteDelegate, const FString& WebhookURL/*=FString::EmptyString*/)
{
	return SendMessage( nullptr, MessageText, CompleteDelegate, WebhookURL );
}

bool FBTStdSlackSender::SendMessage(TWeakPtr<IHttpRequest, ESPMode::ThreadSafe>* OutRequest, const FString& MessageText, const FSendCompleteDelegate& CompleteDelegate, const FString& WebhookURL/*=FString::EmptyString*/)
{
#if !UE_BUILD_SHIPPING
	UE_LOG(LogSlackSender, Log, TEXT("SendMessage()  MessageText=%s%s"), *MessageText.Left(10).Replace(TEXT("\r"),TEXT("")).Replace(TEXT("\n"),TEXT("")), MessageText.Len()>=10?TEXT("..."):TEXT("") );
	const FString	FinalWebhookURL	= WebhookURL.IsEmpty() ? GetDefault<UBTStdSlackSetting>()->DefaultWebhookURL : WebhookURL;

	if (OutRequest)
	{
		OutRequest->Reset();
	}
 
	// 不正なURLを検出した場合は何もしない
	if (FinalWebhookURL.Find(TEXT("https://")) != 0)
	{ 
		return false;
	}
	
	// ペイロードバイト列を生成
	const TArray<uint8>	PayloadBytes = ConvertJsonByte( MessageText );

	// 送信
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
		Request->SetURL( FinalWebhookURL );
		Request->SetHeader( TEXT("Content-Type"), TEXT("application/json; charset=utf-8") );
		Request->SetVerb( TEXT("POST") );
		Request->SetContent( PayloadBytes );
		Request->OnProcessRequestComplete().BindLambda( [CompleteDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			//完了イベント
			UE_LOG(LogSlackSender, Log, TEXT("OnProcessRequestComplete() bSucceeded=%s URL=%s ResponseCode=%d Content=%s"),
				bSucceeded?				*FCoreTexts::Get().True.ToString()	:*FCoreTexts::Get().False.ToString(),
				HttpResponse.IsValid()?	*HttpRequest->GetURL()				:TEXT("None"),
				HttpResponse.IsValid()?	HttpResponse->GetResponseCode()		:-1,
				HttpResponse.IsValid()?	*HttpResponse->GetContentAsString()	:TEXT("None")
				);
			CompleteDelegate.ExecuteIfBound( bSucceeded );
		} );
		bool bSucces = Request->ProcessRequest();
 
		if (bSucces && OutRequest)
		{
			*OutRequest = Request;
		}
		return bSucces;
	}
#else
	return false;
#endif

	
}

/* Slackに送信するペイロードのバイト列を生成 */
TArray<uint8> FBTStdSlackSender::ConvertJsonByte(const FString& MessageText) const
{
	// ペイロード文字列を生成
	const FString	PayloadStr = [&]()
	{
		FString	Retval;

		// prefix
		Retval += TEXT("{");
		// テキスト
		Retval += FString::Printf(TEXT("\"text\": \"%s\""), *JSONEncode(MessageText));
		Retval += TEXT("}");
		return Retval;
	}();

	TArray<uint8>	PayloadBytes;
	FTCHARToUTF8	Converter((const TCHAR*)*PayloadStr, PayloadStr.Len());
	const int32 Length = Converter.Length();
	PayloadBytes.Reset(Length);
	PayloadBytes.AddUninitialized(Length);
	CopyAssignItems((ANSICHAR*)PayloadBytes.GetData(), Converter.Get(), Length);
	return PayloadBytes;
}

/* 文字列をJson形式にエンコード */
FString FBTStdSlackSender::JSONEncode(const FString& InString)
{
	FString OutString = InString;
	OutString.ReplaceInline(TEXT("\\"), TEXT("\\\\"));
	OutString.ReplaceInline(TEXT("\r\n"), TEXT("\\n"));
	OutString.ReplaceInline(TEXT("\r"), TEXT("\\n"));
	OutString.ReplaceInline(TEXT("\n"), TEXT("\\n"));
	OutString.ReplaceInline(TEXT("\""), TEXT("\\\""));
	return OutString;
}