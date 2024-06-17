// Fill out your copyright notice in the Description page of Project Settings.
#include "Text/SNDebugTextRenderObject.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"

//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//----------------------------------------------------------------------//
USNDebugTextRenderObject::USNDebugTextRenderObject()
: Key(0)
{
	DebugTextList.Empty();
}

//----------------------------------------------------------------------//
//
//! @brief デストラクタ
//
//----------------------------------------------------------------------//
USNDebugTextRenderObject::~USNDebugTextRenderObject(){
	DebugTextList.Empty();
}

//----------------------------------------------------------------------//
//
//! @brief 更新処理
//
//! @param DeltaTime 経過時間
//
//----------------------------------------------------------------------//
void USNDebugTextRenderObject::Tick(float DeltaTime){
	
	TArray<int32> removeList;
	
	for(auto& element : DebugTextList){
		
		FSNDebugTextParam& param(element.Value);
		// ライブタイムのチェック
		if(param.lifeTime >= 0.0f){
			param.lifeTime -= DeltaTime;
		} else {
			// ライフタイムが0を下回ったら削除
			if(param.lifeTime < 0.0f){
				
				removeList.Add(param.key);
				
				continue;
			}
		}
	}
	// リストから削除
	for(int32& removeVal : removeList){
		DebugTextList.Remove(removeVal);
	}
	// 配列を整理
	DebugTextList.Compact();
}

//----------------------------------------------------------------------//
//
//! @brief 描画処理
//
//! @param pCanvas 描画するキャンバスへのポインタ
//
//----------------------------------------------------------------------//
void USNDebugTextRenderObject::Draw(UCanvas* pCanvas){
	// 表示する文字列がない場合は処理しない
	if(DebugTextList.Num() <= 0){
		return;
	}
	// nullチェック
	if(pCanvas == nullptr){
		return;
	}
	
	UFont* pFont = GEngine->GetSmallFont();
	
	for(auto& element : DebugTextList){
		
		FSNDebugTextParam& param = element.Value;
		
		FCanvasTextItem TextItem(FVector2D(param.screenPos.X, param.screenPos.Y), FText::FromString(param.str), pFont, param.color);
		
		TextItem.EnableShadow(FLinearColor::Black);
		
		TextItem.Scale.X = param.scale;
		TextItem.Scale.Y = param.scale;
		// 文字列を描画
		pCanvas->DrawItem(TextItem);
	}
}

//----------------------------------------------------------------------//
//
//! @brief 表示する文字列を追加
//
//! @param text           表示する文字列
//! @param color          カラー
//! @param screenX        スクリーン座標X
//! @param screenY        スクリーン座標Y
//! @param lifeTime       表示時間
//! @param pFont          フォント
//! @param scale          スケール
//! @param bScalePosition スケールをポジションに反映させるか
//
//----------------------------------------------------------------------//
void USNDebugTextRenderObject::AddString(const FString& text, FLinearColor color, float screenX, float screenY, float lifeTime, UFont* pFont, float scale, bool bScalePosition){
	// int32最大値に達したらオールクリア
	if(Key >= INT32_MAX){
		
		Key = 0;
		
		DebugTextList.Empty();
		
		return;
	}
	
	FSNDebugTextParam param;
	
	param.key				= Key;
	param.str				= text;
	param.lifeTime			= lifeTime;
	param.screenPos.X		= screenX;
	param.screenPos.Y		= screenY;
	param.color				= color;
	param.pFont				= pFont;
	param.scale				= scale;
	param.bScalePosition	= bScalePosition;
	
	DebugTextList.Add(Key, param);
	
	++Key;
}

