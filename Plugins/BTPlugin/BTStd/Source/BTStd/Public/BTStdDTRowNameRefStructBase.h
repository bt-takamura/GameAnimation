//--------------------------------------------------
//! @file   BTStdDTRowNameRefStructBase.h
//! @brief  データテーブル行参照構造体の基底クラス
//! @author BT.Masaki Okuda
//--------------------------------------------------
#pragma once

#include "BTStdDTRowNameRefStructBase.generated.h"

/**
 * @brief データテーブル行名参照構造体の基底構造体
 * 
 * @note この構造体を継承した構造体を作成して下記exampleに従ってmeta = (ReferencedDTRow) を付与したFNameプロパティを作成するとコンボボックス表示にすることができる。
 *		metaを付与したら UE上で「エディタ設定」から「BT Preference」を選択し「データテーブル行参照構造体の表示設定」で参照するデータテーブルを設定することでコンボボックスとして表示される。
 *	@note 上記ReferencedDTRowMetaを付与したプロパティをUSTRUCTのmetaで指定する形で一つだけヘッダ部分に表示させることができる。(デフォルトは表示されない)
 *		DTRowHeader = "プロパティ名"で指定可能、構造体を新たに継承する等でヘッダ部分に表示したくない場合はDTRowHeader = ""で指定する。
 *		
 *	@example:
 *		```
 *		USTRUCT(meta = (DTRowHeader = "HeaderDTRowReferenceProperty"))
 *		struct FMyStruct : public FBTStdDTRowNameRefStructBase{
 *			//通常のプロパティ(通常のFNameプロパティとして表示される) 
 *			UPROPERTY(EditAnywhere)
 *			FName NormalProperty;
 *			
 *			//ヘッダ部分にコンボボックス表示される
 *			UPROPERTY(EditAnywhere, meta = (ReferencedDTRow))
 *			FName HeaderDTRowReferenceProperty;
 *			
 *			//ヘッダ部分ではなく通常のプロパティとしてコンボボックス表示される
 *			UPROPERTY(EditAnywhere, meta = (ReferencedDTRow))
 *			FName NormalDTRowReferenceProperty; 
 *		};
 *		```
 */
USTRUCT(meta = ( Hidden, HeaderShowProperty = ""))
struct BTSTD_API FBTStdDTRowNameRefStructBase {
	GENERATED_BODY()
};
