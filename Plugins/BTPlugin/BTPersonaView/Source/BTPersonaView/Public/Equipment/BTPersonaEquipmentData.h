//--------------------------------------------------
//! @file 	BTPersonaEquipmentData.h
//! @brief 	ペルソナメッシュにアタッチする機器データ
//! @auther BT.Masaki Okuda
//--------------------------------------------------
#pragma once

#if WITH_EDITOR
/**
* @brief コンポーネントセットアップ処理デリゲート
* @return bool true = 成功、false = (必要アセットがない等で)セットアップ失敗
*/
DECLARE_DELEGATE_RetVal_TwoParams(bool, FBTPersonaEquipmentComponentSetUpDelegate, UActorComponent* /* InCreatedComponent */, const struct FBTPersonaEquipmentComponentData* /* InComponentData */);

/**
* @brief ペルソナメッシュにアタッチする機器コンポーネントデータ
* @note コンポーネントセットアップまわりはBTPersonaEquipmentHelpers命名空間にある関数で生成すると楽です。
* @see namespace BTPersonaEquipmentHelpers
*/
struct FBTPersonaEquipmentComponentData {
	/** 生成するコンポーネントのクラス */
	TSubclassOf<UActorComponent> SpawnedComponentClass;
	/** コンポーネント生成までにロードする必要のあるアセットリスト */
	TArray<FSoftObjectPath, TInlineAllocator<2>> AssetPathList;
	/** [USceneComponent派生限定]アタッチソケット名 */
	FName AttachSocketName;
	/** [USceneComponent派生限定]アタッチ相対位置 */
	FTransform AttachRelativeTransform;
	/** 生成コンポーネントに付与するタグ(Noneの場合タグを追加しない) */
	FName ComponentTag;
	/** コンポーネントセットアップ処理デリゲート */
	FBTPersonaEquipmentComponentSetUpDelegate ComponentSetUpDelegate;

public:
	/** データは有効か */
	BTPERSONAVIEW_API bool IsValidData() const;

	/** AssetPathListのロード済みのアセットを検索
	 * @note 同じクラスのアセットが複数ある場合、最初に見つかったものを返す
	 */
	BTPERSONAVIEW_API UObject* FindLoadedAssetFirst(UClass* InTargetAssetClass) const;

	template<typename TAssetType>
	FORCEINLINE TAssetType* FindLoadedAssetFirst() const { return Cast<TAssetType>(FindLoadedAssetFirst(TAssetType::StaticClass())); }

	/** 文字列化 */
	BTPERSONAVIEW_API FString ToString() const;
};

/**
* @brief ペルソナメッシュにアタッチする機器データ
*/
struct FBTPersonaEquipmentData {
	/** プレビューウィンドウで表示するテキスト(Emptyの場合、アセット名 + (データ配列Index)を使用) */
	FText DisplayName;
	/** マウスオーバー時に表示するToolTipテキスト(Emptyの場合、DisplayNameを使用) */
	FText ToolTip;
	/** 機器コンポーネントデータ */
	TArray<FBTPersonaEquipmentComponentData, TInlineAllocator<4>> ComponentData;
};
#endif // WITH_EDITOR

namespace BTPersonaEquipmentHelpers
	{
#if WITH_EDITOR
		/** アセット型に対応したコンポーネントデータを作成(ex, StaticMeshアセット -> StaticMeshComponent) */
		BTPERSONAVIEW_API FBTPersonaEquipmentComponentData CreateComponentDataFromAssetPath(const FSoftObjectPath& InAssetPath);
		/** スタティックメッシュコンポーネントデータを作成 */
		BTPERSONAVIEW_API FBTPersonaEquipmentComponentData CreateStaticMeshComponentData(const TSoftObjectPtr<UStaticMesh>& InMesh);
		/** スケルタルメッシュコンポーネントデータを作成 */
		BTPERSONAVIEW_API FBTPersonaEquipmentComponentData CreateSkeletalMeshComponentData(const TSoftObjectPtr<USkeletalMesh>& InMesh);
#endif // WITH_EDITOR
	}// namespace BTPersonaEquipmentHelpers
