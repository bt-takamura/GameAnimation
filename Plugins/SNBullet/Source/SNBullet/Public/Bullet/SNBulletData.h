// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SNBulletData.generated.h"

class ASNBulletBase;

//!@{@defgroup 弾
//!@{
//----------------------------------------------------------------------//
//
//! @brief 弾クラスのデータ管理構造体
//
//! 各弾の種類毎に生成する弾の数を基に弾のアクターを使いまわすための構造体。
//
//----------------------------------------------------------------------//
USTRUCT()
struct SNBULLET_API FSNBulletData
{
	GENERATED_BODY()
	
	//! @{@name デフォルトコンストラクタ
	FSNBulletData();
	//! @}
	
	//! @{@name クラス情報をロード(同期)
	bool LoadClass(const TSoftClassPtr<ASNBulletBase>& BulletClass);
	//! @}
	
	//! @{@name 弾を登録
	void RegistBullet(ASNBulletBase* Bullet);
	//! @}
	
	//! @{@name 弾を取得
	ASNBulletBase* GetBullet();
	//! @}
	
	//! @{@name 弾の再利用
	void RecycleBullet(ASNBulletBase* Bullet);
	//! @}
	
	//! @{@name 弾の生成リクエスト数を設定
	void SetRequestNum(int Num);
	//! @}
	
	//! @{@name 弾の生成リクエスト数を取得
	int GetRequestNum() const ;
	//! @}
	
	//! @{@name 弾のクラス情報を取得
	UClass* GetClassPtr() const ;
	//! @}
	
private:
	//!< 弾の生成数
	UPROPERTY()
	int RequestNum = 0;
	
	//!< 弾のクラス情報
	UPROPERTY()
	TObjectPtr<UClass> ClassPtr;
	
	//!< 未使用の弾のリスト
	UPROPERTY()
	TArray<TObjectPtr<ASNBulletBase>> EmptyBullet;
	
	//!< 使用中の弾のリスト
	UPROPERTY()
	TArray<TObjectPtr<ASNBulletBase>> UsedBullet;
};
//! @}
//! @}

//----------------------------------------------------------------------//
//
//! @brief 弾を登録
//
//! @param Bullet 弾のインスタンス
//
//----------------------------------------------------------------------//
FORCEINLINE void FSNBulletData::RegistBullet(ASNBulletBase* Bullet){
	EmptyBullet.Add(Bullet);
}

//----------------------------------------------------------------------//
//
//! @brief 弾の生成リクエスト数を設定
//
//! @param Num 弾の生成リクエスト数
//
//----------------------------------------------------------------------//
FORCEINLINE void FSNBulletData::SetRequestNum(int Num){
	RequestNum = Num;
}

//----------------------------------------------------------------------//
//
//! @brief 弾の生成リクエスト数を取得
//
//! @retval 弾の生成リクエスト数
//
//----------------------------------------------------------------------//
FORCEINLINE int FSNBulletData::GetRequestNum() const {
	return RequestNum;
}

//----------------------------------------------------------------------//
//
//! @brief 弾のクラス情報を取得
//
//! @retval 弾のクラス情報
//
//----------------------------------------------------------------------//
FORCEINLINE UClass* FSNBulletData::GetClassPtr() const {
	return ClassPtr;
}
