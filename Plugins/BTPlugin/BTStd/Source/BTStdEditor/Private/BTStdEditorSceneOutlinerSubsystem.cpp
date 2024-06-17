// Fill out your copyright notice in the Description page of Project Settings.

#include "BTStdEditorSceneOutlinerSubsystem.h"
#include "LevelEditor.h"
#include "SSceneOutliner.h"
#include "ISceneOutlinerMode.h"
#include "ActorTreeItem.h"
#include "ActorDescTreeItem.h"
#include "FolderTreeItem.h"

// 初期化処理
void UBTStdEditorSceneOutlinerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

//  終了処理
void UBTStdEditorSceneOutlinerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}


// SceneOutlinerを取得
TSharedPtr<SSceneOutliner> UBTStdEditorSceneOutlinerSubsystem::GetSceneOutliner() const
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked< FLevelEditorModule >(TEXT("LevelEditor"));
	TSharedPtr<ILevelEditor> LevelEditorPtr = LevelEditorModule.GetLevelEditorInstance().Pin();

	if(LevelEditorPtr.IsValid())
	{
		return StaticCastSharedPtr<SSceneOutliner>(LevelEditorPtr->GetMostRecentlyUsedSceneOutliner());
	}
	
	return nullptr;
}

// 選択しているフォルダまたは選択しているActorが所属するフォルダのパスを取得
FName UBTStdEditorSceneOutlinerSubsystem::GetSelectedFolderOrSelectedActorParentFolder()
{
	TSharedPtr<SSceneOutliner> SceneOutliner = GetSceneOutliner();
	if(!SceneOutliner.IsValid())
	{
		return NAME_None;
	}

	FName FolderPath = NAME_None;
	
	//「 Outliner」で選択中の項目でFor文を回し、最初にヒットした種類によってフォルダ取得方法を分岐
	SceneOutliner->GetSelection().ForEachItem<ISceneOutlinerTreeItem>([&FolderPath](ISceneOutlinerTreeItem& SelectedItem)
	{
		// Actor選択中の場合は、Actor所属フォルダのパスを返す
		if(FActorTreeItem* ActorItem = SelectedItem.CastTo<FActorTreeItem>())
		{
			if(AActor* SelectedActor = ActorItem->Actor.Get())
			{
				if (SelectedActor)
				{
					FolderPath = ActorItem->Actor.Get()->GetFolderPath();
				}
				return false;
			}
		}
		// UnLoadされているActorが選択中の場合は「Outliner」上の親となるItemをみて、フォルダの場合はそれのパスを返す
		else if(FActorDescTreeItem* ActorDescItem = SelectedItem.CastTo<FActorDescTreeItem>())
		{
			if(TSharedPtr<ISceneOutlinerTreeItem> ParentItem = ActorDescItem->GetParent())
			{
				if(FFolderTreeItem* ParentFolderItem = ParentItem->CastTo<FFolderTreeItem>())
				{
					FolderPath = ParentFolderItem->GetPath();
					return false;
				}
			}
		}
		// フォルダの場合はそのままフォルダのパスを返す
		else if(FFolderTreeItem* FolderItem = SelectedItem.CastTo<FFolderTreeItem>())
		{
			FolderPath = FolderItem->GetPath();
			return false;
		}
		// 取得できなかったら次の選択している項目をチェック
		return true;
	});
	
	return FolderPath; 
}

