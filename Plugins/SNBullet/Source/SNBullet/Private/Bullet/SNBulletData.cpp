// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet/SNBulletData.h"

#include "Bullet/SNBulletBase.h"

//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//----------------------------------------------------------------------//
FSNBulletData::FSNBulletData():
 RequestNum(0)
{
	// 未使用の弾をクリア
	EmptyBullet.Empty();
	// 使用中の弾をクリア
	UsedBullet.Empty();
}

//----------------------------------------------------------------------//
//
//! @brief クラス情報をロード(同期)
//
//! @param BulletClass 弾クラスのクラス情報
//
//! @retval true  正常終了
//! @retval false 異常終了
//
//----------------------------------------------------------------------//
bool FSNBulletData::LoadClass(const TSoftClassPtr<ASNBulletBase>& BulletClass){
	
	ClassPtr = Cast<UClass>(BulletClass.LoadSynchronous());
	
	return (ClassPtr != nullptr) ? true : false;
}

//----------------------------------------------------------------------//
//
//! @brief 弾を取得
//
//! @retval 弾へのポインタ
//
//----------------------------------------------------------------------//
ASNBulletBase* FSNBulletData::GetBullet(){
	// 未使用の弾がない場合はnullを返す
	if(EmptyBullet.Num() <= 0){
		return nullptr;
	}
	// 未使用リストから取り出し
	ASNBulletBase* Bullet(EmptyBullet.Pop());
	// 弾を有効化
	Bullet->SetValidattion(true);
	// 使用中リストへ登録
	UsedBullet.Add(Bullet);
	
	return Bullet;
}

//----------------------------------------------------------------------//
//
//! @brief 弾の再利用
//
//! @param Bullet 弾へのポインタ
//
//----------------------------------------------------------------------//
void FSNBulletData::RecycleBullet(ASNBulletBase* Bullet){
	// 使用中リストから削除
	UsedBullet.Remove(Bullet);
	// 弾を無効化
	Bullet->SetValidattion(false);
	// 未使用リストに登録
	EmptyBullet.Add(Bullet);
}
