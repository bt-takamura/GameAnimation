// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SNPlayablePawnInterface.h"
#include "Character/Base/SNWheeledVehiclePawnBase.h"
#include "SNWheeledVehiclePlayerBase.generated.h"

//----------------------------------------------------------------------//
//
//! @brief プレイ可能なビークル用のインターフェイス
//
//----------------------------------------------------------------------//
UCLASS()
class SNPLUGIN_API ASNWheeledVehiclePlayerBase : public ASNWheeledVehiclePawnBase, public ISNPlayablePawnInterface
{
	GENERATED_BODY()

public:
	
	//! @{@name 終了処理
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//! @}
	
	//! @{@name 入力された際に呼ばれるアクションを追加
	virtual void AddInputAction(const FName& Name, USNActionBase* Action) override;
	//! @}
	
	//! @{@name アクションを取得
	virtual USNActionBase* GetAction(const FName& Name) override;
	//! @}

	virtual int GetActionNum() const override;
	
	//! @{@name サーバー側でアクションを実行
	virtual void ExecuteActionOnServer(const FName& Name, const FInputActionValue& InputActionValue) override;
	//! @}
	
protected:
	
	virtual void NotifyRestarted() override;
	
private:
	
	//! @{@name 入力からアクション実行用のレプリケーション
	UFUNCTION(Server, Reliable)
	void ExecuteInputAction_OnServer(FName Name, FVector InputValue, EInputActionValueType Type);
	//! @}
	
	//!< 入力アクションリスト
	UPROPERTY()
	TMap<FName, TObjectPtr<USNActionBase>> InputActionMap;
};
