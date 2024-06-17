//--------------------------------------------------
//! @file   BTPersonaEquipmentAttachSubsystem.h
//! @brief  ペルソナ装備アタッチサブシステム
//! @author BT.Masaki Okuda
//--------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "BTPersonaEquipmentAttachSubsystem.generated.h"

class IPersonaPreviewScene;
class FApplicationMode;
/**
 * ペルソナ装備アタッチサブシステム
 * @note 外部モジュールから挙動設定等アクセスする必要がでた場合はFBTPersonaViewEditorModule経由でアクセスできるようにする
 */
UCLASS()
class BTPERSONAVIEWEDITOR_API UBTPersonaEquipmentAttachSubsystem : public UEditorSubsystem {
	GENERATED_BODY()


	// Begin UEditorSubsystem Interface
public:
	virtual void Deinitialize() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// End UEditorSubsystem Interface
private:
	/** ウィンドウ作成時などでアプリケーション拡張タイミングで呼ばれるデリゲート */
	TSharedRef<FApplicationMode> ExtendApplicationMode(const FName InModeName, TSharedRef<FApplicationMode> InMode);
	/** モジュール変更時に呼ばれるデリゲート */
	void HandleModulesChanged(FName InModuleNem, EModuleChangeReason InReason);
	/** ペルソナプレビューScene作成時に呼ばれるデリゲート */
	void OnPersonaPreviewSceneCreated(const TSharedRef<IPersonaPreviewScene>& InPreviewScene);
	/** アプリケーションからタブを削除 */
	void RemoveTabFactory(bool bRemoveOnlyInvalidMode = true);
private:
	static const FName PersonaModuleName; //!< ペルソナモジュール名

	struct FTabRegisteredModeInfo {
		TWeakPtr<FApplicationMode> Mode;
		FName TabID;
		/** 有効なモードインスタンスか */
		bool IsValidMode() const { return Mode.IsValid(); }
	};

	/** タブ拡張設定しているアプリケーション */
	TArray<FTabRegisteredModeInfo> TabRegisteredApplicationModes;
};
