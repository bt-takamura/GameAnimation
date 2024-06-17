// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/SNUIDef.h"
#include "SNUserWidgetBase.generated.h"

class USNMasterWidget;
/**
 * 
 */
UCLASS()
class SNPLUGIN_API USNUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()

	friend class USNMasterWidget;
	
public:
	
	EWidgetLayer GetRegistLayer() const ;
	
private:

	void SetRegistLayer(EWidgetLayer Layer);
	
	EWidgetLayer RegistLayer = EWidgetLayer::Invalid;
};

FORCEINLINE EWidgetLayer USNUserWidgetBase::GetRegistLayer() const
{
	return RegistLayer;
}

FORCEINLINE void USNUserWidgetBase::SetRegistLayer(EWidgetLayer Layer)
{
	RegistLayer = Layer;
}


