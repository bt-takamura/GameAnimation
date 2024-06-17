// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet/SNBulletManager.h"

#include "SNDef.h"
#include "Bullet/SNBulletBase.h"
#include "Data/SNContentAssetManager.h"
#include "Data/SNContentsAsset.h"
#include "System/SNGameInstance.h"
#include "Utility/SNUtility.h"

//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//----------------------------------------------------------------------//
USNBulletManager::USNBulletManager(){
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	SetComponentTickEnabled(false);
	
	SetIsReplicatedByDefault(true);
	
	SetNetAddressable();
	
	BulletMap.Empty();
}

//----------------------------------------------------------------------//
//
//! @brief レプリケーションするメンバを取得
//
//! @param OutLifetimeProps レプリケーションするメンバの情報
//
//----------------------------------------------------------------------//
void USNBulletManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

//----------------------------------------------------------------------//
//
//! @brief 毎フレームの更新処理
//
//! @param DeltaTime        経過時間
//! @param TickType         タイプ
//! @param ThisTickFunction Tick処理に関する情報
//
//! @note TickComponentは弾の生成の時のみコールされ、全ての弾の生成が終わると止まる。
//
//----------------------------------------------------------------------//
void USNBulletManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// 弾を生成
	if(ExecSpawnBullet() == false){
		SetComponentTickEnabled(false);
	}
	// ...
}

//----------------------------------------------------------------------//
//
//! @brief 弾のデータアセットから各弾を初期化
//
//----------------------------------------------------------------------//
bool USNBulletManager::SetupBulletDataAsset(){
	// GameInstanceを取得
	USNGameInstance* GameInstance(SNUtility::GetGameInstance<USNGameInstance>());
	
	if(GameInstance == nullptr){
		
		SNPLUGIN_LOG(TEXT("BulletManager : GameInstance is nullptr."));
		
		return false;
	}
	// ContentsAssetManagerを取得
	USNContentAssetManager* DataAssetManager(GameInstance->GetContentAssetManager());
	
	if(DataAssetManager == nullptr){
		
		SNPLUGIN_LOG(TEXT("BulletManager : Data Asset Manager is nullptr."));
		
		return false;
	}
	// 弾用のコンテンツアセットを取得
	const USNContentsAsset* BulletAsset(DataAssetManager->GetContentAsset(FName(TEXT("Bullet"))));
	
	if(BulletAsset == nullptr){
		
		SNPLUGIN_LOG(TEXT("BulletManager : Data Asset Manager is nullptr."));
		
		return false;
	}
	// クラス情報を取得
	const TMap<FName, TSoftClassPtr<UObject>>& BulletClassList(BulletAsset->GetContentClassList());
	
	for(auto& Bullet:BulletClassList){
		// これって…失敗することないのかな…？
		TSoftClassPtr<ASNBulletBase> BulletClass(Bullet.Value.ToSoftObjectPath());
		// @note 生成する弾の数が固定値のところを考える。
		RequestSpawn(Bullet.Key, BulletClass, 200);
	}
	
	return true;
}

//----------------------------------------------------------------------//
//
//! @brief スポーンのリクエスト
//
//! @param Key         Mapのキー情報
//! @param BulletClass スポーンする弾のクラス情報
//! @param Num         スポーンする個数
//
//----------------------------------------------------------------------//
void USNBulletManager::RequestSpawn(const FName& Key, const TSoftClassPtr<ASNBulletBase>& BulletClass, int Num){
	// スポーンはサーバーのみ
	if(SNUtility::IsServer(GetWorld())){
		// 弾のリストに情報があるかチェック
		if(BulletMap.Contains(Key) == false){
			// リストに登録
			BulletMap.Add(Key, FSNBulletData());
			// クラス情報をロード
			BulletMap[Key].LoadClass(BulletClass);
			// リクエスト数を設定
			BulletMap[Key].SetRequestNum(Num);
			// コンポーネントのTickを有効化
			SetComponentTickEnabled(true);
		} else {
			// すでに登録済みのものがリクエストされることはないはずなので警告
			SNPLUGIN_WARNING(TEXT("Bullet [%s] is already spawned."), *Key.ToString());
		}
	}
}

//----------------------------------------------------------------------//
//
//! @brief 弾を取得
//
//! @param Name 弾の名前
//
//! @retval 弾のインスタンスへのポインタ
//
//----------------------------------------------------------------------//
ASNBulletBase* USNBulletManager::GetBullet(const FName& Name){
	// リストに登録されていな場合は処理を終了
	if(BulletMap.Contains(Name) == false){
		
		SNPLUGIN_WARNING(TEXT("Bullet designation is invalid.[%s]"), *Name.ToString());
		
		return nullptr;
	}
	// 弾を取得(FSNBulletData::Get内で有効化されてます)
	ASNBulletBase* Bullet(BulletMap[Name].GetBullet());

#if !UE_BUILD_SHIPPING
	// nullの場合は弾の数が足りてない
	if(Bullet == nullptr){
		SNPLUGIN_WARNING(TEXT("Not enouch bullets.[%s]"), *Name.ToString());
	}
#endif
	
	return Bullet;
}

