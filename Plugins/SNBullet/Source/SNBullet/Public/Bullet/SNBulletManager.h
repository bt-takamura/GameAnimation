// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SNBulletData.h"
#include "Components/ActorComponent.h"
#include "SNBulletManager.generated.h"


//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
class ASNBulletBase;

//!@{@defgroup 弾
//!@{
//----------------------------------------------------------------------//
//
//! @brief 弾管理クラス
//
//----------------------------------------------------------------------//
UCLASS( Blueprintable, meta=(BlueprintSpawnableComponent) )
class SNBULLET_API USNBulletManager : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	
	//! @{@name デフォルトコンストラクタ
	USNBulletManager();
	//! @}
	
	//! @{@name 弾のデータアセットから各弾を初期化
	bool SetupBulletDataAsset();
	//! @}
	
	//! @{@name スポーンのリクエスト
	void RequestSpawn(const FName& Key, const TSoftClassPtr<ASNBulletBase>& BulletClass, int Num);
	//! @}
	
	//! @{@name 毎フレームの更新処理
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
	//! @}
	
	//! @{@name 弾を取得
	ASNBulletBase* GetBullet(const FName& Name);
	//! @}
	
	//! @{@name 弾を再利用
	void RecycleBullet(ASNBulletBase* Bullet);
	//! @}
	
protected:
	
	//! @{@name レプリケーションするメンバを取得
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//! @}
	
	//! @{@name 弾の登録処理
	void RegistBullet(const FName& Name, ASNBulletBase* Bullet);
	//! @}
	
private:
	
	//! @{@name 弾を1つスポーン
	void SpawnBullet(TSubclassOf<ASNBulletBase> Class, const FName& Key);
	//! @}
	
	//! @{@name リクエストされた弾のスポーンを実行
	bool ExecSpawnBullet();
	//! @}
	
	//!< 1回のスポーン処理で生成可能な弾の数
	UPROPERTY(EditAnywhere, Category="Settings", meta=(ClampMin=10))
	int Distributed = 10;
	
	//!< 弾管理用のリスト
	UPROPERTY()
	TMap<FName, FSNBulletData> BulletMap;
};
//! @}
//! @}
