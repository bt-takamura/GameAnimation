#pragma once

//! @{@name UIのレイヤー情報
UENUM()
enum class EWidgetLayer:uint8{
	// 描画優先 : 低
	Layer0,
	Layer1,
	Layer2,
	Layer3,
	Layer4,
	Layer5,
	// 描画優先 : 高い
	Num,

	Invalid =0xff,
};
//! @}
