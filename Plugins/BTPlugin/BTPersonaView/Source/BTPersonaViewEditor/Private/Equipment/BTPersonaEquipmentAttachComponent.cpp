//--------------------------------------------------
//! @file   BTPersonaEquipmentAttachComponent.cpp
//! @brief  ペルソナプレビューアクター用機器アタッチコンポーネント
//! @author BT.Masaki Okuda
//--------------------------------------------------
#include "BTPersonaEquipmentAttachComponent.h"

#include "BTPersonaViewEditorPreferenceSettings.h"
#include "BTPersonaViewEditorPrivate.h"
#include "IPersonaPreviewScene.h"
#include "IPersonaToolkit.h"

#include "Animation/DebugSkelMeshComponent.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

#include "Equipment/BTPersonaEquipmentProviderAssetInterface.h"


namespace BTPersonaEquipmentAttachComponentPrivate
	{
		/** アセットロードヘルパ */
		TSharedPtr<FStreamableHandle> LoadAssets(TConstArrayView<FSoftObjectPath> InLoadPaths, bool bAsync = true)
		{
			static const FString DebugName = TEXT("BTPersonaEquipmentAttachComponentPrivateLoad");
			FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
			TSharedPtr<FStreamableHandle> Handle;
			if( bAsync )
			{
				Handle = Streamable.RequestAsyncLoad(
					TArray<FSoftObjectPath>(InLoadPaths), FStreamableDelegate(), FStreamableManager::DefaultAsyncLoadPriority, false, false, DebugName);
			}
			else
			{
				Handle = Streamable.RequestSyncLoad(TArray<FSoftObjectPath>(InLoadPaths), false, DebugName);
			}
			return Handle;
		}
	}// namespace BTPersonaEquipmentAttachComponentPrivate


//////////////////////////////////////////////////////////////////////////
//  FBTPersonaEquipmentAttachComponentSpawnInfo
//////////////////////////////////////////////////////////////////////////

/**
 * @brief 表示用コンポーネントを再生成する
 */
bool FBTPersonaEquipmentAttachComponentSpawnInfo::RecreatePreviewComponent(const TSharedRef<IPersonaPreviewScene>& InPreviewScene)
{
	DestroyPreviewComponent();
	return CreatePreviewComponentIfNotExists(InPreviewScene);
}

/**
 * @brief 表示用コンポーネントがなければ生成を行う
 */
bool FBTPersonaEquipmentAttachComponentSpawnInfo::CreatePreviewComponentIfNotExists(const TSharedRef<IPersonaPreviewScene>& InPreviewScene)
{
	if( IsValidSpawnedComponent(InPreviewScene) ) { return false; }
	if( IsValid(ComponentTemplateInstance) == false ) { return false; }

	AActor* PreviewActor = InPreviewScene->GetActor();
	if( PreviewActor == nullptr ) { return false; }

	USkeletalMeshComponent* SkeletalMeshComponent = InPreviewScene->GetPreviewMeshComponent();
	if( SkeletalMeshComponent == nullptr ) { return false; }

	FixComponentTemplateInstance(); // テンプレートコンポ―ネントの調整
	FixEquipmentComponentData(); // コンポ―ネント生成データの修正

	SpawnedPreviewComponent = DuplicateObject<UActorComponent>(ComponentTemplateInstance, PreviewActor);
	if( SpawnedPreviewComponent == nullptr ) { return false; }

	// 生成コンポーネントを生成する
	InPreviewScene->AddComponent(SpawnedPreviewComponent, EquipmentComponentData.AttachRelativeTransform);

	// アタッチを行う
	if( USceneComponent* SpawnedSceneComp = Cast<USceneComponent>(SpawnedPreviewComponent) )
	{
		const FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
		SpawnedSceneComp->AttachToComponent(SkeletalMeshComponent, AttachRules, EquipmentComponentData.AttachSocketName);
		SpawnedSceneComp->SetRelativeTransform(EquipmentComponentData.AttachRelativeTransform);
	}
	return true;
}

/**
 * @brief 表示用コンポーネントを削除する
 */
void FBTPersonaEquipmentAttachComponentSpawnInfo::DestroyPreviewComponent()
{
	if( IsValid(SpawnedPreviewComponent) )
	{
		SpawnedPreviewComponent->DestroyComponent();
	}
	SpawnedPreviewComponent = nullptr;
}

/**
 * @brief 生成コンポーネントは正常に生成されているか
 */
