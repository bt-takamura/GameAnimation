// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/SNUserWidgetBase.h"
#include "SNMatchingHostSessionMenu.generated.h"

class USNButton;
class UEditableTextBox;
class UComboBoxString;

/**
 * 
 */
UCLASS()
class SNMULTITEST_API USNMatchingHostSessionMenu : public USNUserWidgetBase
{
	GENERATED_BODY()

public:

	bool Initialize() override;

	FName GetSessionName() const ;

	int GetConnectionNum() const ;

	USNButton* GetCreateButton() const;

	USNButton* GetCancelButton() const;
	
private:
	UPROPERTY()
	TObjectPtr<UEditableTextBox> SessionName = nullptr;

	UPROPERTY()
	TObjectPtr<UComboBoxString> ConnectionNum = nullptr;

	UPROPERTY()
	TObjectPtr<USNButton>		CreateButton = nullptr;

	UPROPERTY()
	TObjectPtr<USNButton>		CancelButton = nullptr;
};

FORCEINLINE USNButton* USNMatchingHostSessionMenu::GetCreateButton() const
{
	return CreateButton;
}

FORCEINLINE USNButton* USNMatchingHostSessionMenu::GetCancelButton() const
{
	return CancelButton;
}
