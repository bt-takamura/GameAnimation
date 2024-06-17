// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SNMatchingMenu.h"

#include "UI/Widget/SNButton.h"

bool USNMatchingMenu::Initialize()
{
	bool Result = Super::Initialize();

	HostSessionButton = Cast<USNButton>(GetWidgetFromName(TEXT("Host")));
	JoinSessionButton = Cast<USNButton>(GetWidgetFromName(TEXT("Join")));

	return Result;
}

void USNMatchingMenu::BeginDestroy()
{
	Super::BeginDestroy();

	HostSessionButton = nullptr;
	JoinSessionButton = nullptr;
}