bool FBTPersonaEquipmentAttachComponentSpawnInfo::IsValidSpawnedComponent(const TSharedRef<IPersonaPreviewScene>& InPreviewScene) const
{
	const AActor* PreviewActor = InPreviewScene->GetActor();
	if( PreviewActor == nullptr ) { return false; }
	if( IsValid(SpawnedPreviewComponent) == false ) { return false; }
	if( PreviewActor->OwnsComponent(SpawnedPreviewComponent) == false ) { return false; }
	return true;
}

/**
 * @brief 保持しているオブジェクトを削除
 */
void FBTPersonaEquipmentAttachComponentSpawnInfo::DestroyObjects(const TSharedPtr<IPersonaPreviewScene>& InPreviewScene)
{
	if( GExitPurge ) { return; }

	if( IsValid(ComponentTemplateInstance) )
	{
		ComponentTemplateInstance->DestroyComponent();
		ComponentTemplateInstance = nullptr;
	}

	if( IsValid(SpawnedPreviewComponent) )
	{
		if( InPreviewScene.IsValid() )
		{
			InPreviewScene->RemoveComponent(SpawnedPreviewComponent);
		}
		SpawnedPreviewComponent->DestroyComponent();
		SpawnedPreviewComponent = nullptr;
	}
}

/**
 * @brief テンプレートコンポ―ネントインスタンスの修正
 */
void FBTPersonaEquipmentAttachComponentSpawnInfo::FixComponentTemplateInstance() const
{
	if( IsValid(ComponentTemplateInstance) == false ) { return; }

	// ユーザーが意図せずコンポ―ネントタグを削除してしまうケースを考慮して、テンプレートコンポ―ネントのタグを再設定する
	if( ComponentTemplateInstance->ComponentHasTag(UBTPersonaEquipmentAttachComponent::CreatedComponentTagName) == false )
	{
		ComponentTemplateInstance->ComponentTags.Add(UBTPersonaEquipmentAttachComponent::CreatedComponentTagName);
	}
}

/**
 * @brief コンポネント生成データの修正
 */
void FBTPersonaEquipmentAttachComponentSpawnInfo::FixEquipmentComponentData()
{
	if( IsValid(ComponentTemplateInstance) == false ) { return; }

	// アタッチパラメータを変更する場合があるのであらかじめ入っているパラメータを入れて置く
	if( const USceneComponent* TemplateSceneComponent = Cast<USceneComponent>(ComponentTemplateInstance) )
	{
		EquipmentComponentData.AttachRelativeTransform = TemplateSceneComponent->GetRelativeTransform();
	}
}

//////////////////////////////////////////////////////////////////////////
//  UBTPersonaEquipmentAttachComponent
//////////////////////////////////////////////////////////////////////////
const FName UBTPersonaEquipmentAttachComponent::CreatedComponentTagName(TEXT("BTPersonaEquipment"));

// Sets default values for this component's properties
UBTPersonaEquipmentAttachComponent::UBTPersonaEquipmentAttachComponent()
	: bLoadedEquipmentProviderAssets(false), bDisableAutoAttachTemporary(false), EquipmentChangeStateMachine(this)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bTickInEditor = true;
	bIsEditorOnly = true;
	// ...
}

/**
 * @brief ペルソナプレビューシーンを設定
 */
void UBTPersonaEquipmentAttachComponent::SetPersonaPreviewScene(const TSharedRef<IPersonaPreviewScene>& InPreviewScene)
{
	checkf(IsRegistered() == false, TEXT("UBTPersonaEquipmentAttachComponent::SetPersonaPreviewScene() - Already registered."));
	WeakPreviewScene = InPreviewScene;
}

/**
 * @brief 何かしらのアセットをロード中か
 */
bool UBTPersonaEquipmentAttachComponent::IsLoadingAnyAssets() const
{
	const bool bIsLoadingProviderAsset = EquipmentProviderAssetLoadHandle.IsValid() && EquipmentProviderAssetLoadHandle->IsLoadingInProgress();
	const bool bIsLoadingComponentAsset = EquipmentComponentAssetLoadHandle.IsValid() && EquipmentComponentAssetLoadHandle->IsLoadingInProgress();
	return bIsLoadingProviderAsset || bIsLoadingComponentAsset;
}

/**
 * @brief 機器情報提供アセットを非同期ロードするよう要求する
 */
void UBTPersonaEquipmentAttachComponent::RequestLoadEquipmentProviderAssets()
{
	if( bLoadedEquipmentProviderAssets == false )
	{
		// 機器情報提供アセットがロードされていないのでロードをリクエストする
		EquipmentChangeStateMachine.ChangeStateRequest(EChangeState::EquipmentProviderAssetLoading);
	}
}

/**
 * @brief 外部から機器コンポ―ネントを設定する
 */
