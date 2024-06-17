//--------------------------------------------------
//! @file   BTPersonaEquipmentAttachComponent.h
//! @brief  ペルソナプレビューアクター用機器アタッチコンポーネント
//! @author BT.Masaki Okuda
//--------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Equipment/BTPersonaEquipmentData.h"

#include "StateMachine/BTSimpleClassStateMachine.h"

#include "BTPersonaEquipmentAttachComponent.generated.h"

struct FStreamableHandle;
class IPersonaPreviewScene;

/** 機器変更通知デリゲート */
DECLARE_EVENT(UBTPersonaEquipmentAttachComponent, FOnEquipmentAttachComponentChanged);

/** プロバイダーアセットロード完了デリゲート */
DECLARE_EVENT(UBTPersonaEquipmentAttachComponent, FOnAssetProviderLoadCompleted);

/**
 * @brief アタッチコンポ―ネント生成情報
 */
USTRUCT()
struct FBTPersonaEquipmentAttachComponentSpawnInfo {
	GENERATED_BODY()

	/** コンポ―ネントの生成情報を提供したアセット */
	UPROPERTY(Transient)
	TObjectPtr<UObject> EquipmentProviderAsset;
	/** 生成した表示用コンポ―ネント */
	UPROPERTY(Transient)
	TObjectPtr<UActorComponent> SpawnedPreviewComponent;
	/** コンポーネントテンプレートインスタンス(このコンポ―ネントをコピーして表示コンポーネントを作成する) */
	UPROPERTY(Transient)
	TObjectPtr<UActorComponent> ComponentTemplateInstance;
	/** コンポーネント生成データ */
	FBTPersonaEquipmentComponentData EquipmentComponentData;

public:
	/** 表示用コンポーネントを再生成する */
	bool RecreatePreviewComponent(const TSharedRef<IPersonaPreviewScene>& InPreviewScene);
	/** 表示用コンポーネントがなければ生成を行う */
	bool CreatePreviewComponentIfNotExists(const TSharedRef<IPersonaPreviewScene>& InPreviewScene);
	/** 表示用コンポーネントを削除する */
	void DestroyPreviewComponent();
	/** 生成コンポーネントは正常に生成されているか */
	bool IsValidSpawnedComponent(const TSharedRef<IPersonaPreviewScene>& InPreviewScene) const;
	/** テンプレートインスタンスの一意IDを取得 */
	int32 GetTemplateInstanceUniqueID() const { return ComponentTemplateInstance ? ComponentTemplateInstance->GetUniqueID() : INDEX_NONE; }
	/** 保持しているオブジェクトを削除 */
	void DestroyObjects(const TSharedPtr<IPersonaPreviewScene>& InPreviewScene);

private:
	/** テンプレートコンポ―ネントインスタンスの修正 */
	void FixComponentTemplateInstance() const;
	/** コンポ―ネント生成データの修正 */
	void FixEquipmentComponentData();
};

/**
 * @brief ペルソナプレビューアクター用機器アタッチコンポ―ネント
 */
UCLASS(ClassGroup=(Custom), NotBlueprintType, NotBlueprintable)
class BTPERSONAVIEWEDITOR_API UBTPersonaEquipmentAttachComponent : public UActorComponent {
	GENERATED_BODY()

private:
	/** 機器変更状態 */
	enum class EChangeState : uint8 {
		EquipmentProviderAssetLoading, //!< 機器情報提供アセットロード中
		EquipmentProviderAssetLoadCompleted, //!< 機器情報提供アセットロード完了
		EquipmentComponentAssetLoading, //!< 機器コンポーネント生成で必要なアセットロード中
		Wait, //!< 待機
	};

public:
	// Sets default values for this component's properties
	UBTPersonaEquipmentAttachComponent();

	/** ペルソナプレビューシーンを設定 */
	void SetPersonaPreviewScene(const TSharedRef<IPersonaPreviewScene>& InPreviewScene);

	/** 何かしらのアセットをロード中か */
	bool IsLoadingAnyAssets() const;

	/** 機器情報提供アセットはロード済みか */
	bool IsLoadedEquipmentProviderAssets() const { return bLoadedEquipmentProviderAssets; }
	/** 機器情報提供アセットを非同期ロードするよう要求する */
	void RequestLoadEquipmentProviderAssets();
	/** 外部から機器コンポ―ネントを設定できる状態か */
	bool CanSetEquipmentComponentExternal() const { return IsLoadingAnyAssets() == false && bLoadedEquipmentProviderAssets == true && EquipmentChangeStateMachine.GetStateID() == EChangeState::Wait; }
	/** 外部から機器コンポ―ネントを設定する */
	void RequestEquipmentComponentExternal(TArrayView<FBTPersonaEquipmentComponentData> InEquipmentComponentDataList, TArrayView<UObject*> InProviderAssets);
	/** 外部から生成コンポーネントをすべて削除する */
	void RequestRemoveAllSpawnedComponentsExternal();
	/** 外部からコンポ―ネント変更をマークする(自動アタッチさせない) */
	void MarkComponentChangedExternal();
	
	/** 機器提供アセットを取得 */
	TConstArrayView<TObjectPtr<UObject>> GetLoadedEquipmentProviderAssets() const;


