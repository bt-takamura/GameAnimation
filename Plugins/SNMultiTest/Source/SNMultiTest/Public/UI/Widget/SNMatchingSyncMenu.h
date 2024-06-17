// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/SNUserWidgetBase.h"
#include "SNMatchingSyncMenu.generated.h"

class USNButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class SNMULTITEST_API USNMatchingSyncMenu : public USNUserWidgetBase
{
	GENERATED_BODY()

public:

	bool Initialize() override;

	void SetSessionName(const FName& Name);

	USNButton* GetStartButton() const;

private:
		
	UPROPERTY()
	TObjectPtr<UTextBlock> SessionName = nullptr;

	UPROPERTY()
	TObjectPtr<USNButton>	StartButton = nullptr;
};

FORCEINLINE USNButton* USNMatchingSyncMenu::GetStartButton() const
{
	return StartButton;
}
