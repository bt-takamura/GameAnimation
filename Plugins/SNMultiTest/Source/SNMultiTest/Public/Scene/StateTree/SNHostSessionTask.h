// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Scene/StateTree/SNMultiTestMatchingSceneBase.h"
#include "SNHostSessionTask.generated.h"

class USNButton;

/**
 * 
 */
UCLASS()
class SNMULTITEST_API USNHostSessionTask : public USNMultiTestMatchingSceneBase
{
	GENERATED_BODY()

public:
	
	//! @{@name 毎フレームの更新処理
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	//! @}
	
	//! @{@name タスクの開始処理
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	//! @}
	
	//! @{@name タスクの終了処理
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	//! @}

	virtual void HudPostLoad() override;
	
private:
	UFUNCTION()
	void OnCreateButtonClicked(USNButton* Button);

	UFUNCTION()
	void OnCancelButtonClicked(USNButton* Button);

	UFUNCTION()
	void OnCompleteHostSession(FName SessionName, bool bWasSuccessful);

	UPROPERTY(EditAnywhere, Category="Lobby")
	TSoftObjectPtr<UObject> LobbyMap;
	
	UPROPERTY(VisibleAnywhere)
	bool bSucceed = false;
	
	bool bCancel = false;
};
