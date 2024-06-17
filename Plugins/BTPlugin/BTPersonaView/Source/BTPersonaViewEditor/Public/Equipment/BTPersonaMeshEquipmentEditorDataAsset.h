//--------------------------------------------------
//! @file   BTPersonaMeshEquipmentEditorDataAsset.h
//! @brief  ペルソナMesh機器情報を提供するデータアセット
//! @author BT.Masaki Okuda
//--------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "Equipment/BTPersonaEquipmentProviderAssetInterface.h"

#include "Engine/DataAsset.h"
#include "BTPersonaMeshEquipmentEditorDataAsset.generated.h"


/**
 * @brief プレビュー用メッシュ機器情報
 */
USTRUCT()
struct FBTPersonaMeshEquipmentEditorPreviewData {
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, DisplayName = "表示名")
	FText EquipmentDisplayText;
	UPROPERTY(EditAnywhere, DisplayName = "アタッチソケット")
	FName AttachSocketName;
	UPROPERTY(EditAnywhere, DisplayName = "アタッチ相対位置")
	FTransform AttachRelativeTransform;
	UPROPERTY(EditAnywhere, DisplayName = "表示メッシュ", meta = (AllowedClasses="/Script/Engine.StaticMesh,/Script/Engine.SkeletalMesh", ExactClass="true"))
	// ReSharper disable once CppRedundantTemplateArguments [型を省略するとUE Header 解析でエラーになる ]
	TSoftObjectPtr<UObject> PreviewMesh;
};

/**
 * ペルソナMesh機器情報を提供するデータアセット
 * @note エディタモジュールクラスなのでこのデータアセットクラスをクックに含めないでください。
 */
UCLASS()
class BTPERSONAVIEWEDITOR_API UBTPersonaMeshEquipmentEditorDataAsset final
		: public UDataAsset
		, public IBTPersonaEquipmentProviderAssetInterface {
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Equipment", DisplayName = "ペルソナ表示用機器情報", meta =(TitleProperty ="EquipmentDisplayText"))
	TArray<FBTPersonaMeshEquipmentEditorPreviewData> MeshEquipmentPreviewData;

	// Begin IBTPersonaEquipmentProviderAssetInterface
public:
	virtual void GetPersonaEquipmentData(TArray<FBTPersonaEquipmentData>& OutData) const override;
	virtual void FindAutoCreatablePersonaEquipmentData(TArray<FBTPersonaEquipmentComponentData>& OutComponentsData, const UAnimationAsset* InTestAnimAsset, const USkeletalMesh* InPreviewMesh) const override;
	// End IBTPersonaEquipmentProviderAssetInterface
};
