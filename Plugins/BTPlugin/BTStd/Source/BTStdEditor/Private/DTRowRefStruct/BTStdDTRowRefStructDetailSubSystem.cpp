//--------------------------------------------------
//! @file   BTStdDTRowRefStructDetailSubSystem.cpp
//! @brief  データテーブル行参照構造体表示サブシステム
//! @author  BT.Masaki Okuda
//--------------------------------------------------
#include "BTStdDTRowRefStructDetailSubSystem.h"

#include "BTStdDTRowNameRefStructDetails.h"
#include "BTStdDTRowNameRefStructSettingElementDetails.h"
#include "BTStdEditorPreferenceSettings.h"
#include "BTStdEditorPrivate.h"

void UBTStdDTRowRefStructDetailSubSystem::Deinitialize()
{
	Super::Deinitialize();

	FPropertyEditorModule& PropertyModule = GetPropertyEditorModule();

	// 登録した構造体型を解除
	for( const FName& TypeName : RegisteredStructTypeNames )
	{
		PropertyModule.UnregisterCustomPropertyTypeLayout(TypeName);
	}
}

void UBTStdDTRowRefStructDetailSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FPropertyEditorModule& PropertyModule = GetPropertyEditorModule();

	// 設定で使用する構造体の表示拡張も行う
	RegisterCustomPropertyTypeLayout(PropertyModule, FBTStdDataTableRowNameRefStructSettingElement::StaticStruct(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FBTStdDTRowNameRefStructSettingElementDetails::MakeInstance));

	// 設定から表示拡張する構造体を選択する
	if( const UBTStdEditorPreferenceSettings* pEditorPreferenceSetting = GetDefault<UBTStdEditorPreferenceSettings>();
		pEditorPreferenceSetting != nullptr && pEditorPreferenceSetting->bEnableShowDTRowRefStructs )
	{
		TSet<const UScriptStruct*, DefaultKeyFuncs<const UScriptStruct*>, TInlineSetAllocator<32>> RegisteredStructs;

		for( const FBTStdDataTableRowNameRefStructSettingElement& SettingElm : pEditorPreferenceSetting->DTRowRefStructs )
		{
			// 設定データが有効か検証
			if( SettingElm.IsValidData() == false )
			{
				UE_LOG(LogBTStdEditor, Error, TEXT("UBTStdDTRowRefStructDetailSubSystem::Initialize : EditorSettingで設定されたデータが不足しています。[データテーブル行参照構造体の表示設定]の内容を確認してください。\n 設定データ : %s"), *SettingElm.ToString());
				continue;
			}

			const UScriptStruct* pTargetStruct = SettingElm.TargetStruct.LoadSynchronous();
			if( pTargetStruct == nullptr )
			{
				UE_LOG(LogBTStdEditor, Error, TEXT("UBTStdDTRowRefStructDetailSubSystem::Initialize : EditorSettingで設定された構造体が存在しません。[データテーブル行参照構造体の表示設定]の内容を確認してください。\n 設定データ : %s"), *SettingElm.ToString());
				continue;
			}

			if( RegisteredStructs.Contains(pTargetStruct) )
			{// 同じ型がすでに登録されている
				UE_LOG(LogBTStdEditor, Error, TEXT("UBTStdDTRowRefStructDetailSubSystem::Initialize : EditorSettingで設定された構造体が重複しています。[データテーブル行参照構造体の表示設定]の内容を確認してください。\n 設定データ : %s"), *SettingElm.ToString());
				continue;
			}

			// 登録
			RegisteredStructs.Add(pTargetStruct);
			RegisterCustomPropertyTypeLayout(PropertyModule, pTargetStruct, FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FBTStdDTRowNameRefStructDetails::MakeInstance, SettingElm));
		}
	}

	PropertyModule.NotifyCustomizationModuleChanged();
}

bool UBTStdDTRowRefStructDetailSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

/**
 * @brief プロパティ表示拡張を追加する
 */
void UBTStdDTRowRefStructDetailSubSystem::RegisterCustomPropertyTypeLayout(FPropertyEditorModule& InEditorModule, const UScriptStruct* InTypeStruct, const FOnGetPropertyTypeCustomizationInstance& InDetailMakerDelegate)
{
	const FName StructName = (InTypeStruct != nullptr) ? InTypeStruct->GetFName() : EName::None;
	if( StructName.IsNone() ) { return; }

	InEditorModule.RegisterCustomPropertyTypeLayout(StructName, InDetailMakerDelegate);
	RegisteredStructTypeNames.AddUnique(StructName);
}

/**
 * @brief プロパティエディタモジュールを取得
 */
FPropertyEditorModule& UBTStdDTRowRefStructDetailSubSystem::GetPropertyEditorModule()
{
	static const FName PropertyEditorName = TEXT("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(PropertyEditorName);
	return PropertyModule;
}
