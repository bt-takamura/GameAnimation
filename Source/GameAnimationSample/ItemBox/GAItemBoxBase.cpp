// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/ItemBox/GAItemBoxBase.h"

#include "IDetailTreeNode.h"
#include "SNDef.h"
#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#include "Components/ArrowComponent.h"
#include "GameAnimationSample/System/GAGameInstance.h"
#include "Utility/SNUtility.h"
#include "Sound/SoundCue.h"

//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//----------------------------------------------------------------------//
AGAItemBoxBase::AGAItemBoxBase(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	SetActorTickEnabled(false);
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	
	LidPivotComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("LidPivot"));
	
	ItemBoxBaseComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemBoxBase"));
	
	ItemBoxLidComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemBoxLid"));
	
	LidPivotComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	
	ItemBoxBaseComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	
	ItemBoxLidComponent->AttachToComponent(LidPivotComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

//----------------------------------------------------------------------//
//
//! @brief 宝箱のオープンリクエスト
//
//----------------------------------------------------------------------//
void AGAItemBoxBase::OpenRequest(){
	// すでにタイマーハンドラーが有効な場合は開閉中とみなす
	if(TimerHandle.IsValid()){
		
		SNPLUGIN_LOG(TEXT("OpenRequest : ItemBox Already Request."));
		
		return;
	}
	// 現在すでに開いている場合は処理を呼ばない
	if(IsOpening == true){
		return;
	}
	
	FTimerDelegate Delegate;
	// オープンファンクションをバインド
	Delegate.BindUObject(this, &AGAItemBoxBase::Open);
	// タイマーハンドラーを設定
	GetWorldTimerManager().SetTimer(TimerHandle, Delegate, 0.016f, true);
	// 音が設定されている場合はSEを再生
	if(OpenSound != nullptr){
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Cast<USoundBase>(OpenSound), GetActorLocation(), GetActorRotation());	
	}
	// オープン中フラグをON
	IsOpening = true;
}

//----------------------------------------------------------------------//
//
//! @brief 宝箱のクローズリクエスト
//
//----------------------------------------------------------------------//
void AGAItemBoxBase::CloseRequest(){
	// すでにタイマーハンドラーが有効な場合は開閉中とみなす
	if(TimerHandle.IsValid()){
		
		SNPLUGIN_LOG(TEXT("CloseRequest : ItemBox Already Request."));
		
		return;
	}
	// 現在すでに閉じている場合は処理を呼ばない
	if(IsOpening == false){
		return;
	}
	
	FTimerDelegate Delegate;
	// クローズファンクションをバインド
	Delegate.BindUObject(this, &AGAItemBoxBase::Close);
	// タイマーハンドラーを設定
	GetWorldTimerManager().SetTimer(TimerHandle, Delegate, 0.016f, true);
	// 音が設定されている場合はSEを再生
	if(CloseSound != nullptr){
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Cast<USoundBase>(CloseSound), GetActorLocation(), GetActorRotation());	
	}
}

//----------------------------------------------------------------------//
//
//! @brief 蓋が開く処理
//
//----------------------------------------------------------------------//
void AGAItemBoxBase::Open(){
	
	FRotator Rotator(LidPivotComponent->GetComponentRotation());
	// 回転値を設定
	Rotator.Roll -= Speed;
	// -90度まで開いた場合は処理終了
	if(Rotator.Roll <=-90.0f){
		// クランプ
		Rotator.Roll =-90.0f;
		// タイマーをクリア
		GetWorldTimerManager().ClearTimer(TimerHandle);;
		// ハンドルを無効化
		TimerHandle.Invalidate();
	}
	// 回転値を設定
	LidPivotComponent->SetRelativeRotation(Rotator);
}

//----------------------------------------------------------------------//
//
//! @brief 蓋が閉まる処理
//
//----------------------------------------------------------------------//
void AGAItemBoxBase::Close(){
	
	FRotator Rotator(LidPivotComponent->GetComponentRotation());
	// 回転値を設定
	Rotator.Roll += Speed;
	// 0度まで閉じたら処理終了
	if(Rotator.Roll >= 0.0f){
		// クランプ
		Rotator.Roll = 0.0f;
		// タイマーをクリア
		GetWorldTimerManager().ClearTimer(TimerHandle);;
		// 開封済みに設定
		IsOpened = true;
		// 閉じた状態に設定
		IsOpening = false;
		// ハンドルを無効化
		TimerHandle.Invalidate();
	}
	// 回転値を設定
	LidPivotComponent->SetRelativeRotation(Rotator);
}

//----------------------------------------------------------------------//
//
//! @brief Called when the game starts or when spawned
//
//----------------------------------------------------------------------//
void AGAItemBoxBase::BeginPlay(){
	
	Super::BeginPlay();
	// GameInstanceを取得
	UGAGameInstance* GameInstance(SNUtility::GetGameInstance<UGAGameInstance>());
	
	if(GameInstance != nullptr){
		// アイテム情報を取得
		const FItemData* ItemData(GameInstance->GetItemData(ItemId));
		
		if(ItemData != nullptr){
			// スコアをランダムで取得
			Score = (int)(FMath::RandRange(ItemData->ScoreMin, ItemData->ScoreMax));
			
			int Range = 0;
			// 設定されている比率を全部足して範囲を算出
			for(auto& Item:ItemData->Items){
				Range += Item.Ratio;
			}
			// ランダム生成
			int ItemRatio = FMath::RandRange(0, Range);
			
			int Total = 0;
			
			for(auto& Item:ItemData->Items){
				// ランダム値が範囲内に入っているかチェック
				if(ItemRatio <= Total + Item.Ratio){
					// アイテムタイプを設定
					ItemType = Item.Type;
					
					break;
				}
				
				Total += Item.Ratio;
			}
		}
	}
}

//----------------------------------------------------------------------//
//
//! @brief Called every frame
//
//! @param DeltaTime デルタタイム
//
//----------------------------------------------------------------------//
void AGAItemBoxBase::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
}