void UBTPersonaEquipmentAttachComponent::RequestEquipmentComponentExternal(TArrayView<FBTPersonaEquipmentComponentData> InEquipmentComponentDataList, TArrayView<UObject*> InProviderAssets)
{
	if( CanSetEquipmentComponentExternal() == false || InEquipmentComponentDataList.Num() != InProviderAssets.Num() ) { return; }

	// コンポーネント生成データに格納
	int32 Index = 0;
	for( FBTPersonaEquipmentComponentData& CompDataElm : InEquipmentComponentDataList )
	{
		FBTPersonaEquipmentAttachComponentSpawnInfo& SpawnedCompInfoRef = SpawnedComponentInfos.AddDefaulted_GetRef();
		SpawnedCompInfoRef.EquipmentComponentData = MoveTemp(CompDataElm);
		SpawnedCompInfoRef.SpawnedPreviewComponent = nullptr;
		SpawnedCompInfoRef.EquipmentProviderAsset = InProviderAssets[Index];
		Index++;
	}

	MarkComponentChangedExternal();
	
	EquipmentChangeStateMachine.ChangeState(EChangeState::EquipmentComponentAssetLoading);
}

/**
 * @brief  外部から生成コンポーネントをすべて削除する
 */
void UBTPersonaEquipmentAttachComponent::RequestRemoveAllSpawnedComponentsExternal()
{
	RemoveAllSpawnedComponents();
	MarkComponentChangedExternal();
}

/**
 * @brief 外部からコンポ―ネント変更をマークする(自動アタッチさせない)
 */
void UBTPersonaEquipmentAttachComponent::MarkComponentChangedExternal()
{
	// 外から変更された場合、自動アタッチ機能は無効化する
	bDisableAutoAttachTemporary = true;
}

/**
 * @brief 機器提供アセットを取得
 */
TConstArrayView<TObjectPtr<UObject>> UBTPersonaEquipmentAttachComponent::GetLoadedEquipmentProviderAssets() const
{
	return TConstArrayView<TObjectPtr<UObject>>(LoadedEquipmentProviderAssets);
}

/**
 * @brief テンプレートインスタンスユニークIDから生成機器コンポ―ネント情報を取得
 */
const FBTPersonaEquipmentAttachComponentSpawnInfo* UBTPersonaEquipmentAttachComponent::FindSpawnedComponentInfoFromTemplateInstanceUniqueID(int32 InTemplateInstanceUniqueID) const
{
	return GetSpawnedComponentInfos().FindByPredicate(
		[InTemplateInstanceUniqueID](const FBTPersonaEquipmentAttachComponentSpawnInfo& SpawnedComponentInfoElm)
			{
				if( SpawnedComponentInfoElm.ComponentTemplateInstance != nullptr &&
					SpawnedComponentInfoElm.ComponentTemplateInstance->GetUniqueID() == InTemplateInstanceUniqueID )
				{
					return true;
				}
				return false;
			});
}

// ReSharper disable once CppMemberFunctionMayBeConst
FBTPersonaEquipmentAttachComponentSpawnInfo* UBTPersonaEquipmentAttachComponent::FindSpawnedMutableComponentInfoFromTemplateInstanceUniqueID(int32 InTemplateInstanceUniqueID)
{
	return const_cast<FBTPersonaEquipmentAttachComponentSpawnInfo*>(static_cast<const ThisClass*>(this)->FindSpawnedComponentInfoFromTemplateInstanceUniqueID(InTemplateInstanceUniqueID));
}

/**
 * @brief テンプレートインスタンスユニークIDから生成機器コンポ―ネントを削除
 */
void UBTPersonaEquipmentAttachComponent::RemoveSpawnedComponentFromTemplateInstanceUniqueID(int32 InTemplateInstanceUniqueID)
{
	// IDが無効です
	if( InTemplateInstanceUniqueID == INDEX_NONE ) { return; }

	const int32 HitIndex = SpawnedComponentInfos.FindLastByPredicate([InTemplateInstanceUniqueID](const FBTPersonaEquipmentAttachComponentSpawnInfo& SpawnedComponentInfoElm)
		{
			return SpawnedComponentInfoElm.GetTemplateInstanceUniqueID() == InTemplateInstanceUniqueID;
		});

	// 存在市内
	if( HitIndex == INDEX_NONE ) { return; }

	// 生成情報配列を変更した
	SpawnedComponentInfos[HitIndex].DestroyObjects(WeakPreviewScene.Pin());
	SpawnedComponentInfos.RemoveAt(HitIndex);
	OnEquipmentAttachComponentChangedEvent.Broadcast();
}

/**
 * @brief テンプレートインスタンスユニークIDから生成機器コンポ―ネント再生成
 */
