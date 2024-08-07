// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/System/GAGameInstance.h"

#include "IDetailTreeNode.h"
#include "SNDef.h"

void UGAGameInstance::Init()
{
	Super::Init();

	if((ItemDataTable == nullptr) && (ItemDataObject.IsNull() == false))
	{
		ItemDataTable = ItemDataObject.LoadSynchronous();
	}
}

const FItemData* UGAGameInstance::GetItemData(const FName& Name) const
{
	if(ItemDataTable == nullptr)
	{
		SNPLUGIN_ERROR(TEXT("Item Data Table is nullptr."));

		return nullptr;
	}

	const FItemData* ItemData(ItemDataTable->FindRow<FItemData>(Name, nullptr));

	return ItemData;
}

