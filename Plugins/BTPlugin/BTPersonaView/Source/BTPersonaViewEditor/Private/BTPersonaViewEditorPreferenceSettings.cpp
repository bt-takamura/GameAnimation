//--------------------------------------------------
//! @file   BTPersonaViewEditorPreferenceSettings.cpp
//! @brief  ペルソナ表示エディターの設定
//! @author BT.Masaki Okuda
//--------------------------------------------------
#include "BTPersonaViewEditorPreferenceSettings.h"

void UBTPersonaViewEditorPreferenceSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 機器アタッチ機能を無効にした場合は自動アタッチ機能も無効にする
	if( PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UBTPersonaViewEditorPreferenceSettings, bEnableAttachEquipment) )
	{
		if( bEnableAttachEquipment == false )
		{
			bEnableAutoAttachEquipment = false;
		}
	}
}