void UBTPersonaEquipmentAttachComponent::RecreateSpawnedComponentFromTemplateInstanceUniqueID(int32 InTemplateInstanceUniqueID)
{
	FBTPersonaEquipmentAttachComponentSpawnInfo* HitInfo = FindSpawnedMutableComponentInfoFromTemplateInstanceUniqueID(InTemplateInstanceUniqueID);
	if( HitInfo == nullptr ) { return; }
	const TSharedPtr<IPersonaPreviewScene> PreviewScene = WeakPreviewScene.Pin();
	if( PreviewScene.IsValid() == false ) { return; }
	HitInfo->RecreatePreviewComponent(PreviewScene.ToSharedRef());
}

/**
 * @brief テンプレートインスタンスユニークIDからソケット変更を要求する
 */
void UBTPersonaEquipmentAttachComponent::RequestChangeSocketFromTemplateInstanceUniqueID(int32 InTemplateInstanceUniqueID, const FName& InNewSocketName)
{
	FBTPersonaEquipmentAttachComponentSpawnInfo* HitInfo = FindSpawnedMutableComponentInfoFromTemplateInstanceUniqueID(InTemplateInstanceUniqueID);
	if( HitInfo == nullptr ) { return; }
	const TSharedPtr<IPersonaPreviewScene> PreviewScene = WeakPreviewScene.Pin();
	if( PreviewScene.IsValid() == false ) { return; }

	if( HitInfo->EquipmentComponentData.AttachSocketName != InNewSocketName )
	{// ソケット名を変更した場合はコンポーネント再生成
		HitInfo->EquipmentComponentData.AttachSocketName = InNewSocketName;
		HitInfo->RecreatePreviewComponent(PreviewScene.ToSharedRef());
	}
}

/**
 * @brief 生成コンポーネントをすべて削除する
 */
void UBTPersonaEquipmentAttachComponent::RemoveAllSpawnedComponents()
{
	for( FBTPersonaEquipmentAttachComponentSpawnInfo& SpawnedCompElm : SpawnedComponentInfos )
	{
		SpawnedCompElm.DestroyObjects(WeakPreviewScene.Pin());
	}
	ClearSpawnedComponentInfos();
}

void UBTPersonaEquipmentAttachComponent::OnRegister()
{
	Super::OnRegister();
	const UBTPersonaViewEditorPreferenceSettings* Setting = GetDefault<UBTPersonaViewEditorPreferenceSettings>();

	if( Setting == nullptr ) { return; }
	if( Setting->bEnableAttachEquipment == false ) { return; }

	const TSharedPtr<IPersonaPreviewScene> PreviewScene = WeakPreviewScene.Pin();

	// ソースコントロールで変更を戻すなどでリバートされる場合にPreviewSceneが無効なのにOnRegisterが呼ばれる可能性がある。
	if( PreviewScene.IsValid() == false ) { return; }

	{
		// 自動アタッチを無効化する
		TGuardValue<bool> GuardSetting(bDisableAutoAttachTemporary, true);

		InitEquipmentChangeStateMachine();

		PreviewScene->RegisterOnAnimChanged(FOnAnimChanged::CreateUObject(this, &ThisClass::OnPreviewAnimChanged));
		OnPreviewAnimChanged(PreviewScene->GetPersonaToolkit()->GetAnimationAsset());

		PreviewScene->RegisterOnPreviewMeshChanged(FOnPreviewMeshChanged::CreateUObject(this, &ThisClass::OnPreviewMeshChanged));
		OnPreviewMeshChanged(nullptr, PreviewScene->GetPersonaToolkit()->GetPreviewMesh());
	}

	StartAutoAttachComponentProcess(); // 初回更新
}

void UBTPersonaEquipmentAttachComponent::OnUnregister()
{
	if( const TSharedPtr<IPersonaPreviewScene> PreviewScene = WeakPreviewScene.Pin() )
	{
		PreviewScene->UnregisterOnAnimChanged(this);
		PreviewScene->UnregisterOnPreviewMeshChanged(this);
	}

	// 生成コンポーネント削除
	RemoveAllSpawnedComponents();

	EquipmentChangeStateMachine.Deactivate();
	LatestPreviewAsset = nullptr;
	LatestPreviewMesh = nullptr;
	LoadedEquipmentProviderAssets.Empty();
	bLoadedEquipmentProviderAssets = false;
	bDisableAutoAttachTemporary = false;

	Super::OnUnregister();
}


// Called every frame
void UBTPersonaEquipmentAttachComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	EquipmentChangeStateMachine.Proc(DeltaTime);
}

/**
 * @brief 自動アタッチが有効か
 */
