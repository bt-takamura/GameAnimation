#pragma once

/*前方宣言*/
class IHttpRequest;
/*________________*/

/**
 *Slackへ送信する。(シングルトン)
 */
class BTSTD_API FBTStdSlackSender
{
public:
	/** メッセージ送信完了デリゲート */
	DECLARE_DELEGATE_OneParam(FSendCompleteDelegate, bool /*bConnectedSuccessfully*/);

public:
	/**
	 *	シングルトンインスタンスを取得
	 */
	static FBTStdSlackSender&	Get();

	/**
	 *	Slackにメッセージ送信
	 *	@param	OutRequest			[out]	Httpリクエストインスタンス。リクエストに失敗した場合はInvalidなWeakPtrが返ります。
	 *	@param	MessageText			[in]	投稿するメッセージ内容。
	 *	@param	CompleteDelegate	[in]	送信完了時のコールバック。
	 *	@param	WebhookURL			[in]	SlackのWebhookURL。デフォルトから変更したい場合のみ指定します。
	 *	@return	リクエストに成功したらtrue。※送信の成否ではありません。
	 */
	bool SendMessage(const FString& MessageText,	const FString& WebhookURL=TEXT(""));
	bool SendMessage(const FString& MessageText,	const FSendCompleteDelegate& CompleteDelegate, const FString& WebhookURL=TEXT(""));
	bool SendMessage(TWeakPtr<IHttpRequest, ESPMode::ThreadSafe>* OutRequest,
		const FString& MessageText, const FSendCompleteDelegate& CompleteDelegate, const FString& WebhookURL=TEXT(""));

private:
	/* ペイロードのバイト列を生成 */
	TArray<uint8>			ConvertJsonByte(const FString& MessageText) const;
	/* 文字列をJson形式にエンコード */
	static FString			JSONEncode(const FString& InString);

private:
	/** シングルトンインスタンス */
	static FBTStdSlackSender*	Instance;
};

