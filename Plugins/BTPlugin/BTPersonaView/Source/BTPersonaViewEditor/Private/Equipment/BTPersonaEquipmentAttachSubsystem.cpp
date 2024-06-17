//--------------------------------------------------
//! @file   BTPersonaEquipmentAttachSubsystem.cpp
//! @brief  ペルソナ装備アタッチサブシステム
//! @author BT.Masaki Okuda
//--------------------------------------------------
#include "BTPersonaEquipmentAttachSubsystem.h"

#include "BTPersonaEquipmentAttachComponent.h"
#include "BTPersonaViewEditorPreferenceSettings.h"
#include "BTPersonaViewEditorPrivate.h"
#include "BTPersonaViewEquipmentTabSummoner.h"
#include "IPersonaPreviewScene.h"
#include "IPersonaToolkit.h"
#include "PersonaModule.h"

#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

const FName UBTPersonaEquipmentAttachSubsystem::PersonaModuleName("Persona");

void UBTPersonaEquipmentAttachSubsystem::Deinitialize()
{
	Super::Deinitialize();

	// アプリケーションの拡張用デリゲート解除
	FWorkflowCentricApplication::GetModeExtenderList().RemoveAll([this](const FWorkflowApplicationModeExtender& StoredExtender)
		{
			if( const UObject* pStoredObject = StoredExtender.GetUObject() )
			{
				return pStoredObject->GetUniqueID() == this->GetUniqueID();
			}
			return false;
		});

	// デリゲートバインド解除
	FModuleManager::Get().OnModulesChanged().RemoveAll(this);
	HandleModulesChanged(PersonaModuleName, EModuleChangeReason::ModuleUnloaded);

	// タブ削除
	RemoveTabFactory(false);
}


void UBTPersonaEquipmentAttachSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if( IsRunningCommandlet() ) { return; }

	// アプリケーションの拡張用デリゲート登録
	FWorkflowCentricApplication::GetModeExtenderList().Add(
		FWorkflowApplicationModeExtender::CreateUObject(this, &ThisClass::ExtendApplicationMode));


	// この時点ではモジュールが読み込まれていないのでモジュール変更デリゲートでモジュールロードを確認する
	FModuleManager::Get().OnModulesChanged().AddUObject(this, &ThisClass::HandleModulesChanged);

	// ...一応ロード済みか確認
	if( FModuleManager::Get().IsModuleLoaded(PersonaModuleName) )
	{
		HandleModulesChanged(PersonaModuleName, EModuleChangeReason::ModuleLoaded);
	}
}

bool UBTPersonaEquipmentAttachSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if( const UBTPersonaViewEditorPreferenceSettings* PersonaSetting = GetDefault<UBTPersonaViewEditorPreferenceSettings>() )
	{
		return PersonaSetting->bEnableAttachEquipment;
	}
	return false;
}

/**
 * @brief ウィンドウ作成時などでアプリケーション拡張タイミングで呼ばれるデリゲート
 */
TSharedRef<FApplicationMode> UBTPersonaEquipmentAttachSubsystem::ExtendApplicationMode(const FName InModeName, TSharedRef<FApplicationMode> InMode)
{
	// For animation editor modes add our custom tab factory to it
	if( InModeName == TEXT("AnimationEditorMode") )
	{
		// ペルソナ設定ウィンドウ表示用ウィジェット追加
		InMode->AddTabFactory(FCreateWorkflowTabFactory::CreateStatic(&FBTPersonaViewEquipmentTabSummoner::CreateFactory));
		TabRegisteredApplicationModes.Add({ InMode, FBTPersonaViewEquipmentTabSummoner::BTPersonaSettingsName });
	}
	RemoveTabFactory();
	return InMode;
}

/**
 * @brief モジュール変更時に呼ばれるデリゲート
 */
void UBTPersonaEquipmentAttachSubsystem::HandleModulesChanged(FName InModuleNem, EModuleChangeReason InReason)
{
	if( InModuleNem == PersonaModuleName )
	{
		switch( InReason )
		{
		case EModuleChangeReason::ModuleLoaded : {
			FPersonaModule& PersonaModule = FModuleManager::GetModuleChecked<FPersonaModule>(PersonaModuleName);
			PersonaModule.OnPreviewSceneCreated().AddUObject(this, &ThisClass::OnPersonaPreviewSceneCreated);
			break;
		}

		case EModuleChangeReason::ModuleUnloaded : {
			if( FModuleManager::Get().IsModuleLoaded(PersonaModuleName) )
			{
				FPersonaModule& PersonaModule = FModuleManager::GetModuleChecked<FPersonaModule>(PersonaModuleName);
				PersonaModule.OnPreviewSceneCreated().RemoveAll(this);
			}
			break;
		}
		case EModuleChangeReason::PluginDirectoryChanged : break;
		default :
			UE_LOG(LogBTPersonaViewEditor, Fatal,
				TEXT("UBTPersonaEquipmentAttachSubsystem::HandleModulesChanged; 不明なモジュール変更理由です。case処理を追記してください。Reason: %d"), (int32)InReason);
		}
	}
}

/**
 * @brief ペルソナプレビューScene作成時に呼ばれるデリゲート
 */
// ReSharper disable once CppMemberFunctionMayBeStatic
void UBTPersonaEquipmentAttachSubsystem::OnPersonaPreviewSceneCreated(const TSharedRef<IPersonaPreviewScene>& InPreviewScene)
{
	// 念のため、機能が有効になっているかチェック(エディタ設定で無効になっている場合は何もしない)
	if( const UBTPersonaViewEditorPreferenceSettings* PersonaSetting = GetDefault<UBTPersonaViewEditorPreferenceSettings>();
		PersonaSetting != nullptr && PersonaSetting->bEnableAttachEquipment == false )
	{
		return;
	}

	const UAnimationAsset* pAnimationAsset = InPreviewScene->GetPersonaToolkit()->GetAnimationAsset();
	if( pAnimationAsset == nullptr )return;

	AActor* pPreviewActor = InPreviewScene->GetActor();
	if( pPreviewActor == nullptr )return;

	// コンポ―ネント生成
	if( UBTPersonaEquipmentAttachComponent* pCreatedComponent = NewObject<UBTPersonaEquipmentAttachComponent>(pPreviewActor) )
	{
		pCreatedComponent->SetPersonaPreviewScene(InPreviewScene);
		InPreviewScene->AddComponent(pCreatedComponent, FTransform::Identity);
	}
}

/**
 * @brief アプリケーションからタブを削除
 */
void UBTPersonaEquipmentAttachSubsystem::RemoveTabFactory(bool bRemoveOnlyInvalidMode)
{
	for( auto Itr = TabRegisteredApplicationModes.CreateIterator(); Itr; ++Itr )
	{
		if( Itr->IsValidMode() && bRemoveOnlyInvalidMode == true )
		{
			continue;
		}

		if( const TSharedPtr<FApplicationMode> Mode = Itr->Mode.Pin() )
		{
			Mode->RemoveTabFactory(Itr->TabID);
		}
		Itr.RemoveCurrent();
	}
}
