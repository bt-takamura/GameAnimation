// Fill out your copyright notice in the Description page of Project Settings.


#include "Scene/SNSceneBase.h"

#include "Blueprint/UserWidget.h"
#include "System/SNGameInstance.h"
#include "UI/Widget/SNMasterWidget.h"
#include "Utility/SNUtility.h"

// Sets default values
//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//----------------------------------------------------------------------//
ASNSceneBase::ASNSceneBase()
:Super()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 最初はTickをOFF
	PrimaryActorTick.bStartWithTickEnabled = false;
	// TickをOff
	SetActorTickEnabled(false);
	// ルートコンポーネントを設定
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
}

//----------------------------------------------------------------------//
//
//! @brief ゲーム開始時またはスポーン時にコールされる
//
//----------------------------------------------------------------------//
void ASNSceneBase::BeginPlay(){
	
	Super::BeginPlay();
	// マスターウィジェットのクラス情報をロード
	UClass* Class = Cast<UClass>(MasterWidgetClass.LoadSynchronous());

	USNGameInstance* GameInstance(SNUtility::GetGameInstance<USNGameInstance>());

	if(GameInstance != nullptr)
	{
		GameInstance->SetCurrentScene(this);
	}
	if(Class != nullptr){
		
		APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
		// マスターウィジェットを生成
		MasterWidget = CreateWidget<USNMasterWidget>(PlayerController, Class, TEXT("MasterWidget"));
		// nullチェック
		if(MasterWidget != nullptr){
			// 初期化処理
			MasterWidget->Init();
			
			if((MasterWidget->IsInViewport() == false  )
			&& (MasterWidget->GetParent()    == nullptr)){
				// ビューポートに追加
				MasterWidget->AddToViewport();
			}
		}
	}
	
}

//----------------------------------------------------------------------//
//
//! @brief フレームごとに呼び出される
//
//! @param DeltaTime 経過時間
//
//----------------------------------------------------------------------//
void ASNSceneBase::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}

//----------------------------------------------------------------------//
//
//! @brief プレイ終了時に呼ばれる。
//
//! @param EndPlayReason 終了理由
//
//----------------------------------------------------------------------//
void ASNSceneBase::EndPlay(const EEndPlayReason::Type EndPlayReason){
	
	Super::EndPlay(EndPlayReason);
	
	if(MasterWidget != nullptr){
		// 登録されているウィジェットをすべてクリア
		MasterWidget->ClearAll();
		// マスターウィジェットを破棄
		MasterWidget->ConditionalBeginDestroy();
		
		MasterWidget = nullptr;
	}
}
