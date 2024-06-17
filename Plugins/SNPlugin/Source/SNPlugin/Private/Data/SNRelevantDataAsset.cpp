// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/SNRelevantDataAsset.h"

#include <ios>
#include <fstream>

#include "SNDef.h"
#include "Data/SNRelevantAssetDataTable.h"
#include "Utility/SNUtility.h"

class HeaderOutput
{
public:
	 HeaderOutput():OutputStream(nullptr){}
	~HeaderOutput(){
		
		 if(OutputStream != nullptr){
			
			 delete OutputStream;
			
		 	OutputStream = nullptr;
		 }
	}
	
	bool Setup(const FName& FilePath, const FString& AssetName){
		
		FString Path(FilePath.ToString());
		
		if(Path[Path.Len()-1] != TEXT('\\')){
			Path += TEXT("/");
		}
		
		int PreFixPos = AssetName.Find("_");
		
		FString Temp(AssetName);
		
		if(PreFixPos >= 0){
			Temp = Temp.RightChop(PreFixPos+1);
		}
		
		EnumName = *FString(TEXT("E")+ Temp);
		
		Temp += TEXT(".h");
		
		FString Filename = *(Path + Temp);
		
		OutputStream = new std::wfstream(*Filename, std::ios::out);
		
		if((OutputStream == nullptr) || (OutputStream->fail())){
			return false;
		}
		
		(*OutputStream) << "#include \"Misc/EnumRange.h\"" << std::endl;
		(*OutputStream) << std::endl;
		(*OutputStream) << "UENUM(BlueprintType)" << std::endl;
		(*OutputStream) << "enum class " << EnumName << " : uint8 {" << std::endl;
		
		return true;
	}
	
	void OutputElement(const FString& Key){
		(*OutputStream) << "\t" << EnumName << "_" << *Key << " UMETA(DisplayName = \"" << *Key << "\", ToolTip=\"" << *Key << "\")," << std::endl;
	}
	
	void Finalize(){
		(*OutputStream) << "\tNum UMETA(Hidden)" << std::endl;
		(*OutputStream) << "};" << std::endl;
		(*OutputStream) << std::endl;
		(*OutputStream) << "ENUM_RANGE_BY_COUNT(" << EnumName << ", " << EnumName << "::Num);" << std::endl;
	}
	
private:
	
	std::wstring EnumName;
	
	std::wfstream* OutputStream;
};

//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//! @param objectInitializer イニシャライザ
//
//----------------------------------------------------------------------//
USNRelevantDataAsset::USNRelevantDataAsset(const FObjectInitializer& objectInitializer)
: bAssetLoaded(false)
{
	
}

//----------------------------------------------------------------------//
//
//! @brief ロードリクエスト設定処理
//
//----------------------------------------------------------------------//
void	USNRelevantDataAsset::Setup(){
	LoadAssetAsync();
}

//----------------------------------------------------------------------//
//
//! @brief 終了処理
//
//----------------------------------------------------------------------//
void USNRelevantDataAsset::Finalize(){
	// 参照を削除
	AssetMap.Empty();
	// イベントのバインドを削除
	OnAssetLoadFinishedEvent.Clear();
}

//----------------------------------------------------------------------//
//
//! @brief 破棄される時の処理
//
//----------------------------------------------------------------------//
void USNRelevantDataAsset::BeginDestroy(){
	
	Super::BeginDestroy();
	
	Finalize();
}

//----------------------------------------------------------------------//
//
//! @brief アセットのロード処理
//
//! @retval true  正常終了
//! @retval false 異常終了
//
//----------------------------------------------------------------------//
bool	USNRelevantDataAsset::LoadAssetAsync(){
	// nullチェック
	if(AssetList == nullptr){
		
		SNPLUGIN_LOG(TEXT("USNRelevantDataAsset::LoadAssetAsync [%s] DataTable doesn't set\n"), *GetName());
		// アニメーションリストがなくてもOKとする
		return true;
	}
	
	TArray<FSoftObjectPath> assetList;
	// データテーブル登録されているアセットをストリーミングリストに登録
	AssetList->ForeachRow(TEXT(""), TFunctionRef<void(const FName&, const FSNRelevantAssetDataTable&)>(
			// アセットをストリーミングリストに登録
			[&](const FName& key, const FSNRelevantAssetDataTable& info){
				
				(void)key;
				// リストに登録
				assetList.Add(info.Asset.ToSoftObjectPath());
			}
		)
	);
	// ロードをリクエスト
	SNUtility::RequestAsyncLoad<USNRelevantDataAsset>(assetList, this, &USNRelevantDataAsset::FinishedLoadAsset);
	
	return true;
}

