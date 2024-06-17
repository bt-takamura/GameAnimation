//--------------------------------------------------
//! @file   BTPersonaViewEditorPreferenceSettings.h
//! @brief  ペルソナ表示エディターの設定
//! @author BT.Masaki Okuda
//--------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BTPersonaViewEditorPreferenceSettings.generated.h"

/**
 * ペルソナ表示エディターの設定
 */
UCLASS(Config=EditorSettings, meta=(DisplayName = "BT Persona View Editor Preference", ToolTip="BTペルソナ拡張機能設定"))
class BTPERSONAVIEWEDITOR_API UBTPersonaViewEditorPreferenceSettings : public UDeveloperSettings {
	GENERATED_BODY()

public:
	//////////////////////////////////////////////////////////////////////////
	//  Equipment
	//////////////////////////////////////////////////////////////////////////

	/** ペルソナメッシュに機器をアタッチする機能を有効にするか */
	UPROPERTY(EditAnywhere, Config, Category = "Equipment", DisplayName = "機器アタッチ機能を有効にするか", meta = (ConfigRestartRequired = true))
	bool bEnableAttachEquipment = false;;

	/** ペルソナメッシュに自動的に機器をアタッチするか */
	UPROPERTY(EditAnywhere, Config, Category = "Equipment", DisplayName = "自動アタッチを有効にするか", meta = (EditCondition = "bEnableAttachEquipment"))
	bool bEnableAutoAttachEquipment = false;

	/** アタッチする機器を提供するアセットパスリスト */
	UPROPERTY(EditAnywhere, Config, Category = "Equipment", DisplayName = "機器情報提供アセット一覧", meta = (EditCondition = "bEnableAttachEquipment", AllowedClasses = "/Script/BTPersonaView.BTPersonaEquipmentProviderAssetInterface", ConfigRestartRequired = true))
	TArray<FSoftObjectPath> EquipmentProviderAssetPaths;


public:
	// Begin UObject Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End UObject Interface
};
