// Fill out your copyright notice in the Description page of Project Settings.

#include "BTStdEditorPreferenceSettings.h"

#include "BTStdDTRowNameRefStructBase.h"
#include "BTStdEditorPrivate.h"

#include "DTRowRefStruct/BTStdDTRowNameRefStructDetails.h"

//////////////////////////////////////////////////////////////////////////
//  FBTStdDataTableRowNameRefStructSettingElement
//////////////////////////////////////////////////////////////////////////

FBTStdDataTableRowNameRefStructSettingElement::FBTStdDataTableRowNameRefStructSettingElement() { ; }

/**
 * @brief データが有効か
 */
bool FBTStdDataTableRowNameRefStructSettingElement::IsValidData(bool InLowLevelCheck) const
{
	if( TargetStruct.IsNull() || RowReferencedDataTableMap.Num() == 0 )
	{// 簡易チェック失敗
		return false;
	}

	if( InLowLevelCheck )
	{// 完全チェック
		const UScriptStruct* pScriptStruct = TargetStruct.LoadSynchronous();
		if( pScriptStruct == nullptr ) { return false; }

		// 対象構造体がFBTStdDTRowNameRefStructBaseを継承しているか
		if( pScriptStruct->IsChildOf(FBTStdDTRowNameRefStructBase::StaticStruct()) == false || pScriptStruct == FBTStdDTRowNameRefStructBase::StaticStruct() )
		{
			return false;
		}

		// 一つでもマップに有効なデータがあればOK
		for( const TTuple<FName, TSoftObjectPtr<UDataTable>>& RowNameRef : RowReferencedDataTableMap )
		{
			if( RowNameRef.Key.IsNone() ) { continue;; }
			if( RowNameRef.Value.IsNull() ) { continue; }
			const FNameProperty* pTargetProperty = FindFProperty<FNameProperty>(pScriptStruct, RowNameRef.Key);
			if( pTargetProperty == nullptr ) { continue;; }

			if( pTargetProperty->HasMetaData(FBTStdDTRowNameRefStructDetails::NAME_ReferencedDTRowPropertyMeta) == false )
			{
				continue;
			}
			return true;
		}
	}
	else
	{
		return true;
	}

	return false;
}

/**
 * @brief 文字列化
 */
FString FBTStdDataTableRowNameRefStructSettingElement::ToString() const
{
	if( const UScriptStruct* ThisScriptStruct = StaticStruct() )
	{
		FString ExportText;
		ThisScriptStruct->ExportText(ExportText, this, this, nullptr, EPropertyPortFlags::PPF_None, nullptr);
		return ExportText;
	}
	return FString();
}

/**
 * @brief TargetStruct構造体からRowReferencedDataTableMapを変更
 */
void FBTStdDataTableRowNameRefStructSettingElement::UpdateRowReferencedDataTableMap()
{
	TMap<FName, TSoftObjectPtr<UDataTable>, TInlineSetAllocator<32>> NewRowReferencedDataTableMap;

	ON_SCOPE_EXIT
		{
			// 関数終了時にマップを従来プロパティに適用
			RowReferencedDataTableMap.Empty(NewRowReferencedDataTableMap.Num());
			RowReferencedDataTableMap.Append(NewRowReferencedDataTableMap);
		};

	UScriptStruct* pTargetScriptStruct = TargetStruct.LoadSynchronous();
	if( pTargetScriptStruct == nullptr ) { return; }
	if( pTargetScriptStruct->IsChildOf(FBTStdDTRowNameRefStructBase::StaticStruct()) == false || pTargetScriptStruct == FBTStdDTRowNameRefStructBase::StaticStruct() )
	{
		UE_LOG(LogBTStdEditor, Warning, TEXT("TargetStruct は FBTStdDTRowNameRefStructBase の子ではありません。ゼロクリアします。"));
		TargetStruct = nullptr;
		return;
	}

	// 構造体のメンバを巡回して対象になるNameプロパティを探す
	// 対象Nameプロパティの設定がすでに追加されている場合は追加パスを設定する
	for( const FNameProperty* pNamePropElm : TFieldRange<FNameProperty>(pTargetScriptStruct) )
	{
		if( pNamePropElm == nullptr ) { continue; }
		if( pNamePropElm->HasMetaData(FBTStdDTRowNameRefStructDetails::NAME_ReferencedDTRowPropertyMeta) == false ) { continue; }

		const FName NamePropName = pNamePropElm->GetFName();
		if( NamePropName.IsNone() ) { continue; }

		TSoftObjectPtr<UDataTable>& DataTableSoftObjPtrRef = NewRowReferencedDataTableMap.Add(NamePropName);

		// 構造体の親をたどっていき、設定されているDTパスを探す
		for( const UScriptStruct* pSuperScriptStruct = pTargetScriptStruct;
			pSuperScriptStruct != nullptr && pSuperScriptStruct != FBTStdDTRowNameRefStructBase::StaticStruct();
			pSuperScriptStruct = Cast<UScriptStruct>(pSuperScriptStruct->GetSuperStruct()) )
		{
			if( pSuperScriptStruct->IsChildOf(FBTStdDTRowNameRefStructBase::StaticStruct()) == false ) { continue; }
			const FBTStdDataTableRowNameRefStructSettingElement* pElmStructSetting = UBTStdEditorPreferenceSettings::FindDTRowRefStructSetting(pSuperScriptStruct);
			if( pElmStructSetting == nullptr ) { continue; }

			const TSoftObjectPtr<UDataTable>* pDataTableSoftObject = pElmStructSetting->RowReferencedDataTableMap.Find(NamePropName);
			if( pDataTableSoftObject == nullptr ) { continue; }
			if( pDataTableSoftObject->IsNull() ) { continue; }

			// すでに設定されているDTパスを登録
			DataTableSoftObjPtrRef = *pDataTableSoftObject;
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//  UBTStdEditorPreferenceSettings
//////////////////////////////////////////////////////////////////////////

/* コンストラクタ */
UBTStdEditorPreferenceSettings::UBTStdEditorPreferenceSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

/**
 * @brief Called when a property on this object has been modified externally
 */
void UBTStdEditorPreferenceSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

/**
 * @brief データテーブル行参照構造体の設定を取得
 */
const FBTStdDataTableRowNameRefStructSettingElement* UBTStdEditorPreferenceSettings::FindDTRowRefStructSetting(const UScriptStruct* InStruct)
{
	if( InStruct == nullptr ) { return nullptr; }

	const UBTStdEditorPreferenceSettings* pEditorPreferenceSetting = GetDefault<UBTStdEditorPreferenceSettings>();
	if( pEditorPreferenceSetting == nullptr ) { return nullptr; }

	TSoftObjectPtr<UScriptStruct> TargetStructSoftPtr(InStruct);

	const FBTStdDataTableRowNameRefStructSettingElement* pFindSettingElm = pEditorPreferenceSetting->DTRowRefStructs.FindByPredicate([TargetStructSoftPtr](const FBTStdDataTableRowNameRefStructSettingElement& InElement)
		{
			return InElement.TargetStruct.LoadSynchronous() == TargetStructSoftPtr;
		});

	return pFindSettingElm;
}