	/** 再生機器コンポ―ネント情報配列を取得 */
	TConstArrayView<FBTPersonaEquipmentAttachComponentSpawnInfo> GetSpawnedComponentInfos() const { return SpawnedComponentInfos; }
	/** テンプレートインスタンスユニークIDから生成機器コンポ―ネント情報を取得 */
	const FBTPersonaEquipmentAttachComponentSpawnInfo* FindSpawnedComponentInfoFromTemplateInstanceUniqueID(int32 InTemplateInstanceUniqueID) const;

private:
	/** 再生機器コンポ―ネント情報配列を取得 */
	TArrayView<FBTPersonaEquipmentAttachComponentSpawnInfo> GetSpawnedComponentInfosMutable() { return SpawnedComponentInfos; }
	/** テンプレートインスタンスユニークIDから生成機器コンポ―ネント情報を取得 */
	FBTPersonaEquipmentAttachComponentSpawnInfo* FindSpawnedMutableComponentInfoFromTemplateInstanceUniqueID(int32 InTemplateInstanceUniqueID);

public:
	/** テンプレートインスタンスユニークIDから生成機器コンポ―ネントを削除 */
	void RemoveSpawnedComponentFromTemplateInstanceUniqueID(int32 InTemplateInstanceUniqueID);
	/** テンプレートインスタンスユニークIDから生成機器コンポ―ネント再生成 */
	void RecreateSpawnedComponentFromTemplateInstanceUniqueID(int32 InTemplateInstanceUniqueID);
	/** テンプレートインスタンスユニークIDからソケット変更を要求する */
	void RequestChangeSocketFromTemplateInstanceUniqueID(int32 InTemplateInstanceUniqueID, const FName& InNewSocketName);
	
	/** 機器変更通知デリゲート取得 */
	FOnEquipmentAttachComponentChanged& OnEquipmentAttachComponentChanged() { return OnEquipmentAttachComponentChangedEvent; }
	/** プロバイダーアセットロード完了デリゲート取得 */
	FOnAssetProviderLoadCompleted& OnAssetProviderLoadCompleted() { return OnAssetProviderLoadCompletedEvent; }

	// Begin UActorComponent Interface
public:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// End UActorComponent Interface

private:
	/** 生成コンポーネントをすべて削除する */
	void RemoveAllSpawnedComponents();
	/** 自動アタッチが有効か */
	bool IsEnableAutoAttachEquipment() const;
	/** 機器変更ステートマシン初期化 */
	void InitEquipmentChangeStateMachine();
	/** 自動アタッチコンポ―ネントのアタッチを開始する */
	void StartAutoAttachComponentProcess();
	/** 生成コンポーネントを自動アタッチコンポーネントデータに変更する */
	void UpdateSpawnedComponentInfosFromAutoAttachData();
	/** プレビューアニメーション変更通知デリゲート */
	void OnPreviewAnimChanged(UAnimationAsset* InAnimAsset);
	/** プレビューメッシュ変更通知デリゲート */
	void OnPreviewMeshChanged(USkeletalMesh* InOldPreviewMesh, USkeletalMesh* InNewPreviewMesh);

	/** 生成コンポーネント情報をクリアする */
	void ClearSpawnedComponentInfos();

private:
	//////////////////////////////////////////////////////////////////////////
	//  機器変更ステートマシン
	//////////////////////////////////////////////////////////////////////////
	/** Wait */
	bool OnUpdateWait(float DeltaTime);
	/** EquipmentProviderAssetLoading*/
	void OnEnterEquipmentProviderAssetLoading(EChangeState InPrevState);
	bool OnUpdateEquipmentProviderAssetLoading(float DeltaTime);
	void OnExitEquipmentProviderAssetLoading(EChangeState InNextState);
	/** EquipmentProviderAssetLoadCompleted*/
	void OnEnterEquipmentProviderAssetLoadCompleted(EChangeState InPrevState);
	bool OnUpdateEquipmentProviderAssetLoadCompleted(float DeltaTime);
	void OnExitEquipmentProviderAssetLoadCompleted(EChangeState InNextState);
	/** EquipmentComponentAssetLoading*/
	void OnEnterEquipmentComponentAssetLoading(EChangeState InPrevState);
	bool OnUpdateEquipmentComponentAssetLoading(float DeltaTime);
	void OnExitEquipmentComponentAssetLoading(EChangeState InNextState);

public:
	static const FName CreatedComponentTagName; //!< 生成コンポーネントに必ず付与するタグ名(この機能で生成したコンポ―ネントか識別できるようにタグをつける)

private:
	/** ペルソナプレビューScene */
	TWeakPtr<IPersonaPreviewScene> WeakPreviewScene;
	/** 現在のブレビューアニメーションアセット */
	UPROPERTY(Transient)
	TObjectPtr<UAnimationAsset> LatestPreviewAsset;
	/** 現在のプレビューメッシュ */
	UPROPERTY(Transient)
	TObjectPtr<USkeletalMesh> LatestPreviewMesh;
	/** ロード済み機器情報提供アセット */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> LoadedEquipmentProviderAssets;
	/** 生成コンポ―ネント情報 */
	UPROPERTY(Transient)
	TArray<FBTPersonaEquipmentAttachComponentSpawnInfo> SpawnedComponentInfos;
	/**  機器情報提供アセットはロード済みか */
	bool bLoadedEquipmentProviderAssets;
	/** 機器情報提供アセットロードハンドル */
	TSharedPtr<FStreamableHandle> EquipmentProviderAssetLoadHandle;
	/** コンポ―ネントアセットロードハンドル */
	TSharedPtr<FStreamableHandle> EquipmentComponentAssetLoadHandle;
	/** 自動アタッチ更新を1時的に無効化するか */
	bool bDisableAutoAttachTemporary;
	/** 機器変更ステートマシン */
	TBTSimpleClassStateMachine<UBTPersonaEquipmentAttachComponent, EChangeState> EquipmentChangeStateMachine = { this };
	/** 機器変更通知デリゲート */
	FOnEquipmentAttachComponentChanged OnEquipmentAttachComponentChangedEvent;
	/** プロバイダーアセットロード完了デリゲート */
	FOnAssetProviderLoadCompleted OnAssetProviderLoadCompletedEvent;
};
