//--------------------------------------------------
//! @file 	BTStdDTRowNameRefStructDetails.cpp
//! @brief 	データテーブルの行を参照する構造体の詳細カスタマイズクラス
//! @author  BT.Masaki Okuda
//--------------------------------------------------
#include "BTStdDTRowNameRefStructDetails.h"

#include "BTStdEditorPrivate.h"

const FName FBTStdDTRowNameRefStructDetails::NAME_ReferencedDTRowPropertyMeta(TEXT("ReferencedDTRow"));
const FName FBTStdDTRowNameRefStructDetails::NAME_DTRowHeaderStructMeta(TEXT("DTRowHeader"));


// ReSharper disable once CppPassValueParameterByConstReference /* const参照渡しはvoid(void)デリゲートでバインドキャプチャを行う為、できない */
TSharedRef<IPropertyTypeCustomization> FBTStdDTRowNameRefStructDetails::MakeInstance(FBTStdDataTableRowNameRefStructSettingElement InDetailParam)
{
	return MakeShareable<FBTStdDTRowNameRefStructDetails>(new FBTStdDTRowNameRefStructDetails(InDetailParam));
}

const TMap<FName, unsigned char*>& FBTStdDTRowNameRefStructDetails::GetMap(TSharedRef<IPropertyHandle> InPropHandle) const
{
	const FName PropName = InPropHandle->GetProperty()->GetFName();

	UDataTable* pDataTable = nullptr; // 参照するデータテーブル

	// プロパティ名からデータテーブルを取得する。できない場合は設定から取得する
	if( const TStrongObjectPtr<UDataTable>* pStrongPtrDT = RowReferencedDataTablePtrMap.Find(PropName) )
	{
		pDataTable = pStrongPtrDT->Get();
	}
	else if( const TSoftObjectPtr<UDataTable>* pDetailMapDT = DetailParam.RowReferencedDataTableMap.Find(PropName) )
	{
		pDataTable = pDetailMapDT->LoadSynchronous();
		if( pDataTable != nullptr )
		{
			// マップに追加
			RowReferencedDataTablePtrMap.Add(PropName, TStrongObjectPtr(pDataTable));
		}
	}

	if( pDataTable != nullptr )
	{
		return pDataTable->GetRowMap();
	}

	return TBTStdMapKeyNameReferencedStructDetails::GetMap(InPropHandle);
}

bool FBTStdDTRowNameRefStructDetails::IsShowComboBoxNameProperty(TSharedRef<IPropertyHandle> InPropHandle) const
{
	if( TBTStdMapKeyNameReferencedStructDetails::IsShowComboBoxNameProperty(InPropHandle) == false )
	{// 親クラス側の弾く判断に従う
		return false;
	}

	const FNameProperty* pNameProp = CastField<FNameProperty>(InPropHandle->GetProperty());
	if( pNameProp == nullptr ) { return false; }

	const FName PropName = pNameProp->GetFName();
	if( PropName.IsNone() ) { return false; }

	// メタデータが付与されていない
	if( pNameProp->HasMetaData(NAME_ReferencedDTRowPropertyMeta) == false ) { return false; }

	// データテーブルが設定されていない
	const TSoftObjectPtr<UDataTable>* pDataTableSoftPtr = DetailParam.RowReferencedDataTableMap.Find(PropName);
	if( pDataTableSoftPtr == nullptr || pDataTableSoftPtr->IsNull() )
	{
		UE_LOG(LogBTStdEditor, Error,
			TEXT("FBTStdDTRowNameRefStructDetails::IsShowComboBoxNameProperty : データテーブルが設定されていません。 設定してください。プロパティ名 = %s, 設定データ = %s"),  *PropName.ToString(), *DetailParam.ToString());
		return false;
	}

	return true;
}

TSharedPtr<IPropertyHandle> FBTStdDTRowNameRefStructDetails::GetHeaderShowPropertyHandle() const
{
	const TSharedPtr<IPropertyHandle> pStructHandle = GetStructPropertyHandle();
	if( pStructHandle.IsValid() == false ) { return TSharedPtr<IPropertyHandle>(); }

	const FStructProperty* pStructProp = CastField<FStructProperty>(pStructHandle->GetProperty());
	if( pStructProp == nullptr ) { return TSharedPtr<IPropertyHandle>(); }

	const UScriptStruct* pTargetStructProp = pStructProp->Struct;
	if( pTargetStructProp == nullptr ) { return TSharedPtr<IPropertyHandle>(); }

	// USTRUCTのmetaデータを取得
	const FString& DTRowHeaderPropNameMeta = pTargetStructProp->GetMetaData(NAME_DTRowHeaderStructMeta);
	const FString DTRowHeaderPropString = DTRowHeaderPropNameMeta.Replace(TEXT(" "), TEXT(""));
	if( DTRowHeaderPropString.IsEmpty() ) { return TSharedPtr<IPropertyHandle>(); }

	// metaからプロパティハンドルを取得 + プロパティハンドルがヘッダー箇所に表示できるFNamePropertyか検証
	TSharedPtr<IPropertyHandle> HeaderShowProperty = pStructHandle->GetChildHandle(FName(DTRowHeaderPropString));
	if( HeaderShowProperty.IsValid() == false )
	{
		UE_LOG(LogBTStdEditor, Error,
			TEXT("FBTStdDTRowNameRefStructDetails::GetHeaderShowPropertyHandle : meta[DTRowHeader]に指定されたプロパティが存在しません。構造体 = %s, meta = %s"), *pTargetStructProp->GetName(), *DTRowHeaderPropNameMeta);
		return TSharedPtr<IPropertyHandle>();
	}
	if( IsShowComboBoxNameProperty(HeaderShowProperty.ToSharedRef()) == false )
	{
		UE_LOG(LogBTStdEditor, Error,
			TEXT("FBTStdDTRowNameRefStructDetails::GetHeaderShowPropertyHandle : meta[DTRowHeader]に指定されたプロパティがコンボボックス対象にできません。構造体 = %s, プロパティ名 = %s"), *pTargetStructProp->GetName(), *DTRowHeaderPropString);
		return TSharedPtr<IPropertyHandle>();
	}
	return HeaderShowProperty;
}


/**
 * @brief コンストラクタ
 */
FBTStdDTRowNameRefStructDetails::FBTStdDTRowNameRefStructDetails(const FBTStdDataTableRowNameRefStructSettingElement& InDetailParam)
	: DetailParam(InDetailParam) { ; }
