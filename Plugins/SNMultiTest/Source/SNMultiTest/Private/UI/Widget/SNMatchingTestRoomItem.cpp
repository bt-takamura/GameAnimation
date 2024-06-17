// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SNMatchingTestRoomItem.h"

#include "Components/TextBlock.h"
#include "UI/Widget/SNButton.h"

//----------------------------------------------------------------------//
//
//! @brief 初期化処理
//
//! @retval true  正常終了
//! @retval false 異常終了
//
//----------------------------------------------------------------------//
bool USNMatchingTestRoomItem::Initialize(){
	
	bool Result = Super::Initialize();
	// 参加ボタンを取得
	JoinButton = Cast<USNButton>(GetWidgetFromName(TEXT("Join")));
	
	if(JoinButton != nullptr){
		// ルートとして自分を登録
		JoinButton->SetRootWidget(this);
	}
	// セッション名用のテキストボックスを取得
	SessionName = Cast<UTextBlock>(GetWidgetFromName(TEXT("SessionNameText")));
	// 接続数用のテキストボックスを取得
	ConnectionNum = Cast<UTextBlock>(GetWidgetFromName(TEXT("ConnectionNumText")));
	
	return Result;
}

//----------------------------------------------------------------------//
//
//! @brief ルーム情報を設定
//
//! @param Name セッション名
//! @param Num  接続数
//
//----------------------------------------------------------------------//
void USNMatchingTestRoomItem::SetRoom(const FString& Name, int Num){
	
	if(SessionName != nullptr){
		SessionName->SetText(FText::FromString(Name));
	}
	
	if(ConnectionNum != nullptr){
		ConnectionNum->SetText(FText::AsNumber(Num));
	}
}

//----------------------------------------------------------------------//
//
//! @brief セッション名を取得
//
//! @retval セッション名
//
//----------------------------------------------------------------------//
FString USNMatchingTestRoomItem::GetSessionName() const{
	
	FString Name("");
	
	if(SessionName != nullptr){
		Name = SessionName->GetText().ToString();
	}
	
	return Name;
}

