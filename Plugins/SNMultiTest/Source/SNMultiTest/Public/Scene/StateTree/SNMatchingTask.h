// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Scene/StateTree/SNMultiTestMatchingSceneBase.h"
#include "SNMatchingTask.generated.h"

class USNButton;

UENUM(BlueprintInternalUseOnly)
enum class EMatchingSelection:uint8
{
	None UMETA(DisplayName = "Not Select"),
	Host UMETA(DisplayName = "Create Host Session"),
	Join UMETA(DisplayName = "Join Session")
};

/**
 * 
 */
UCLASS()
class SNMULTITEST_API USNMatchingTask : public USNMultiTestMatchingSceneBase
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

	UFUNCTION()
	void OnClickHostSession(USNButton* Button);

	UFUNCTION()
	void OnClickJoinSession(USNButton* Button);

private:
	// 
	UPROPERTY(VisibleAnywhere)
	EMatchingSelection MatchingSelection = EMatchingSelection::None;
};
