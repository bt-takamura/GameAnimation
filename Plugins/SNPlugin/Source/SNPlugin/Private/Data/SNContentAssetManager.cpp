// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/SNContentAssetManager.h"

#include "SNDef.h"
#include "Data/SNContentsAsset.h"
#include "System/SNDLCModuleInterface.h"

//----------------------------------------------------------------------//
//
//! @brief コンテンツのアセットを取得
//
//! @param Key キー
//
//! @retval コンテンツのアセット
//
//----------------------------------------------------------------------//
const USNContentsAsset* USNContentAssetManager::GetContentAsset(FName Key) const {
	// データが存在するかチェック
	if(DataAssetList.Find(Key) == nullptr){
		return nullptr;
	}

	return DataAssetList[Key];
}

//----------------------------------------------------------------------//
//
//! @brief DLCに含まれるコンテンツの初期化処理
//
//! @retval true  正常終了
//! @retval false 異常終了
//
//----------------------------------------------------------------------//
bool USNContentAssetManager::SetupDLCContents(){
	
	TArray<FName> ModuleDescriptors;
	// DLCのモジュールを検索
	FModuleManager::Get().FindModules(TEXT("*DLC*"), ModuleDescriptors);
	
	for(auto& Module:ModuleDescriptors){
		// モジュールとしてDLCをロード
		ISNDLCModuleInterface* DLCInterface = FModuleManager::LoadModulePtr<ISNDLCModuleInterface>(Module);
		
		if(DLCInterface != nullptr){
			// アセット管理用のアセットを取得
			TSoftClassPtr<USNContentAssetManager> AssetManagerSoftClassPtr = DLCInterface->GetAssetManagerSoftClassPtr();
			
			if(AssetManagerSoftClassPtr.IsNull() == false){
				// クラス情報をロード
				// @@Satoshi Nishimura 2024/03/06 非同期にしたほうが良い場合は要検討
				UClass* AssetManagerClass = AssetManagerSoftClassPtr.LoadSynchronous();
				// nullチェック
				SNPLUGIN_ASSERT(AssetManagerClass != nullptr, TEXT("Failed to Load DLC AssetManager : [%s]"), *Module.ToString());
				// 管理クラスを生成
				USNContentAssetManager* Manager = NewObject<USNContentAssetManager>(this, AssetManagerClass);
				
				for(auto& Contents:Manager->DataAssetList){
					
					USNContentsAsset* ContentAsset = (DataAssetList.Find(Contents.Key) != nullptr) ? DataAssetList[Contents.Key] : nullptr;
					
					if(ContentAsset == nullptr){
						// コンテンツがない場合はクローンとして生成
						ContentAsset = NewObject<USNContentsAsset>(this, Contents.Key, RF_NoFlags, Contents.Value);
						
						DataAssetList.Add(Contents.Key, ContentAsset);
					} else {
						
						for(auto& Object:Contents.Value->ContentsAssetList){
							
							if(ContentAsset->ContentsAssetList.Find(Object.Key) != nullptr){
								// アセットの情報を上書きするログを出力
								SNPLUGIN_LOG(TEXT("Duplicate Asset names : [%s] : %s"), *Module.ToString(), *Object.Key.ToString())
								// アセットの情報を上書き
								ContentAsset->ContentsAssetList[Object.Key] = TSoftObjectPtr<UObject>(Object.Value);
							} else {
								// アセット情報を新規登録
								ContentAsset->ContentsAssetList.Add(Object.Key, TSoftObjectPtr<UObject>(Object.Value));
							}
						}
						
						for(auto& Class:Contents.Value->ContentsClassList){
							
							if(ContentAsset->ContentsClassList.Find(Class.Key) != nullptr){
								// クラスの情報を上書きするログを出力
								SNPLUGIN_LOG(TEXT("Duplicate Class names : [%s] : %s"), *Module.ToString(), *Class.Key.ToString());
								// クラスの情報を上書き
								ContentAsset->ContentsClassList[Class.Key] = TSoftClassPtr<UObject>(Class.Value);
							} else {
								ContentAsset->ContentsClassList.Add(Class.Key, TSoftClassPtr<UObject>(Class.Value));
							}
						}
					}
				}
				// 管理クラスを破棄
				Manager->ConditionalBeginDestroy();
			}
		}
	}

	return true;
}
