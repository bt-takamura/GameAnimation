//--------------------------------------------------
//! @file 	BTStdDTRowNameRefStructDetails.h
//! @brief 	データテーブルの行を参照する構造体の詳細カスタマイズクラス
//! @author  BT.Masaki Okuda
//--------------------------------------------------

#pragma once
#include "BTStdEditorPreferenceSettings.h"
#include "BTStdMapKeyNameRefStructDetails.h"

/**
 * @brief データテーブルの行を参照する構造体の詳細カスタマイズクラス
 */
class FBTStdDTRowNameRefStructDetails : public TBTStdMapKeyNameReferencedStructDetails<uint8*> {
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance(FBTStdDataTableRowNameRefStructSettingElement InDetailParam);

	// Begin TBTStdMapKeyNameReferencedStructDetails Interface
protected:
	virtual const TMap<FName, unsigned char*>& GetMap(TSharedRef<IPropertyHandle> InPropHandle) const override;
	virtual bool IsShowComboBoxNameProperty(TSharedRef<IPropertyHandle> InPropHandle) const override;
	virtual TSharedPtr<IPropertyHandle> GetHeaderShowPropertyHandle() const override;
	// End TBTStdMapKeyNameReferencedStructDetails Interface
private:
	explicit FBTStdDTRowNameRefStructDetails(const FBTStdDataTableRowNameRefStructSettingElement& InDetailParam);

public:
	/** コンボボックス表示を行うプロパティメタ */
	static const FName NAME_ReferencedDTRowPropertyMeta;
	/** ヘッダ部分に表示する用の構造体型のメタ */
	static const FName NAME_DTRowHeaderStructMeta;

private:
	/** 表示構造体設定 */
	FBTStdDataTableRowNameRefStructSettingElement DetailParam;
	/** 参照データテーブルMap */
	mutable TMap<FName, TStrongObjectPtr<UDataTable>> RowReferencedDataTablePtrMap;
};
