//--------------------------------------------------
//! @file 	BTStdDTRowNameRefStructSettingElementDetails.cpp
//! @brief	FBTStdDataTableRowNameRefStructSettingElementの詳細カスタマイズクラス
//! @author BT.Masaki Okuda
//--------------------------------------------------
#include "BTStdDTRowNameRefStructSettingElementDetails.h"

#include "BTStdDTRowNameRefStructBase.h"
#include "BTStdEditorPreferenceSettings.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"

TSharedRef<IPropertyTypeCustomization> FBTStdDTRowNameRefStructSettingElementDetails::MakeInstance()
{
	return MakeShareable<FBTStdDTRowNameRefStructSettingElementDetails>(new FBTStdDTRowNameRefStructSettingElementDetails());
}

void FBTStdDTRowNameRefStructSettingElementDetails::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	static const FName TargetStructPropertyName = GET_MEMBER_NAME_CHECKED(FBTStdDataTableRowNameRefStructSettingElement, TargetStruct);
	static const FName RowReferencedDataTableMapPropertyName = GET_MEMBER_NAME_CHECKED(FBTStdDataTableRowNameRefStructSettingElement, RowReferencedDataTableMap);

	StructPropertyHandle = PropertyHandle;
	checkf(StructPropertyHandle.IsValid(), TEXT("FBTStdDataTableRowNameRefStructSettingElementは有効なプロパティ名ではありません。"));

	TargetStructPropertyHandlePropertyHandle = StructPropertyHandle->GetChildHandle(TargetStructPropertyName);
	checkf(TargetStructPropertyHandlePropertyHandle.IsValid(), TEXT("%sは有効なプロパティ名ではありません。"), *TargetStructPropertyName.ToString());

	RowReferencedDataTableMapPropertyHandle = StructPropertyHandle->GetChildHandle(RowReferencedDataTableMapPropertyName);
	checkf(RowReferencedDataTableMapPropertyHandle.IsValid(), TEXT("%sは有効なプロパティ名ではありません。"), *RowReferencedDataTableMapPropertyName.ToString());

	TargetStructPropertyHandlePropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(
		this, &FBTStdDTRowNameRefStructSettingElementDetails::OnTargetStructValueChanged, true));

	HeaderRow.NameContent()
			[
				StructPropertyHandle->CreatePropertyNameWidget()
			]
			.ValueContent()
			[
				StructPropertyHandle->CreatePropertyValueWidget()
			];

	// パラメータ更新
	OnTargetStructValueChanged(false);
}

void FBTStdDTRowNameRefStructSettingElementDetails::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if( TargetStructPropertyHandlePropertyHandle.IsValid() )
	{
		ChildBuilder.AddProperty(TargetStructPropertyHandlePropertyHandle.ToSharedRef());
	}

	if( RowReferencedDataTableMapPropertyHandle.IsValid() )
	{
		ChildBuilder.AddProperty(RowReferencedDataTableMapPropertyHandle.ToSharedRef());
	}
}

/**
 * @brief 対象構造体Ptrを取得
 */
FBTStdDataTableRowNameRefStructSettingElement* FBTStdDTRowNameRefStructSettingElementDetails::GetRawStructPtr() const
{
	if( StructPropertyHandle.IsValid() == false ) { return nullptr; }

	// データptrを取得
	const FStructProperty* pStructProp = CastField<FStructProperty>(StructPropertyHandle->GetProperty());
	if( pStructProp == nullptr ) { return nullptr; }

	if( pStructProp->Struct == FBTStdDataTableRowNameRefStructSettingElement::StaticStruct() )
	{
		TArray<void*> StructPtrArray;
		StructPropertyHandle->AccessRawData(StructPtrArray);
		return (StructPtrArray.Num() == 1) ? static_cast<FBTStdDataTableRowNameRefStructSettingElement*>(StructPtrArray[0]) : nullptr;
	}
	return nullptr;
}

/**
 * @brief 設定の対象としている構造体変更時の関数
 */
void FBTStdDTRowNameRefStructSettingElementDetails::OnTargetStructValueChanged(bool bNotifyChange) const
{
	FBTStdDataTableRowNameRefStructSettingElement* pRawStructPtr = GetRawStructPtr();
	if( pRawStructPtr == nullptr ) { return; }

	const UScriptStruct* pTargetScriptStruct = pRawStructPtr->TargetStruct.LoadSynchronous();

	//////////////////////////////////////////////////////////////////////////
	//  対象構造体が有効か確認
	//////////////////////////////////////////////////////////////////////////
	bool bValidTargetStruct = true;

	if( pTargetScriptStruct != nullptr )
	{
		if( pTargetScriptStruct == FBTStdDTRowNameRefStructBase::StaticStruct() )
		{
			// 構造体はFBTStdDTRowNameRefStructBase以外にする必要がある。
			if( bNotifyChange )
			{
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("FBTStdDTRowNameRefStructBase以外の構造体を指定してください。")));
			}
			bValidTargetStruct = false;
		}
		else if( pTargetScriptStruct->IsChildOf(FBTStdDTRowNameRefStructBase::StaticStruct()) == false )
		{
			// FBTStdDTRowNameRefStructBaseを継承する必要がある。
			if( bNotifyChange )
			{
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("対象構造体はFBTStdDTRowNameRefStructBaseを継承している必要があります。")));
			}
			bValidTargetStruct = false;
		}
	}

	if( bValidTargetStruct == false && TargetStructPropertyHandlePropertyHandle.IsValid() )
	{// 有効でない構造体の場合はnullにする
		constexpr UObject* const pNullPtr = nullptr;
		TargetStructPropertyHandlePropertyHandle->SetValue(pNullPtr);
	}

	//////////////////////////////////////////////////////////////////////////
	//  マップの更新
	//////////////////////////////////////////////////////////////////////////

	if( bNotifyChange && RowReferencedDataTableMapPropertyHandle.IsValid() )
	{
		RowReferencedDataTableMapPropertyHandle->NotifyPreChange();
	}

	// 構造体型にあわせてたマップに変更
	pRawStructPtr->UpdateRowReferencedDataTableMap();

	if( bNotifyChange && RowReferencedDataTableMapPropertyHandle.IsValid() )
	{
		RowReferencedDataTableMapPropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
	}
}
