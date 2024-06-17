//--------------------------------------------------
//! @file 	BTPersonaEquipmentComponentSettingCustomization.h
//! @brief 	Details customization for Persona Equipment Component settings
//! @auther BT.Masaki Okuda
//--------------------------------------------------

#pragma once
#include "IDetailCustomization.h"

class IPersonaPreviewScene;

/** Details customization for Persona Equipment Component settings */
class FBTPersonaEquipmentComponentSettingCustomization : public IDetailCustomization {
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<class IDetailCustomization> MakeInstance(TWeakObjectPtr<class UBTPersonaEquipmentAttachComponent> InPersonaAttachComponent, TWeakPtr<IPersonaPreviewScene> InPreviewScene, TSharedRef<SWidget> InParentWidget)
	{
		return MakeShareable(new FBTPersonaEquipmentComponentSettingCustomization(InPersonaAttachComponent, InPreviewScene, InParentWidget));
	}

	// Begin IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	// End IDetailCustomization interface

private:
	FBTPersonaEquipmentComponentSettingCustomization(
		TWeakObjectPtr<class UBTPersonaEquipmentAttachComponent> InPersonaAttachComponent,
		TWeakPtr<IPersonaPreviewScene> InPreviewScene,
		TSharedRef<SWidget> InParentWidget);

	/** 設定中のソケット名取得 */
	FText GetSocketName() const;
	/** ソケット変更可能か */
	bool CanChangeSocket() const;
	/** ソケット変更ボタン押下時 */
	void OnBrowseSocket();
	/** ソケットクリアボタン押下時 */
	void OnClearSocket();
	/** ソケット変更 */
	void ChangeSocket(FName InSocketName);
private:
	/** 親ウィジェット */
	TSharedRef<SWidget> ParentWidget;
	/** ペルソナプレビューScene */
	TWeakPtr<IPersonaPreviewScene> WeakPreviewScene;
	/** プレビュー用コンポーネント選択 */
	TStrongObjectPtr<class UBTPersonaEquipmentAttachComponent> PersonaAttachComponent;
	/** 変更中のコンポーネント */
	TStrongObjectPtr<class UActorComponent> EditingComponent;
};
