// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SNPlayablePawnInterface.generated.h"

class  USNActionBase;
struct FInputActionValue;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USNPlayablePawnInterface : public UInterface
{
	GENERATED_BODY()
};

//----------------------------------------------------------------------//
//
//! @brief プレイ可能なポーン用のインターフェイス
//
//----------------------------------------------------------------------//
class SNPLUGIN_API ISNPlayablePawnInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	//! @{@name デフォルトコンストラクタ
	ISNPlayablePawnInterface();
	//! @}
	
	//! @{@name 入力された際に呼ばれるアクションを追加
	virtual void AddInputAction(const FName& Name, USNActionBase* Action) = 0;
	//! @}
	
	//! @{@name アクションを取得
	virtual USNActionBase* GetAction(const FName& Name) = 0;
	//! @}
	
	//! @{@name サーバー側でアクションを実行
	virtual void ExecuteActionOnServer(const FName& Name, const FInputActionValue& InputActionValue) = 0;
	//! @}

	virtual int GetActionNum() const = 0;
	
protected:
	
	//! @{@name アクションを実行
	void ExecuteAction(const FName& Name, const FInputActionValue& InputActionValue);
	//! @}
};
