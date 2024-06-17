// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SNPlayerController.generated.h"

class USNInputConfig;

/**
 * 
 */
UCLASS()
class SNPLUGIN_API ASNPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	bool InitializeInput();

	void EnabledInputType(FName Name, bool bClearAll = true);

	void DisableInputType(FName Name);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:

	void SetEnableInput(FName Name, bool bClearAll);

	void SetDisableInput(FName Name);
	
	//!< 入力用コンフィグのリスト
	UPROPERTY(EditAnyWhere, Category="Input")
	TMap<FName, TObjectPtr<USNInputConfig>> InputConfigMap;
	
	UPROPERTY()
	TSet<FName> CurrentInputTypes;

	//!< 入力周りの初期化が終了したかどうかのフラグ
	bool bInitializedInput = false;
};
