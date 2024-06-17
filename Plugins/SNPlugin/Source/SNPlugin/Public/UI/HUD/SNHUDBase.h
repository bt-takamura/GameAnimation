// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SNHUDBase.generated.h"

//----------------------------------------------------------------------//
//! @brief HUDベースクラス
//----------------------------------------------------------------------//
UCLASS()
class SNPLUGIN_API ASNHUDBase : public AHUD
{
	GENERATED_BODY()
	
public:
	
	//! @{@name デフォルトコンストラクタ
	ASNHUDBase();
	//! @}
	
	//! @{@name デストラクタ
	virtual ~ASNHUDBase();
	//! @}
	
	//! @{@name 更新
	virtual void Tick(float DeltaTime) override ;
	//! @}
	
	//! @{@name 描画
	virtual void PostRender() override ;
	//! @}
	
	//! @{@name デバッグ用のキャンバスを取得
	UCanvas* GetDebugCanvas();
	//! @}
	
private:
	
};

//----------------------------------------------------------------------//
//
//! @brief デバッグ用のキャンバスを取得
//
//! @retval デバッグ用のキャンバス
//
//----------------------------------------------------------------------//
inline UCanvas* ASNHUDBase::GetDebugCanvas(){
	return DebugCanvas;
}
