// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SNBulletBase.generated.h"

//!@{@defgroup 弾
//!@{
//----------------------------------------------------------------------//
//
//! @brief 弾のベースクラス
//
//! SNBulletManagerで管理する場合は必ずこのクラスからの派生にする必要があります。
//
//----------------------------------------------------------------------//
UCLASS()
class SNBULLET_API ASNBulletBase : public AActor
{
	GENERATED_BODY()
	
public:	
	//! @{@name デフォルトコンストラクタ
	ASNBulletBase();
	//! @}
	
	//! @{@name 毎フレームの更新処理
	virtual void Tick(float DeltaTime) override;
	//! @}
	
	//! @{@name 弾のキー情報を設定
	void SetBulletKey(const FName& Key);
	//! @}
	
	//! @{@name 弾のキー情報を取得
	const FName& GetBulletKey() const ;
	//! @}
	
	//! @{@name 弾を有効/無効化
	void SetValidattion(bool bValidate);
	//! @}
	
	//! @{@name 有効化した時のトランスフォーム情報を設定
	void SetStartTransform(const FTransform& Transform);
	//! @}
	
protected:
	
	//! @{@name ゲームの開始時、もしくはスポーンされた際に呼ばれます。
	virtual void BeginPlay() override;
	//! @}
	
	//! @{@name 弾の移動処理
	virtual void Move();
	//! @}
	
private:
	
	//!< 弾のスピード
	UPROPERTY(EditAnywhere, Category="Settings")
	float Speed = 1.0f;
	
	//!< 弾が所属している種類のキー
	UPROPERTY()
	FName BulletKey = NAME_None;
	
	//!< 動き開始時のトランジション情報
	UPROPERTY()
	FTransform StartTransform;
};
//! @}
//! @}


//----------------------------------------------------------------------//
//
//! @brief 弾のキー情報を取得
//
//! @retval 弾のキー情報
//
//----------------------------------------------------------------------//
FORCEINLINE const FName& ASNBulletBase::GetBulletKey() const {
	return BulletKey;
}

//----------------------------------------------------------------------//
//
//! @brief 弾のキー情報を設定
//
//! @param Key 弾のキー情報
//
//----------------------------------------------------------------------//
FORCEINLINE void ASNBulletBase::SetBulletKey(const FName& Key){
	BulletKey = Key;
}
