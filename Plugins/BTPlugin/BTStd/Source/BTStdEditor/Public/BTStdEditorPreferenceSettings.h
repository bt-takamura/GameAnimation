// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"


#include "BTStdEditorPreferenceSettings.generated.h"

/**
 * @brief データテーブル参照構造体設定の配列要素型
 * @see UBTStdEditorPreferenceSettings::DTRowRefStructs
 */
USTRUCT()
struct FBTStdDataTableRowNameRefStructSettingElement {
	GENERATED_BODY()

	FBTStdDataTableRowNameRefStructSettingElement();

public:
	/** 表示を拡張する構造体型 */
	UPROPERTY(EditAnywhere, Config, DisplayName = "構造体型", meta = (MetaStruct = "/Script/BTStd.BTStdDTRowNameRefStructBase"))
	TSoftObjectPtr<UScriptStruct> TargetStruct;

	/** データテーブル参照マップ
	 * @note データテーブル参照マップのキーは meta = (ReferencedDTRow) があるFNameプロパティ名(DisplayNameではない)
	 */
	UPROPERTY(EditAnywhere, Config, DisplayName = "データテーブル参照マップ", EditFixedSize, meta = (ReadOnlyKeys, EditFixedOrder, ForceInlineRow, DisplayThumbnail = false))
	TMap<FName, TSoftObjectPtr<UDataTable>> RowReferencedDataTableMap;

public:
	/** データが有効か */
	bool IsValidData(bool InLowLevelCheck = true) const;
	/** 文字列化 */
	FString ToString() const;
	/** TargetStruct構造体からRowReferencedDataTableMapを変更 */
	void UpdateRowReferencedDataTableMap();
	
};

// UEエディタのBTStd独自の機能拡張
//UCLASS(minimalapi, config=EditorSettings, meta=(DisplayName = "BTStd Preference", ToolTip="Perferences of the editor"))
UCLASS(config=EditorSettings, meta=(DisplayName = "BT Preference", ToolTip="Perferences of the editor"))
class BTSTDEDITOR_API UBTStdEditorPreferenceSettings : public UDeveloperSettings {
	GENERATED_UCLASS_BODY()

public:
	//////////////////////////////////////////////////////////////////////////
	//  AutoMoveFolderToPlacedActor
	//////////////////////////////////////////////////////////////////////////

	// Outlinerで選択しているフォルダに自動でActorを移動する処理
	UPROPERTY(EditAnywhere, config, Category = "LevelEditor")
	bool bEnableAutoMoveFolderToPlacedActor = false;

	//////////////////////////////////////////////////////////////////////////
	//  PropertyEditor
	//////////////////////////////////////////////////////////////////////////

	// データテーブルの行名参照を行う構造体の設定
	UPROPERTY(EditAnywhere, Config, Category = "PropertyEditor", DisplayName = "データテーブル行参照構造体の表示設定", meta = (TitleProperty = "TargetStruct", EditCondition = "bEnableShowDTRowRefStructs", ConfigRestartRequired = true))
	TArray<FBTStdDataTableRowNameRefStructSettingElement> DTRowRefStructs;

	// データテーブルの行名参照を行う構造体表示を有効にするか
	UPROPERTY(EditAnywhere, Config, Category = "PropertyEditor", DisplayName = "データテーブル行参照構造体の表示を有効にする", meta = (ConfigRestartRequired = true))
	bool bEnableShowDTRowRefStructs = false;

public:
	// Begin UDeveloperSettings Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End UDeveloperSettings Interface
	
	/** データテーブル行参照構造体の設定を取得 */
	static const FBTStdDataTableRowNameRefStructSettingElement* FindDTRowRefStructSetting(const UScriptStruct* InStruct);
};
