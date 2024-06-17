// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/SNUserWidgetBase.h"
#include "SNMatchingMenu.generated.h"

class USNButton;
/**
 * 
 */
UCLASS()
class SNMULTITEST_API USNMatchingMenu : public USNUserWidgetBase
{
	GENERATED_BODY()

public:
	bool Initialize() override;

	void BeginDestroy() override;

	USNButton* GetHostSessionButton();

	USNButton* GetJoinSessionButton();

private:

	UPROPERTY()
	TObjectPtr<USNButton> HostSessionButton = nullptr;

	UPROPERTY()
	TObjectPtr<USNButton> JoinSessionButton = nullptr;
};

FORCEINLINE USNButton* USNMatchingMenu::GetHostSessionButton()
{
	return HostSessionButton;
}

FORCEINLINE USNButton* USNMatchingMenu::GetJoinSessionButton()
{
	return JoinSessionButton;
}