//----------------------------------------------------------------------//
//
//! @brief 弾を再利用
//
//! @param Bullet リサイクルする弾へのポインタ
//
//----------------------------------------------------------------------//
void USNBulletManager::RecycleBullet(ASNBulletBase* Bullet){
	// nullの場合は処理をしない
	if(Bullet == nullptr){
		
		SNPLUGIN_WARNING(TEXT("Invalid bullets recycled."));
		
		return;
	}
	// 弾の名前を取得
	FName Name(Bullet->GetBulletKey());
	// 管理リストに存在するかチェック
	if(BulletMap.Contains(Name) == false){
		// 存在しない場合はアサート
		SNPLUGIN_ASSERT(false, TEXT("Uncontrolled bullets materialise."));
	}
	// 弾をリサイクル
	BulletMap[Name].RecycleBullet(Bullet);

}

//----------------------------------------------------------------------//
//
//! @brief 弾の登録処理
//
//! @param Name   Mapのキー情報
//! @param Bullet 登録する弾のポインタ
//
//----------------------------------------------------------------------//
void USNBulletManager::RegistBullet(const FName& Name, ASNBulletBase* Bullet){
	// リストにない場合は登録…するけど、一応ワーニング
	if(BulletMap.Contains(Name) == false){
		
		SNPLUGIN_WARNING(TEXT("Regist Bullet does not contains."));
		
		BulletMap.Add(Name, FSNBulletData());
	}
	// 弾を登録
	BulletMap[Name].RegistBullet(Bullet);
}

//----------------------------------------------------------------------//
//
//! @brief 弾を1つスポーン
//
//! @param Class スポーンする弾のクラス情報
//! @param Key   Mapのキー情報
//
//----------------------------------------------------------------------//
void USNBulletManager::SpawnBullet(TSubclassOf<ASNBulletBase> Class, const FName& Key){
	// プレイヤーコントローラを取得
	APlayerController* PlayerController(SNUtility::GetPlayerController<APlayerController>());
	// プレイヤーコントローラがない場合はアサート
	SNPLUGIN_ASSERT(PlayerController != nullptr, TEXT("Player Controller is nullptr"));
	
	FActorSpawnParameters Parameters;
	// オーナーをプレイヤーコントローラに設定
	Parameters.Owner = PlayerController;
	// 弾をスポーン
	ASNBulletBase* Bullet(Cast<ASNBulletBase>(GetWorld()->SpawnActor(Class.Get(), nullptr, Parameters)));
	
	if(Bullet != nullptr){
		// 生成後は弾を無効化
		Bullet->SetValidattion(false);
		// キー情報を設定
		Bullet->SetBulletKey(Key);
		// 弾を登録
		RegistBullet(Key, Bullet);
	} else {
		SNPLUGIN_ERROR(TEXT("Bullet spawn failed."));
	}
}

//----------------------------------------------------------------------//
//
//! @brief リクエストされた弾のスポーンを実行
//
//! @retval true  スポーン中
//! @retval false スポーン終了
//
//----------------------------------------------------------------------//
bool USNBulletManager::ExecSpawnBullet(){
	// サーバーじゃない場合はスポーンしない
	if(SNUtility::IsServer(GetWorld()) == false){
		return false;
	}
	
	bool bDoSpawn = false;
	
	int Num = Distributed;
	
	for(auto& BulletData:BulletMap){
		// 残り生成するべき個数を取得
		int RequestNum = BulletData.Value.GetRequestNum();
		// リクエスト数が上限値を超えてる場合は上限値分スポーン
		int GenerateNum = (RequestNum > Num) ? Num : RequestNum;
		
		for(int i=0 ; i<GenerateNum ; ++i){
			// 弾をスポーン
			SpawnBullet(BulletData.Value.GetClassPtr(), BulletData.Key);
		}
		// リクエスト数を更新
		BulletData.Value.SetRequestNum(RequestNum - GenerateNum);
		// 生成数が0より大きい場合はフラグを有効化
		if(GenerateNum > 0){
			bDoSpawn = true;
		}
		// 生成可能な数を更新
		Num = Num - GenerateNum;
		// 生成可能な数が0以下になった場合はこのフレームでのスポーンを終了
		if(Num <= 0){
			break;
		}
	}
	
	return bDoSpawn;
}
