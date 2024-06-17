//--------------------------------------------------
//! @file 	BTStdDTRowNameRefStructSettingElementDetails.h
//! @brief	FBTStdDataTableRowNameRefStructSettingElementの詳細カスタマイズクラス
//! @author BT.Masaki Okuda
//--------------------------------------------------

#pragma once

#include "IPropertyTypeCustomization.h"


/**
 * @brief FBTStdDataTableRowNameRefStructSettingElementの詳細カスタマイズクラス
 */
class FBTStdDTRowNameRefStructSettingElementDetails : public IPropertyTypeCustomization {
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	// Begin IPropertyTypeCustomization Interface
public:
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	// End IPropertyTypeCustomization Interface

private:
	/** 対象構造体Ptrを取得 */
	struct FBTStdDataTableRowNameRefStructSettingElement* GetRawStructPtr() const;
	
	/** 設定の対象としている構造体変更時の関数 */
	void OnTargetStructValueChanged(bool bNotifyChange) const;
private:
	/** 構造体プロパティハンドル */
	TSharedPtr<IPropertyHandle> StructPropertyHandle;
	/** 設定の対象としている構造体型プロパティハンドル */
	TSharedPtr<IPropertyHandle> TargetStructPropertyHandlePropertyHandle;
	/** データテーブル参照マッププロパティハンドル */
	TSharedPtr<IPropertyHandle> RowReferencedDataTableMapPropertyHandle;
};
