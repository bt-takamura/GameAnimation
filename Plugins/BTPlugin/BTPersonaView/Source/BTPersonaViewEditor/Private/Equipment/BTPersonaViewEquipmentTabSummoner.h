//--------------------------------------------------
//! @file   BTPersonaViewEquipmentTabSummoner.h
//! @brief  ペルソナ機器アタッチ拡張機能用設定を開くためのタブサモナー
//--------------------------------------------------
#pragma once

#include "CoreMinimal.h"
// #include "AFPersonaExtensionSettingWidget.h"

#include "IAnimationEditor.h"
#include "SBTPersonaEquipmentAttachSettingWidget.h"

#include "WorkflowOrientedApp/WorkflowTabFactory.h"


#define LOCTEXT_NAMESPACE "BTPersonaEditorEquipmentSettings"

/**
 * @brief ペルソナ機器アタッチ拡張機能用設定を開くためのタブサモナー
 */
struct FBTPersonaViewEquipmentTabSummoner : public FWorkflowTabFactory {
public:
	/** Tab ID name */
	static const FName BTPersonaSettingsName;

	// ReSharper disable once CppPassValueParameterByConstReference
	explicit FBTPersonaViewEquipmentTabSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
		: FWorkflowTabFactory(BTPersonaSettingsName, InHostingApp)
	{
		TabLabel = LOCTEXT("BTPersonaEditorEquipmentSettings", "BTペルソナアタッチプレビュー表示設定");
		TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.ParticleSystem");
	}

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override
	{
		const TSharedPtr<FAssetEditorToolkit> HostingAppToolkit = HostingApp.Pin();

		// AnimationEditorのみIAnimationEditorでダウンキャスト
		// 参考：SSkeletonAnimNotifies::OnFindReferences()
		if( HostingAppToolkit.IsValid() &&
			HostingAppToolkit->GetEditorName() == TEXT("AnimationEditor") )
		{
			const TSharedPtr<IAnimationEditor> AnimationEditor = StaticCastSharedPtr<IAnimationEditor>(HostingAppToolkit);

			return SNew(SBTPersonaEquipmentAttachSettingWidget)
					.InPersonaToolkit(AnimationEditor->GetPersonaToolkit());
		}

		return SNullWidget::NullWidget;
	}

	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override
	{
		return LOCTEXT("BTPersonaEditorEquipmentSettings", "ペルソナ機器アタッチエディタの表示設定");
	}

	// ReSharper disable once CppPassValueParameterByConstReference
	static TSharedPtr<FWorkflowTabFactory> CreateFactory(TSharedPtr<FAssetEditorToolkit> InAssetEditor)
	{
		return MakeShareable(new FBTPersonaViewEquipmentTabSummoner(InAssetEditor));
	}
};

const FName FBTPersonaViewEquipmentTabSummoner::BTPersonaSettingsName = TEXT("BTPersonaSettings");

#undef LOCTEXT_NAMESPACE
