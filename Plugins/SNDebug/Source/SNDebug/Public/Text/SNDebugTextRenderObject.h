// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SNDebugTextRenderObject.generated.h"

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
struct FSNDebugTextParam{
	
	int32			key;
	FString			str;
	float			lifeTime;
	FVector			screenPos;
	FLinearColor	color;
	UFont*			pFont;
	float			scale;
	bool			bScalePosition;
	
	FSNDebugTextParam()
	: key(0)
	, str("")
	, lifeTime(0.0f)
	, screenPos(FVector::ZeroVector)
	, color(FColor::White)
	, pFont(NULL)
	, scale(1.0f)
	, bScalePosition(false)
	{
	}
};

//----------------------------------------------------------------------//
//
//! @brief デバッグ用の描画オブジェクト
//
//----------------------------------------------------------------------//
UCLASS()
class SNDEBUG_API USNDebugTextRenderObject : public UObject
{
	GENERATED_BODY()
	
public:
	//! @{@name デフォルトコンストラクタ
	USNDebugTextRenderObject();
	//! @}
	
	//! @{@name デストラクタ
	~USNDebugTextRenderObject();
	//! @}
	
	//! @{@name 更新処理
	void Tick(float DeltaTime);
	//! @}
	
	//! @{@name 描画処理
	void Draw(UCanvas* pCanvas);
	//! @}
	
	//! @{@name 表示するデバッグ文字を登録
	void AddString(const FString& text, FLinearColor color, float screenX, float screenY, float lifeTime, UFont* pFont, float scale, bool bScalePosition);
	//! @}
	
private:
	//!< デバッグテキスト
	TMap<int32, FSNDebugTextParam> DebugTextList;
	//!< キー情報
	int32 Key;
};
