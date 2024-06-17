//--------------------------------------------------
//! @file   BTStdMapKeyNameRefStructDetails.h
//! @brief  マップのキー名を参照する構造体の詳細カスタマイズ
//! @author BT.Masaki Okuda
//--------------------------------------------------
#pragma once

#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyCustomizationHelpers.h"

/**
 * @brief マップのキー名を参照する構造体の詳細カスタマイズクラス
 * @tparam MapValueType TMap<FName, XXX>のXXXに当たる部分
 */
template<typename MapValueType>
class TBTStdMapKeyNameReferencedStructDetails : public IPropertyTypeCustomization {
	// Begin IPropertyTypeCustomization Interface
public:
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual bool ShouldInlineKey() const override { return true; }
	// End IPropertyTypeCustomization Interface

protected:
	/** マップを取得 */
	virtual const TMap<FName, MapValueType>& GetMap(TSharedRef<IPropertyHandle> InPropHandle) const
	{
		static const TMap<FName, MapValueType> EmptyMap;
		return EmptyMap;
	}

	/** コンボボックスのリストにNoneを常に追加するか */
	virtual bool AlwaysAddNone(TSharedRef<IPropertyHandle> InPropHandle) const { return true; }
	/** コンボボックスに検索欄を表示する基準コンボ要素数を取得 */
	virtual int32 GetShowSearchForItemCount(TSharedRef<IPropertyHandle> InPropHandle) const { return 5; }
	/** コンボボックスとして表示するNamePropertyか */
	virtual bool IsShowComboBoxNameProperty(TSharedRef<IPropertyHandle> InPropHandle) const { return CastField<FNameProperty>(InPropHandle->GetProperty()) != nullptr; }
	/** ヘッダ部分にプロパティ表示する場合のプロパティハンドルを取得(Invalidの場合はヘッダで設定しない) */
	virtual TSharedPtr<IPropertyHandle> GetHeaderShowPropertyHandle() const { return TSharedPtr<IPropertyHandle>(); }
	/** コンボボックスとして表示する以外が存在している場合は詳細パラメータとして表示するか */
	virtual bool ShouldShowDetailParameter() const { return true; }

protected:
	/**
	 * @brief 構造体プロパティハンドルを取得
	 */
	const TSharedPtr<IPropertyHandle>& GetStructPropertyHandle() const { return StructPropertyHandle; }

private:
	/** 行名コンボボックスを作成 */
	TSharedRef<SWidget> MakeNameComboBoxWidget(const TSharedRef<IPropertyHandle>& InNamePropHandle)
	{
		FPropertyComboBoxArgs ComboBoxArgs;
		ComboBoxArgs.ShowSearchForItemCount = GetShowSearchForItemCount(InNamePropHandle);
		ComboBoxArgs.OnGetStrings = FOnGetPropertyComboBoxStrings::CreateSP(this, &TBTStdMapKeyNameReferencedStructDetails::GetNameList, InNamePropHandle);
		ComboBoxArgs.OnGetValue = FOnGetPropertyComboBoxValue::CreateStatic(&TBTStdMapKeyNameReferencedStructDetails::GetCurrentName, InNamePropHandle);
		ComboBoxArgs.OnValueSelected = FOnPropertyComboBoxValueSelected::CreateStatic(&TBTStdMapKeyNameReferencedStructDetails::OnNameSelected, InNamePropHandle);
		return PropertyCustomizationHelpers::MakePropertyComboBox(ComboBoxArgs);
	}

	/** Nameリストを取得 */
	// ReSharper disable once CppPassValueParameterByConstReference
	void GetNameList(TArray<TSharedPtr<FString>>& OutStrings, TArray<TSharedPtr<SToolTip>>& OutToolTip, TArray<bool>& OutRestrictedItems, TSharedRef<IPropertyHandle> InPropHandle) const
	{
		// 編集不可の場合はコンボボックスを表示しない
		if( InPropHandle->IsEditable() == false ) { return; }

		const TMap<FName, MapValueType>& Map = GetMap(InPropHandle);

		const int32 ReserveArrayNum = OutStrings.Num() + (AlwaysAddNone(InPropHandle) ? 1 : 0) + (Map.Num());
		OutStrings.Reserve(ReserveArrayNum);
		OutRestrictedItems.Reserve(ReserveArrayNum);
		OutToolTip.Reserve(ReserveArrayNum);

		if( AlwaysAddNone(InPropHandle) )
		{// Noneを追加
			OutStrings.Add(MakeShared<FString>(LexToString(EName::None)));
			OutRestrictedItems.Add(false);
			OutToolTip.Add(SNew(SToolTip).Text(FText::GetEmpty()));
		}

		for( const TTuple<FName, MapValueType> NameMapElm : Map )
		{
			if( NameMapElm.Key == NAME_None ) { continue; }

			OutStrings.Add(MakeShared<FString>(NameMapElm.Key.ToString()));
			OutRestrictedItems.Add(false);
			OutToolTip.Add(SNew(SToolTip).Text(FText::FromName(NameMapElm.Key)));
		}
	}

