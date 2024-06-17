// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Base/SNPlayerBase.h"
#include "SNDef.h"
#include "Utility/SNUtility.h"
#include "Character/SNPlayerController.h"

void ASNPlayerBase::NotifyRestarted()
{
	Super::NotifyRestarted();

	ASNPlayerController* PlayerController(Cast<ASNPlayerController>(Controller));

	if(PlayerController != nullptr)
	{
		// 入力の初期化処理
		PlayerController->InitializeInput();
		
		SNPLUGIN_LOG(TEXT("Setup Player Input Component."));
	}
}
