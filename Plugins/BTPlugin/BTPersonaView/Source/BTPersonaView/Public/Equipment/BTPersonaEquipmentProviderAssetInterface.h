//--------------------------------------------------
//! @file   BTPersonaEquipmentProviderAssetInterface.h
//! @brief  ペルソナプレビューメッシュにアタッチする機能を提供するアセットのインターフェース
//! @author BT.Masaki Okuda
//--------------------------------------------------
#pragma once
#include "CoreMinimal.h"
#include "BTPersonaEquipmentData.h"

#include "UObject/Interface.h"

#include "BTPersonaEquipmentProviderAssetInterface.generated.h"


// This class does not need to be modified.
UINTERFACE()
class BTPERSONAVIEW_API UBTPersonaEquipmentProviderAssetInterface : public UInterface {
	GENERATED_BODY()
};

/**
 * ペルソナプレビューメッシュにアタッチする機能を提供するアセットのインターフェース
 */
class BTPERSONAVIEW_API IBTPersonaEquipmentProviderAssetInterface {
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
#if WITH_EDITOR
	/** 機器データを取得 */
	virtual void GetPersonaEquipmentData(TArray<FBTPersonaEquipmentData>& OutData) const = 0;

	/**
	 * @brief 自動アタッチできる機器データを見つける
	 * @attention 継承時OutComponentsDataの要素をクリアする処理は入れないでください。
	 */
	virtual void FindAutoCreatablePersonaEquipmentData(TArray<FBTPersonaEquipmentComponentData>& OutComponentsData, const UAnimationAsset* InTestAnimAsset, const USkeletalMesh* InPreviewMesh) const = 0;
#endif// WITH_EDITOR
};
