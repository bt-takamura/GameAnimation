// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet/SNBulletBase.h"

//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//----------------------------------------------------------------------//
ASNBulletBase::ASNBulletBase(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 最初はTickを止めておく
	PrimaryActorTick.bStartWithTickEnabled = false;
	// ルートコンポーネントを設定
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	
	SetReplicates(true);
	
	SetReplicateMovement(true);
	
	SetActorTickEnabled(false);
}

//----------------------------------------------------------------------//
//
//! @brief ゲームの開始時、もしくはスポーンされた際に呼ばれます。
//
//----------------------------------------------------------------------//
void ASNBulletBase::BeginPlay(){
	Super::BeginPlay();
}

//----------------------------------------------------------------------//
//
//! @brief 毎フレームの更新処理
//
//! @param DeltaTime 経過時間
//
//----------------------------------------------------------------------//
void ASNBulletBase::Tick(float DeltaTime){
	
	Super::Tick(DeltaTime);
	
	Move();
}

//----------------------------------------------------------------------//
//
//! @brief 弾を有効/無効化
//
//! @param bValidate true : 有効化 / false : 無効化
//
//----------------------------------------------------------------------//
void ASNBulletBase::SetValidattion(bool bValidate){
	// コリジョンの設定
	SetActorEnableCollision(bValidate);
	// ゲーム内での表示設定
	SetActorHiddenInGame(!bValidate);
	// Tickの設定
	SetActorTickEnabled(bValidate);
}

//----------------------------------------------------------------------//
//
//! @brief 有効化した時のトランスフォーム情報を設定
//
//! @param Transform 開始時のトランスフォーム情報
//
//----------------------------------------------------------------------//
void ASNBulletBase::SetStartTransform(const FTransform& Transform){
	// スタートトランスフォームを覚えておく
	StartTransform = Transform;

	SetActorTransform(Transform);
}

//----------------------------------------------------------------------//
//
//! @brief 弾の移動処理
//
//! @note 弾毎に移動処理を変えたい場合、Tickをoverrideするのではなく、この関数をoverrideする。
//
//----------------------------------------------------------------------//
void ASNBulletBase::Move(){
	
	FVector Vec(Speed, 0, 0);
	
	Vec = StartTransform.TransformVector(Vec);
	
	FVector Position(GetActorLocation());
	
	Position += Vec;
	
	SetActorLocation(Position);
}
