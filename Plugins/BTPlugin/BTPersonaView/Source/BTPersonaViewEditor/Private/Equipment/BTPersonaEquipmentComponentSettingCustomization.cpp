//--------------------------------------------------
//! @file 	BTPersonaEquipmentComponentSettingCustomization.h
//! @brief 	Details customization for Persona Equipment Component settings
//! @auther BT.Masaki Okuda
//--------------------------------------------------
#include "BTPersonaEquipmentComponentSettingCustomization.h"

#include "BTPersonaEquipmentAttachComponent.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IPersonaPreviewScene.h"
#include "PropertyCustomizationHelpers.h"
#include "SSocketChooser.h"

#include "Animation/DebugSkelMeshComponent.h"

void FBTPersonaEquipmentComponentSettingCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	for( TWeakObjectPtr<UObject> WeakSelectedObject : DetailBuilder.GetSelectedObjects() )
	{
		UObject* SelectedObject = WeakSelectedObject.Get();
		if( SelectedObject == nullptr ) { continue; }
		EditingComponent = TStrongObjectPtr<UActorComponent>(Cast<UActorComponent>(SelectedObject));
		if( EditingComponent.IsValid() ) { break; }
	}

	// コンポーネントが取得できなかったら終了
	if( EditingComponent.IsValid() == false ) { return; }

	// アタッチ設定で表示が不要そうなカテゴリを非表示にする
	DetailBuilder.HideCategory(TEXT("Collision"));
	DetailBuilder.HideCategory(TEXT("HLOD"));
	DetailBuilder.HideCategory(TEXT("LOD"));
	DetailBuilder.HideCategory(TEXT("Replication"));
	DetailBuilder.HideCategory(TEXT("Component Replication"));
	DetailBuilder.HideCategory(TEXT("Cooking"));
	DetailBuilder.HideCategory(TEXT("Activation"));

	const FText SocketDisplayText = FText::FromString(TEXT("ソケット"));
	IDetailCategoryBuilder& SocketsCategory = DetailBuilder.EditCategory("Sockets", SocketDisplayText, ECategoryPriority::Important);

	SocketsCategory.AddCustomRow(SocketDisplayText)
					.NameContent()
			[
				SNew(STextBlock)
				.Text(SocketDisplayText)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
			.ValueContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SEditableTextBox)
					.Text(this, &FBTPersonaEquipmentComponentSettingCustomization::GetSocketName)
					.IsReadOnly(true)
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(2.0f, 1.0f)
				[
					PropertyCustomizationHelpers::MakeBrowseButton(
						FSimpleDelegate::CreateSP(this, &FBTPersonaEquipmentComponentSettingCustomization::OnBrowseSocket),
						FText::FromString(TEXT("ソケット名をブラウザから選択")),
						TAttribute<bool>(this, &FBTPersonaEquipmentComponentSettingCustomization::CanChangeSocket)
						)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(2.0f, 1.0f)
				[
					PropertyCustomizationHelpers::MakeClearButton(
						FSimpleDelegate::CreateSP(this, &FBTPersonaEquipmentComponentSettingCustomization::OnClearSocket),
						FText::FromString(TEXT("ソケット名をクリア")),
						TAttribute<bool>(this, &FBTPersonaEquipmentComponentSettingCustomization::CanChangeSocket)
						)
				]
			];
}

FBTPersonaEquipmentComponentSettingCustomization::FBTPersonaEquipmentComponentSettingCustomization(TWeakObjectPtr<UBTPersonaEquipmentAttachComponent> InPersonaAttachComponent, TWeakPtr<IPersonaPreviewScene> InPreviewScene, TSharedRef<SWidget> InParentWidget)
	: ParentWidget(InParentWidget)
	, WeakPreviewScene(InPreviewScene)
	, PersonaAttachComponent(InPersonaAttachComponent.Get()) { ; }

/**
 * @brief 設定中のソケット名取得
 */
FText FBTPersonaEquipmentComponentSettingCustomization::GetSocketName() const
{
	if( PersonaAttachComponent.IsValid() && EditingComponent.IsValid() )
	{
		const FBTPersonaEquipmentAttachComponentSpawnInfo* FindSpawnInfo = PersonaAttachComponent->FindSpawnedComponentInfoFromTemplateInstanceUniqueID(EditingComponent->GetUniqueID());

		if( FindSpawnInfo != nullptr )
		{
			return FText::FromName(FindSpawnInfo->EquipmentComponentData.AttachSocketName);
		}
	}
	return FText();
}

/**
 * @brief ソケット変更可能か
 */
bool FBTPersonaEquipmentComponentSettingCustomization::CanChangeSocket() const
{
	// ソケット変更はSceneComponent継承のみ可能
	return EditingComponent.IsValid() && EditingComponent->IsA<USceneComponent>();
}

/**
 * @brief ソケット変更ボタン押下時
 */
void FBTPersonaEquipmentComponentSettingCustomization::OnBrowseSocket()
{
	TSharedPtr<IPersonaPreviewScene> PreviewScene = WeakPreviewScene.Pin();
	if( PreviewScene.IsValid() == false ) { return; }

	if( USceneComponent* ParentSceneComponent = PreviewScene->GetPreviewMeshComponent() )
	{
		if( ParentSceneComponent->HasAnySockets() )
		{
			// Pop up a combo box to pick socket from mesh
			FSlateApplication::Get().PushMenu(
				ParentWidget,
				FWidgetPath(),
				SNew(SSocketChooserPopup)
				.SceneComponent(ParentSceneComponent)
				.OnSocketChosen(this, &FBTPersonaEquipmentComponentSettingCustomization::ChangeSocket),
				FSlateApplication::Get().GetCursorPos(),
				FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup)
				);
		}
	}
}

/**
 * @brief ソケットクリアボタン押下時
 */
void FBTPersonaEquipmentComponentSettingCustomization::OnClearSocket()
{
	ChangeSocket(NAME_None);
}

/**
 * @brief ソケット変更
 */
// ReSharper disable once CppMemberFunctionMayBeConst
void FBTPersonaEquipmentComponentSettingCustomization::ChangeSocket(FName InSocketName)
{
	if( PersonaAttachComponent.IsValid() && EditingComponent.IsValid() )
	{
		PersonaAttachComponent->RequestChangeSocketFromTemplateInstanceUniqueID(EditingComponent->GetUniqueID(), InSocketName);
	}
}