//----------------------------------------------------------------------//
//
//! @brief ロード終了時のイベント
//
//----------------------------------------------------------------------//
void	USNRelevantDataAsset::FinishedLoadAsset(){
	// データテーブル登録されているアセットをアニメーションマップに登録
	AssetList->ForeachRow(TEXT(""), TFunctionRef<void(const FName&, const FSNRelevantAssetDataTable &)>(
			// アセットをアニメーションマップに登録
			[&](const FName& key, const FSNRelevantAssetDataTable & info){
				AssetMap.Add(key, info.Asset.Get());
			}
		)
	);
	// アニメーションロード終了フラグをON
	bAssetLoaded = true;
	// イベント発火
	OnAssetLoadFinishedEvent.Broadcast();
}

//----------------------------------------------------------------------//
//
//! @brief アセットへのポインタを取得
//
//! @param key アセット登録時のキー
//
//! @retval アセットへのポインタ
//
//----------------------------------------------------------------------//
UObject*	USNRelevantDataAsset::GetAsset(const FName& key){
	
	if(AssetMap.Find(key) == nullptr){
		return nullptr;
	}
	
	return AssetMap[key].Get();
}

#if WITH_EDITOR
//----------------------------------------------------------------------//
//! @brief AssetPathの設定
//----------------------------------------------------------------------//
void	USNRelevantDataAsset::AddAssetPath(const FAssetSetting& AssetSetting){
#if WITH_EDITORONLY_DATA
	AssetPath.Emplace(AssetSetting);
#endif // WITH_EDITORONLY_DATA
}

//----------------------------------------------------------------------//
//! @brief AssetPathの設定
//----------------------------------------------------------------------//
void	USNRelevantDataAsset::AppendAssetPath(const TArray<FAssetSetting>& AssetSettings){
#if WITH_EDITORONLY_DATA
	AssetPath.Append(AssetSettings);
#endif // WITH_EDITORONLY_DATA
}

//----------------------------------------------------------------------//
//! @brief DataTableの設定
//----------------------------------------------------------------------//
bool	USNRelevantDataAsset::SetDataTable(UDataTable* DataTable){
	
	if(!IsValid(DataTable)){
		
		SNPLUGIN_ERROR(TEXT("%s: Asset(%s)に設定しようとしているDataTableが不正です."), __FUNCTIONW__, *GetFullName());
		
		return false;
	}
	
	if(DataTable->GetRowStruct() != FSNRelevantAssetDataTable::StaticStruct()){
		
		SNPLUGIN_ERROR(TEXT("%s: Asset(%s)に設定しようとしているDataTableの構造体が不正です."), __FUNCTIONW__, *GetFullName());
		
		return false;
	}
	
	AssetList = DataTable;
	
	return true;
}
#endif

//----------------------------------------------------------------------//
//
//! @brief アセットのリビルド(DTを全消去してからの作り直し)
//
//----------------------------------------------------------------------//
void	USNRelevantDataAsset::AssetRebuild(){
#if WITH_EDITORONLY_DATA
	ExecBuild(true);
#endif
}

//----------------------------------------------------------------------//
//
//! @brief アセットのビルド(DTの内容は残しつつの再構築)
//
//----------------------------------------------------------------------//
void	USNRelevantDataAsset::AssetBuild(){
#if WITH_EDITORONLY_DATA
	ExecBuild(false);
#endif
}

