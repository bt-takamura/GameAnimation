//--------------------------------------------------
//! @file 	BTPersonaEquipmentData.cpp
//! @brief 	ペルソナメッシュにアタッチする機器データ
//! @auther BT.Masaki Okuda
//--------------------------------------------------
#include "Equipment/BTPersonaEquipmentData.h"

#include "AssetRegistry/IAssetRegistry.h"

namespace BTPersonaEquipmentHelpersPrivate
	{
#if WITH_EDITOR
		/**
		 * @brief パスからアセットクラスを取得する(ロードは発生しない)
		 */
		static UClass* GetClassFromAssetPath(const FSoftObjectPath& AssetPath)
		{
			IAssetRegistry* AssetRegistry = IAssetRegistry::Get();

			if( AssetRegistry == nullptr ) { return nullptr; }

			FAssetData AssetData = AssetRegistry->GetAssetByObjectPath(AssetPath, true);

			// リダイレクタ解決
			// パスがリダイレクタの場合、リダイレクタ先のパスを取得する
			// リダイレクタアセットをロードするとリダイレクタ先のアセットまでロードされてヒッチが発生してしまうので、DestinationObjectタグでリダイレクタ先のパスを取得する
			while( AssetData.IsValid() && AssetData.IsRedirector() )
			{
				FString RedirectedPath;
				if( AssetData.GetTagValue("DestinationObject", RedirectedPath) == false )
				{
					break;
				}

				ConstructorHelpers::StripObjectClass(RedirectedPath);
				FSoftObjectPath RedirectedPathName(RedirectedPath);
				AssetData = AssetRegistry->GetAssetByObjectPath(RedirectedPathName, true);
			}

			if( AssetData.IsValid() == false ) { return nullptr; }

			return AssetData.GetClass(EResolveClass::Yes);
		}
#endif // WITH_EDITOR
	}// namespace BTPersonaEquipmentHelpersPrivate

//////////////////////////////////////////////////////////////////////////
//  FBTPersonaEquipmentComponentData
//////////////////////////////////////////////////////////////////////////
#if WITH_EDITOR
/**
 * @brief データは有効か
 */
bool FBTPersonaEquipmentComponentData::IsValidData() const
{
	return SpawnedComponentClass != nullptr && ComponentSetUpDelegate.IsBound();
}

/**
 * @brief AssetPathListのロード済みのアセットを検索
 */
UObject* FBTPersonaEquipmentComponentData::FindLoadedAssetFirst(UClass* InTargetAssetClass) const
{
	if( AssetPathList.Num() == 0 ) { return nullptr; }
	if( InTargetAssetClass == nullptr ) { return nullptr; }

	for( const FSoftObjectPath& PathListElm : AssetPathList )
	{
		UObject* ResolvedElmObject = PathListElm.ResolveObject();
		if( ResolvedElmObject != nullptr && ResolvedElmObject->IsA(InTargetAssetClass) )
		{
			return ResolvedElmObject;
		}
	}
	return nullptr;
}

/**
 * @brief 文字列化
 */
FString FBTPersonaEquipmentComponentData::ToString() const
{
	TStringBuilder<512> Builder;
	Builder << TEXT("Component Class : ") << GetNameSafe(SpawnedComponentClass.Get()) << TEXT("\n");
	Builder << TEXT("Component Tag : ") << ComponentTag.ToString() << TEXT("\n");
	Builder << TEXT("AssetPathList : \n");
	for( const FSoftObjectPath& PathListElm : AssetPathList )
	{
		Builder << TEXT("\t") << PathListElm.ToString() << TEXT("\n");
	}
	return Builder.ToString();
}
#endif // WITH_EDITOR

//////////////////////////////////////////////////////////////////////////
//  BTPersonaEquipmentHelpers
//////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR

/**
 * @brief アセットパスに対応したコンポーネントデータを作成
 */
FBTPersonaEquipmentComponentData BTPersonaEquipmentHelpers::CreateComponentDataFromAssetPath(const FSoftObjectPath& InAssetPath)
{
	const UClass* MeshAssetClass = BTPersonaEquipmentHelpersPrivate::GetClassFromAssetPath(InAssetPath);
	if( MeshAssetClass == nullptr ) { return FBTPersonaEquipmentComponentData(); }

	// アセットパスからクラスを取得して、クラスに応じたコンポーネントデータを作成する
	if( MeshAssetClass->IsChildOf<USkeletalMesh>() )
	{
		return CreateSkeletalMeshComponentData(TSoftObjectPtr<USkeletalMesh>(InAssetPath));
	}
	 if( MeshAssetClass->IsChildOf<UStaticMesh>() )
	{
		return CreateStaticMeshComponentData(TSoftObjectPtr<UStaticMesh>(InAssetPath));
	}
	UE_LOG(LogTemp, Warning, TEXT("BTPersonaEquipmentHelpers::CreateComponentDataFromAssetPath; Not supported mesh asset class. Class: %s, Path: %s"), *MeshAssetClass->GetName(), *InAssetPath.ToString());
	return FBTPersonaEquipmentComponentData();
}

/**
 * @brief スタティックメッシュ生成コンポーネントデータを作成
 */
FBTPersonaEquipmentComponentData BTPersonaEquipmentHelpers::CreateStaticMeshComponentData(const TSoftObjectPtr<UStaticMesh>& InMesh)
{
	FBTPersonaEquipmentComponentData RetParam;
	if( InMesh.IsNull() ) { return RetParam; }

	RetParam.SpawnedComponentClass = UStaticMeshComponent::StaticClass();
	RetParam.AssetPathList.Add(InMesh.ToSoftObjectPath());

	RetParam.ComponentSetUpDelegate = FBTPersonaEquipmentComponentSetUpDelegate::CreateLambda(
		[](UActorComponent* InCreatedComponent, const FBTPersonaEquipmentComponentData* InComponentData) ->bool
			{
				UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(InCreatedComponent);
				if( StaticMeshComponent == nullptr ) { return false; }
				UStaticMesh* TargetStaticMesh = InComponentData->FindLoadedAssetFirst<UStaticMesh>();
				if( TargetStaticMesh == nullptr )
				{
					return false;
				}

				return StaticMeshComponent->SetStaticMesh(TargetStaticMesh);;
			});

	return RetParam;
}

/**
 * @brief スケルタルメッシュコンポーネントデータを作成
 */
FBTPersonaEquipmentComponentData BTPersonaEquipmentHelpers::CreateSkeletalMeshComponentData(const TSoftObjectPtr<USkeletalMesh>& InMesh)
{
	FBTPersonaEquipmentComponentData RetParam;
	if( InMesh.IsNull() ) { return RetParam; }

	RetParam.SpawnedComponentClass = USkeletalMeshComponent::StaticClass();
	RetParam.AssetPathList.Add(InMesh.ToSoftObjectPath());

	RetParam.ComponentSetUpDelegate = FBTPersonaEquipmentComponentSetUpDelegate::CreateLambda(
		[](UActorComponent* InCreatedComponent, const FBTPersonaEquipmentComponentData* InComponentData) ->bool
			{
				USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(InCreatedComponent);
				if( SkeletalMeshComponent == nullptr ) { return false; }
				USkeletalMesh* TargetSkeletalMesh = InComponentData->FindLoadedAssetFirst<USkeletalMesh>();
				if( TargetSkeletalMesh == nullptr )
				{
					return false;
				}

				SkeletalMeshComponent->SetSkeletalMesh(TargetSkeletalMesh);;
				return true;
			});

	return RetParam;
}

#endif // WITH_EDITOR
