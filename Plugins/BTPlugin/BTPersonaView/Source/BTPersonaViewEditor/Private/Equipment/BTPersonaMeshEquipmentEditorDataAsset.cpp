//--------------------------------------------------
//! @file   BTPersonaMeshEquipmentEditorDataAsset.cpp
//! @brief  ペルソナMesh機器情報を提供するデータアセット
//! @author BT.Masaki Okuda
//--------------------------------------------------
#include "Equipment/BTPersonaMeshEquipmentEditorDataAsset.h"

#ifdef ENABLE_BT_PERSONA_MESH_DATA_AUTO_ATTACH
static_assert(false, "ENABLE_BT_PERSONA_MESH_DATA_AUTO_ATTACH is enabled. This is for debug only.");
#endif
// 自動アタッチされるようにする
#define ENABLE_BT_PERSONA_MESH_DATA_AUTO_ATTACH 0

void UBTPersonaMeshEquipmentEditorDataAsset::GetPersonaEquipmentData(TArray<FBTPersonaEquipmentData>& OutData) const
{
	for( const FBTPersonaMeshEquipmentEditorPreviewData& PreviewDataElm : MeshEquipmentPreviewData )
	{
		if( PreviewDataElm.PreviewMesh.IsNull() )
		{
			continue;
		}

		FBTPersonaEquipmentData EquipmentData;
		EquipmentData.DisplayName = PreviewDataElm.EquipmentDisplayText;

		FBTPersonaEquipmentComponentData& ComponentDataRef = EquipmentData.ComponentData.AddDefaulted_GetRef();
		ComponentDataRef = BTPersonaEquipmentHelpers::CreateComponentDataFromAssetPath(PreviewDataElm.PreviewMesh.ToSoftObjectPath());
		ComponentDataRef.AttachSocketName = PreviewDataElm.AttachSocketName;
		ComponentDataRef.AttachRelativeTransform = PreviewDataElm.AttachRelativeTransform;

		OutData.Add(MoveTemp(EquipmentData));
	}
}

void UBTPersonaMeshEquipmentEditorDataAsset::FindAutoCreatablePersonaEquipmentData(TArray<FBTPersonaEquipmentComponentData>& OutComponentsData, const UAnimationAsset* InTestAnimAsset, const USkeletalMesh* InPreviewMesh) const
{
#if ENABLE_BT_PERSONA_MESH_DATA_AUTO_ATTACH
	for( const FBTPersonaMeshEquipmentEditorPreviewData& PreviewDataElm : MeshEquipmentPreviewData )
	{
		if( PreviewDataElm.PreviewMesh.IsNull() )
		{
			continue;
		}
		

		FBTPersonaEquipmentComponentData& ComponentDataRef = OutComponentsData.AddDefaulted_GetRef();
		ComponentDataRef = BTPersonaEquipmentHelpers::CreateComponentDataFromAssetPath(PreviewDataElm.PreviewMesh.ToSoftObjectPath());
		ComponentDataRef.AttachSocketName = PreviewDataElm.AttachSocketName;
		ComponentDataRef.AttachRelativeTransform = PreviewDataElm.AttachRelativeTransform;
	}
#endif
}

#undef ENABLE_BT_PERSONA_MESH_DATA_AUTO_ATTACH