#if WITH_EDITORONLY_DATA
//----------------------------------------------------------------------//
//
//! @brief アセットのビルドを実行
//
//! @param isRebuild リビルドを行うかのフラグ
//! @param bShowDialog ダイアログを表示するかどうかのフラグ
//
//----------------------------------------------------------------------//
void	USNRelevantDataAsset::ExecBuild(bool isRebuild, bool bShowDialog/* = true*/){
	
	TMap<FName, FSoftObjectPath> pathList;
	// データテーブルが登録されているかチェック
	if((AssetList != nullptr) && (AssetList->RowStruct == FSNRelevantAssetDataTable::StaticStruct())){
		// データテーブル登録されているアセットをアニメーションマップに登録
		AssetList->ForeachRow(TEXT(""), TFunctionRef<void(const FName&, const FSNRelevantAssetDataTable &)>(
				// アセットをアニメーションマップに登録
				[&](const FName& key, const FSNRelevantAssetDataTable & info){
					// リストに登録
					pathList.Add(key, info.Asset.ToSoftObjectPath());
				}
			)
		);
	} else {
		if(bShowDialog){
			// エラーメッセージを表示して終了
			FString msg(FString::Printf(TEXT("FSNRelevantAssetDataTable指定のデータテーブルが設定されていません。")));
			
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(msg));
		}
		return;
	}
	// リビルドの場合はデータテーブルの内容を消去
	if(isRebuild == true){
		// データテーブルを一旦消去
		AssetList->EmptyTable();
	}
	
	for(auto& setting:AssetPath){
		// 指定されたディレクトリに含まれるアセットを検索
		FARFilter filter;
		// フィルター情報を設定
		filter.PackagePaths.Add(setting.AssetPath);
		filter.bRecursivePaths	= setting.bRecursiveSerarch;
		filter.bRecursiveClasses= true;
		//  アセットレジストリを取得
		IAssetRegistry& assetRegistryModule = FAssetRegistryModule::GetRegistry();
		
		TArray<FAssetData> assets;
		// フィルターからアセット情報を取得
		assetRegistryModule.GetAssets(filter, assets);
		
		for(const FAssetData& asset : assets){
			
			FName key;
			// Valueからキー情報を検索
			const FName* pKey = pathList.FindKey(asset.ToSoftObjectPath());
			// 検索に引っかかった場合はすでにデータテーブルに含まれている
			if(pKey != nullptr){
				key = *pKey;
			} else {
				// アセットが登録されている順番からキー情報を生成
				FString name = FString::Printf(TEXT("%d"), AssetList->GetRowMap().Num());
				
				key = *name;
			}
			
			FSNRelevantAssetDataTable tmp;
			// アセット情報を設定
			tmp.Asset = TSoftObjectPtr<UObject>(asset.ToSoftObjectPath());
			// リビルドではなく、DTにも登録されていない場合は追加
			// (リビルドの場合は消去しているので問答無用で追加)
			if(isRebuild == true){
				AssetList->AddRow(key, tmp);
			} else {
				
				if(pKey == nullptr){
					AssetList->AddRow(key, tmp);
				} else {
					// データテーブルの情報を取得
					FSNRelevantAssetDataTable* DataTable(AssetList->FindRow<FSNRelevantAssetDataTable>(key, FString()));
					
					if(DataTable != nullptr){
						// データを上書き
						*DataTable = tmp;
					}
				}
			}
		}
	}
	
	if(bOutputHeader == true){
		
		if(AssetList->GetRowMap().Num() >= 0xfe){
			
			if(bShowDialog){
				// エラーメッセージを表示して終了
				FString msg(FString::Printf(TEXT("テーブルに含まれているアセットが規定数(255個)を超えています。\nヘッダを使用してEnumでアクセス場合、アセット数を255個以内に収めてください。")));
				
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(msg));
			}
			
			return;
		}
		
		HeaderOutput OutputEnum;
		
		if(OutputEnum.Setup(HeaderPath, GetName()) == true){
			
			AssetList->ForeachRow(TEXT(""), TFunctionRef<void(const FName&, const FSNRelevantAssetDataTable &)>(
					// アセットをアニメーションマップに登録
					[&](const FName& key, const FSNRelevantAssetDataTable & info){
						// リストに登録
						OutputEnum.OutputElement(key.ToString());
					}
				)
			);
			
			OutputEnum.Finalize();
		}
	}
	// 終了メッセージを表示
	if(bShowDialog){
		
		FString msg(FString::Printf(TEXT("参照アセットのビルドが終了しました。")));
		
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(msg));
	}
}
#endif
