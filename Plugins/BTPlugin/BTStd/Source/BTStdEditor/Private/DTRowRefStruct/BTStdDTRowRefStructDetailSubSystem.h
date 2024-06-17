//--------------------------------------------------
//! @file   BTStdDTRowRefStructDetailSubSystem.h
//! @brief  データテーブル行参照構造体表示サブシステム
//! @author  BT.Masaki Okuda
//--------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"

#include "BTStdDTRowRefStructDetailSubSystem.generated.h"

/**
 * @brief データテーブル行参照構造体表示サブシステム
 */
UCLASS()
class BTSTDEDITOR_API UBTStdDTRowRefStructDetailSubSystem : public UEditorSubsystem {
	GENERATED_BODY()

	// Begin UEditorSubsystem Interface
public:
	virtual void Deinitialize() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// End UEditorSubsystem Interface

private:
	/** プロパティ表示拡張を追加する*/
	void RegisterCustomPropertyTypeLayout(::FPropertyEditorModule& InEditorModule, const UScriptStruct* InTypeStruct, const FOnGetPropertyTypeCustomizationInstance& InDetailMakerDelegate);
private:
	/** プロパティエディタモジュールを取得 */
	static FPropertyEditorModule& GetPropertyEditorModule();

	/** 登録済みの構造体型名 */
	TArray<FName> RegisteredStructTypeNames;
};
