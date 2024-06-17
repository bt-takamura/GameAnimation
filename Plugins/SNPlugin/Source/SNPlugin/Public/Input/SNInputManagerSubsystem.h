// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SNInputManagerSubsystem.generated.h"

class UInputMappingContext;
/**
 * 
 */
UCLASS()
class SNPLUGIN_API USNInputManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	//! @{@name 初期化処理
	void Initialize(FSubsystemCollectionBase& Collection) override;
	//! @}
	
	void AddInputContext(const FName& Name, UInputMappingContext* InputMappingContext);
	
	void RemoveInputContext(const FName& Name);
	
	void EnableInputMapping(const FName& Name, bool bClearMapping=true);

	void DisableInputMapping(const FName& Name);

	bool IsExist(const FName& Name) const ;
	
private:
	UPROPERTY()
	TMap<FName, TObjectPtr<UInputMappingContext>> InputMapContextMap;
};

FORCEINLINE bool USNInputManagerSubsystem::IsExist(const FName& Name) const
{
	return (InputMapContextMap.Contains(Name));
}