bool UBTPersonaEquipmentAttachComponent::IsEnableAutoAttachEquipment() const
{
	// 自動アタッチ機能が無効なら何もしない
	if( bDisableAutoAttachTemporary ) { return false; }
	const UBTPersonaViewEditorPreferenceSettings* Setting = GetDefault<UBTPersonaViewEditorPreferenceSettings>();
	if( Setting == nullptr || Setting->bEnableAttachEquipment == false || Setting->bEnableAutoAttachEquipment == false ) { return false; }
	return true;
}

/**
 * @brief 機器変更ステートマシン初期化
 */
void UBTPersonaEquipmentAttachComponent::InitEquipmentChangeStateMachine()
{
#ifdef BT_PERSONA_SET_STATE
	static_assert(false, "BT_PERSONA_SET_STATE is already defined.");
#else

	// 全ての状態で関数を設定する
#define BT_PERSONA_SET_STATE(InState)\
EquipmentChangeStateMachine.SetState(EChangeState::InState, &ThisClass::OnEnter##InState, &ThisClass::OnUpdate##InState, &ThisClass::OnExit##InState);

	EquipmentChangeStateMachine.SetState(EChangeState::Wait, nullptr, &ThisClass::OnUpdateWait, nullptr);
	BT_PERSONA_SET_STATE(EquipmentProviderAssetLoading);
	BT_PERSONA_SET_STATE(EquipmentProviderAssetLoadCompleted);
	BT_PERSONA_SET_STATE(EquipmentComponentAssetLoading);

#undef BT_PERSONA_SET_STATE

#endif // BT_PERSONA_SET_STATE

	// デフォルトは待機
	EquipmentChangeStateMachine.Activate(EChangeState::Wait);
}

/**
 * @brief 自動アタッチコンポーネントのアタッチを開始する
 */
void UBTPersonaEquipmentAttachComponent::StartAutoAttachComponentProcess()
{
	// 自動アタッチ機能が無効なら何もしない
	if( IsEnableAutoAttachEquipment() == false ) { return; }
	// プレビューアニメーションが無効なら何もしない
	if( LatestPreviewAsset == nullptr || LatestPreviewMesh == nullptr ) { return; }

	if( bLoadedEquipmentProviderAssets == false )
	{
		// 機器情報提供アセットがロードされていないのでロードをリクエストする
		EquipmentChangeStateMachine.ChangeStateRequest(EChangeState::EquipmentProviderAssetLoading);
	}
	else
	{
		// ロード済みならばコンポ―ネントをつけるステートに移動する
		EquipmentChangeStateMachine.ChangeStateRequest(EChangeState::EquipmentProviderAssetLoadCompleted);
	}
}

/**
 * @brief 生成コンポーネントを自動アタッチコンポーネントデータに変更する
 */
void UBTPersonaEquipmentAttachComponent::UpdateSpawnedComponentInfosFromAutoAttachData()
{
	if( IsEnableAutoAttachEquipment() == false ) { return; }
	if( bLoadedEquipmentProviderAssets == false ) { return; }
	if( EquipmentComponentAssetLoadHandle.IsValid() ) { return; }

	ClearSpawnedComponentInfos();

	TArray<FBTPersonaEquipmentComponentData> AutoAttachComponentsData;
	for( UObject* LoadedProvider : LoadedEquipmentProviderAssets )
	{
		const IBTPersonaEquipmentProviderAssetInterface* ProviderInterface = Cast<IBTPersonaEquipmentProviderAssetInterface>(LoadedProvider);
		if( ProviderInterface == nullptr ) { continue; }

		const int32 PrevCompDataNum = AutoAttachComponentsData.Num();
		ProviderInterface->FindAutoCreatablePersonaEquipmentData(AutoAttachComponentsData, LatestPreviewAsset, LatestPreviewMesh);
		const int32 AfterCompDataNum = AutoAttachComponentsData.Num();

		UE_CLOG(PrevCompDataNum > AfterCompDataNum, LogBTPersonaViewEditor, Fatal,
			TEXT("UBTPersonaEquipmentAttachComponent::UpdateSpawnedComponentInfosFromAutoAttachData; "
				"FindAutoCreatablePersonaEquipmentData()でOutComponentsDataの要素をクリアしてはいけません。"
				" AssetPath %s"), *LoadedProvider->GetPathName());

		// コンポーネント生成データに格納
		for( FBTPersonaEquipmentComponentData& CompDataElm : AutoAttachComponentsData )
		{
			FBTPersonaEquipmentAttachComponentSpawnInfo& SpawnedCompInfoRef = SpawnedComponentInfos.AddDefaulted_GetRef();
			SpawnedCompInfoRef.EquipmentComponentData = MoveTemp(CompDataElm);
			SpawnedCompInfoRef.SpawnedPreviewComponent = nullptr;
			SpawnedCompInfoRef.EquipmentProviderAsset = LoadedProvider;
		}

		AutoAttachComponentsData.Empty(AutoAttachComponentsData.Num());
	}
}

/**
 * @brief プレビューアニメーション変更通知デリゲート
 */
void UBTPersonaEquipmentAttachComponent::OnPreviewAnimChanged(UAnimationAsset* InAnimAsset)
{
	if( IsRegistered() == false ) return;
	if( InAnimAsset == nullptr ) return;
	// 前回と同じアニメーションアセットで呼ばれている
	if( LatestPreviewAsset == InAnimAsset ) { return; }

	// アセット更新
	LatestPreviewAsset = InAnimAsset;

	StartAutoAttachComponentProcess();
}

/**
 * @brief プレビューメッシュ変更通知デリゲート
 */
void UBTPersonaEquipmentAttachComponent::OnPreviewMeshChanged(USkeletalMesh* InOldPreviewMesh, USkeletalMesh* InNewPreviewMesh)
{
	if( IsRegistered() == false ) return;
	if( InNewPreviewMesh == nullptr ) return;
	// 前回と同じメッシュアセットで呼ばれている
	if( LatestPreviewMesh == InNewPreviewMesh ) { return; }

	// アセット更新
	LatestPreviewMesh = InNewPreviewMesh;

	StartAutoAttachComponentProcess();
}

/**
 * @brief 生成コンポーネント情報をクリアする
 */
void UBTPersonaEquipmentAttachComponent::ClearSpawnedComponentInfos()
{
	SpawnedComponentInfos.Empty();
	OnEquipmentAttachComponentChangedEvent.Broadcast();
}

//////////////////////////////////////////////////////////////////////////
//  機器変更ステート：Wait
//////////////////////////////////////////////////////////////////////////
bool UBTPersonaEquipmentAttachComponent::OnUpdateWait(float DeltaTime)
{
	const TSharedPtr<IPersonaPreviewScene> PersonaScene = WeakPreviewScene.Pin();
	if( PersonaScene.IsValid() == false ) { return false; }

	// 生成コンポーネント配列で実際に生成されていないコンポーネントが１つでもあれば生成しに行く
	bool bAnyInvalidComponentData = false;
	for( FBTPersonaEquipmentAttachComponentSpawnInfo& SpawnedCompInfoElm : SpawnedComponentInfos )
	{
		const bool bIsNewSpawnedComponent = SpawnedCompInfoElm.CreatePreviewComponentIfNotExists(PersonaScene.ToSharedRef());
		if( bIsNewSpawnedComponent == false && SpawnedCompInfoElm.IsValidSpawnedComponent(PersonaScene.ToSharedRef()) == false )
		{// 生成できない = 無効データ
			bAnyInvalidComponentData = false;
		}
	}

	if( bAnyInvalidComponentData )
	{
		EquipmentChangeStateMachine.ChangeState(EChangeState::EquipmentComponentAssetLoading);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//  機器変更ステート：EquipmentProviderAssetLoading
//////////////////////////////////////////////////////////////////////////
void UBTPersonaEquipmentAttachComponent::OnEnterEquipmentProviderAssetLoading(EChangeState InPrevState)
{
	if( bLoadedEquipmentProviderAssets )
	{// ロード完了している
		EquipmentChangeStateMachine.ChangeStateRequest(EChangeState::EquipmentProviderAssetLoadCompleted);
		return;
	}

	const UBTPersonaViewEditorPreferenceSettings* Setting = GetDefault<UBTPersonaViewEditorPreferenceSettings>();
	if( Setting == nullptr ) { return; }

	// 非同期ロード
	EquipmentProviderAssetLoadHandle = BTPersonaEquipmentAttachComponentPrivate::LoadAssets(Setting->EquipmentProviderAssetPaths, true);
}

bool UBTPersonaEquipmentAttachComponent::OnUpdateEquipmentProviderAssetLoading(float DeltaTime)
{
	if( EquipmentProviderAssetLoadHandle.IsValid() == false )
	{// 失敗しているので待機に戻す
		UE_LOG(LogBTPersonaViewEditor, Warning, TEXT("UBTPersonaEquipmentAttachComponent::OnUpdateEquipmentProviderAssetLoading; 機器プロバイダーのアセットをロードできませんでした。"));
		EquipmentChangeStateMachine.ChangeStateRequest(EChangeState::Wait);
		return false;
	}

	if( EquipmentProviderAssetLoadHandle->IsLoadingInProgress() == false )
	{// ロード完了した
		int32 LoadedCount = 0, RequestCount = 0;
		EquipmentProviderAssetLoadHandle->GetLoadedCount(LoadedCount, RequestCount);

		LoadedEquipmentProviderAssets.Empty(LoadedCount);

		bLoadedEquipmentProviderAssets = true;
		if( LoadedCount == 0 )
		{// ロードできていないので待機に戻す
			UE_LOG(LogBTPersonaViewEditor, Log, TEXT("UBTPersonaEquipmentAttachComponent::OnUpdateEquipmentProviderAssetLoading; 機器プロバイダーのアセットをロードが0です。待機状態に移行します。"));
			EquipmentChangeStateMachine.ChangeStateRequest(EChangeState::Wait);
			return false;
		}

		TArray<UObject*> LoadAssetsTmp;
		// アセットを入れて置く
		EquipmentProviderAssetLoadHandle->GetLoadedAssets(LoadAssetsTmp);
		LoadedEquipmentProviderAssets = LoadAssetsTmp;
		EquipmentChangeStateMachine.ChangeStateRequest(EChangeState::EquipmentProviderAssetLoadCompleted);
	}

	return false;
}

void UBTPersonaEquipmentAttachComponent::OnExitEquipmentProviderAssetLoading(EChangeState InNextState)
{
	if( EquipmentProviderAssetLoadHandle.IsValid() && EquipmentProviderAssetLoadHandle->IsLoadingInProgress() )
	{// ロード中にExitされた場合はロードハンドルをキャンセルする
		EquipmentProviderAssetLoadHandle->CancelHandle();
	}
	EquipmentProviderAssetLoadHandle.Reset();
}

//////////////////////////////////////////////////////////////////////////
//  機器変更ステート：EquipmentProviderAssetLoadCompleted
//////////////////////////////////////////////////////////////////////////
void UBTPersonaEquipmentAttachComponent::OnEnterEquipmentProviderAssetLoadCompleted(EChangeState InPrevState)
{
	OnAssetProviderLoadCompletedEvent.Broadcast();

	if( IsEnableAutoAttachEquipment() == false )
	{// 自動アタッチ無効なので待機に戻す
		EquipmentChangeStateMachine.ChangeState(EChangeState::Wait);
		return;
	}

	// ロードアセットから自動アタッチできるコンポ―ネントデータを取得する
	UpdateSpawnedComponentInfosFromAutoAttachData();
	EquipmentChangeStateMachine.ChangeState(EChangeState::EquipmentComponentAssetLoading);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool UBTPersonaEquipmentAttachComponent::OnUpdateEquipmentProviderAssetLoadCompleted(float DeltaTime)
{
	return false;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UBTPersonaEquipmentAttachComponent::OnExitEquipmentProviderAssetLoadCompleted(EChangeState InNextState)
{
}

//////////////////////////////////////////////////////////////////////////
//  機器変更ステート：EquipmentComponentAssetLoading
//////////////////////////////////////////////////////////////////////////
void UBTPersonaEquipmentAttachComponent::OnEnterEquipmentComponentAssetLoading(EChangeState InPrevState)
{
	// 生成コンポ―ネント情報から必要なアセットをロードする
	TArray<FSoftObjectPath, TInlineAllocator<64>> LoadPaths;
	for( const FBTPersonaEquipmentAttachComponentSpawnInfo& SpawnedCompInfoElm : SpawnedComponentInfos )
	{
		LoadPaths.Append(SpawnedCompInfoElm.EquipmentComponentData.AssetPathList);
	}

	// 非同期ロード
	EquipmentComponentAssetLoadHandle = BTPersonaEquipmentAttachComponentPrivate::LoadAssets(LoadPaths, true);
}

bool UBTPersonaEquipmentAttachComponent::OnUpdateEquipmentComponentAssetLoading(float DeltaTime)
{
	if( EquipmentComponentAssetLoadHandle.IsValid() == false )
	{// 失敗しているので待機に戻す
		UE_LOG(LogBTPersonaViewEditor, Warning, TEXT("UBTPersonaEquipmentAttachComponent::OnUpdateEquipmentComponentAssetLoading; コンポ―ネント関連アセットをロードできませんでした。"));
		ClearSpawnedComponentInfos();
		EquipmentChangeStateMachine.ChangeStateRequest(EChangeState::Wait);
		return false;
	}

	if( EquipmentComponentAssetLoadHandle->IsLoadingInProgress() == false )
	{// ロード完了した
		int32 LoadedCount = 0, RequestCount = 0;
		EquipmentComponentAssetLoadHandle->GetLoadedCount(LoadedCount, RequestCount);

		const TSharedPtr<IPersonaPreviewScene> PersonaScene = WeakPreviewScene.Pin();
		if( PersonaScene.IsValid() )
		{
			//////////////////////////////////////////////////////////////////////////
			//  コンポーネントを生成する
			// テンプレートコンポ―ネントを作成 + セットアップして、生成コンポーネントをコピーとして作成する
			//////////////////////////////////////////////////////////////////////////
			for( auto Itr = SpawnedComponentInfos.CreateIterator(); Itr; ++Itr )
			{
				if( Itr->IsValidSpawnedComponent(PersonaScene.ToSharedRef()) ) { continue; }// 生成されていないコンポ―ネントのみ処理を通す

				FBTPersonaEquipmentComponentData& ComponentDataRef = Itr->EquipmentComponentData;
				bool bComponentCreateSuccess = false;
				UActorComponent* TemplateComponentInstance = NewObject<UActorComponent>(GetTransientPackage(), ComponentDataRef.SpawnedComponentClass.Get());

				// コンポ―ネントセットアップ(失敗する可能性あり)
				if( TemplateComponentInstance != nullptr )
				{
					// 必須タグ設定(この機能で生成したコンポ―ネントか識別できるようにタグをつける)
					TemplateComponentInstance->ComponentTags.AddUnique(CreatedComponentTagName);

					if( ComponentDataRef.ComponentTag.IsNone() == false )
					{// 任意タグ設定
						TemplateComponentInstance->ComponentTags.AddUnique(ComponentDataRef.ComponentTag);
					}

					// (Sceneコンポーネントのみ）初期相対姿勢を格納する
					if( USceneComponent* TemplateSceneComponentInstance = Cast<USceneComponent>(TemplateComponentInstance) )
					{
						// コンポーネントとしての機能を有効にしたくないので、直にデータを格納
						TemplateSceneComponentInstance->SetRelativeTransform_Direct(ComponentDataRef.AttachRelativeTransform);
					}

					if( ComponentDataRef.ComponentSetUpDelegate.IsBound() &&
						ComponentDataRef.ComponentSetUpDelegate.Execute(TemplateComponentInstance, &ComponentDataRef) )
					{// セットアップデリゲートがある場合は実行
						bComponentCreateSuccess = true;
					}
					else if( ComponentDataRef.ComponentSetUpDelegate.IsBound() == false )
					{
						bComponentCreateSuccess = true;
					}
				}
				// セットアップ完了


				if( bComponentCreateSuccess )
				{// 作成成功
					Itr->ComponentTemplateInstance = TemplateComponentInstance;

					if( Itr->CreatePreviewComponentIfNotExists(PersonaScene.ToSharedRef()) == false )
					{// 表示コンポ―ネントの生成に失敗
						bComponentCreateSuccess = false;
					}
				}

				if( bComponentCreateSuccess == false )
				{// 作成失敗(このテンプレートコンポ―ネントは削除する)
					if( TemplateComponentInstance != nullptr )
					{// 生成したコンポ―ネントを破棄
						TemplateComponentInstance->DestroyComponent();
					}

					Itr.RemoveCurrent();
				}
			}

			//////////////////////////////////////////////////////////////////////////
			//  生成完了
			//////////////////////////////////////////////////////////////////////////

			// 見やすさ重視でコンポ―ネントクラスごとにソートしておく
			SpawnedComponentInfos.Sort([](const FBTPersonaEquipmentAttachComponentSpawnInfo& A, const FBTPersonaEquipmentAttachComponentSpawnInfo& B)
				{
					return A.EquipmentComponentData.SpawnedComponentClass.Get() < B.EquipmentComponentData.SpawnedComponentClass.Get();
				});

			// 生成情報配列を変更した
			OnEquipmentAttachComponentChangedEvent.Broadcast();
		}
		else
		{// コンポ―ネント作成に必要なシーン情報が取得できなかった
			UE_LOG(LogBTPersonaViewEditor, Warning, TEXT("UBTPersonaEquipmentAttachComponent::OnUpdateEquipmentComponentAssetLoading; コンポ―ネント作成に必要なシーン情報が取得できませんでした。"));
			ClearSpawnedComponentInfos();
		}

		EquipmentChangeStateMachine.ChangeStateRequest(EChangeState::Wait);
	}

	return false;
}

void UBTPersonaEquipmentAttachComponent::OnExitEquipmentComponentAssetLoading(EChangeState InNextState)
{
	if( EquipmentComponentAssetLoadHandle.IsValid() && EquipmentComponentAssetLoadHandle->IsLoadingInProgress() )
	{// ロード中にExitされた場合はロードハンドルをキャンセルする
		EquipmentComponentAssetLoadHandle->CancelHandle();
	}
	EquipmentComponentAssetLoadHandle.Reset();
}