	/** 現在選択されているNameを取得 */
	// ReSharper disable once CppPassValueParameterByConstReference
	static FString GetCurrentName(TSharedRef<IPropertyHandle> InPropHandle)
	{
		FName CurrentBGMName;
		InPropHandle->GetValue(CurrentBGMName);
		return CurrentBGMName.ToString();
	}

	/** 行名が選択された時に呼ばれる */
	// ReSharper disable once CppPassValueParameterByConstReference
	static void OnNameSelected(const FString& InSelectedName, TSharedRef<IPropertyHandle> InPropHandle)
	{
		const FName SelectedName = FName(InSelectedName);
		InPropHandle->SetValue(SelectedName);
	}

private:
	/** 構造体プロパティ */
	TSharedPtr<IPropertyHandle> StructPropertyHandle;
	/** ヘッダ部分に表示するプロパティ */
	TSharedPtr<IPropertyHandle> ShowHeaderNamePropertyHandle;
};

template<typename MapValueType>
void TBTStdMapKeyNameReferencedStructDetails<MapValueType>::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	StructPropertyHandle = PropertyHandle;

	ShowHeaderNamePropertyHandle = GetHeaderShowPropertyHandle();

	if( ShowHeaderNamePropertyHandle.IsValid() && IsShowComboBoxNameProperty(ShowHeaderNamePropertyHandle.ToSharedRef()) == false )
	{// ヘッダ部分に表示するプロパティがコンボボックスとして表示できる対象でない場合は無効化
		ShowHeaderNamePropertyHandle.Reset();
	}

	HeaderRow.NameContent()
			[
				PropertyHandle->CreatePropertyNameWidget()
			]
			.ValueContent()
			[
				// 行名表示をヘッダ部分に表示する場合
				ShowHeaderNamePropertyHandle.IsValid() ? MakeNameComboBoxWidget(ShowHeaderNamePropertyHandle.ToSharedRef()) : PropertyHandle->CreatePropertyValueWidget()
			];
}

template<typename MapValueType>
void TBTStdMapKeyNameReferencedStructDetails<MapValueType>::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if( StructPropertyHandle->IsValidHandle() == false ) { return; }

	uint32 NumChildProperty = -1;
	const FPropertyAccess::Result ResultGetChildNum = StructPropertyHandle->GetNumChildren(NumChildProperty);
	if( ResultGetChildNum == FPropertyAccess::Success && NumChildProperty > 0 )
	{
		const FProperty* ShowHeaderNameProperty = (ShowHeaderNamePropertyHandle.IsValid()) ? ShowHeaderNamePropertyHandle->GetProperty() : nullptr;
		for( uint32 ChildIndex = 0; ChildIndex < NumChildProperty; ++ChildIndex )
		{
			const TSharedPtr<IPropertyHandle> ChildPropertyHandle = StructPropertyHandle->GetChildHandle(ChildIndex);
			if( ChildPropertyHandle.IsValid() == false ) { continue; }
			const FProperty* ChildProperty =  ChildPropertyHandle->GetProperty();

			// すでにヘッダ部分に表示しているプロパティは表示しない
			if(ChildProperty == ShowHeaderNameProperty) { continue; }
			
			if( IsShowComboBoxNameProperty(ChildPropertyHandle.ToSharedRef()) )
			{// 行名表示を詳細ウィンドウに表示する場合
				ChildBuilder.AddCustomRow(FText::GetEmpty())
							.NameContent()
						[
							ChildPropertyHandle->CreatePropertyNameWidget()
						]
						.ValueContent()
						[
							MakeNameComboBoxWidget(ChildPropertyHandle.ToSharedRef())
						];
			}
			else if( ShouldShowDetailParameter() )
			{// 行名表示プロパティ以外をプロパティ表示する場合
				ChildBuilder.AddProperty(ChildPropertyHandle.ToSharedRef());
			}
		}
	}
}
