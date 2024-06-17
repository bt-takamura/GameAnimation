// Fill out your copyright notice in the Description page of Project Settings.

#include "BTStdUnrealEdEngine.h"
#include "Engine/Engine.h"
#include "BTStdEditor/Public/BTStdEditorSceneOutlinerSubsystem.h"
#include "BTStdEditor/Public/BTStdEditorPreferenceSettings.h"

UBTStdUnrealEdEngine::UBTStdUnrealEdEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

// 初期化
void UBTStdUnrealEdEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
	
	// アセット配置時に選択しているフォルダに移動する為のデリゲート登録
	ActorSpawnedDelegateHandle = GEngine->OnLevelActorAdded().AddUObject(this, &UBTStdUnrealEdEngine::OnActorSpawned);
	MapOpenedDelegateHandle = FEditorDelegates::OnMapOpened.AddUObject(this, &UBTStdUnrealEdEngine::OnMapOpened);
	DestroyWorldDelegateHandle = FWorldDelegates::OnPreWorldFinishDestroy.AddUObject(this, &UBTStdUnrealEdEngine::OnDestroyWorld);
}

// Actor追加時に呼ばれる
void UBTStdUnrealEdEngine::OnActorSpawned(AActor* SpawnedActor)
{
	check(SpawnedActor);

	if(!bWorldInitialized)
	{
		// Mapを開いている途中やEditor初期化中にSpawnされたものは除外
		return;
	}

	if(const UWorld* World = SpawnedActor->GetWorld())
	{
		if(World->WorldType != EWorldType::Editor)
		{
			return; // PIE等でのSpawnは除外
		}

		if(SpawnedActor->IsChildActor())
		{
			return; // ChildActorは除外
		}

		// EditorPreferenceで自動フォルダ設定が有効か確認
		const UBTStdEditorPreferenceSettings* EditorPreferenceSettings = GetDefault<UBTStdEditorPreferenceSettings>();
		if(EditorPreferenceSettings && EditorPreferenceSettings->bEnableAutoMoveFolderToPlacedActor)
		{
			// 「選択フォルダ」 or 「選択Actorが所属するフォルダ」に配置したActorを所属させる
			UBTStdEditorSceneOutlinerSubsystem* const SceneOutlinerSubsystemBT = GEditor ? GEditor->GetEditorSubsystem<UBTStdEditorSceneOutlinerSubsystem>() : nullptr;
			
			FName IntoFolderPath = NAME_None;
			if ( SceneOutlinerSubsystemBT )
			{
				IntoFolderPath = SceneOutlinerSubsystemBT->GetSelectedFolderOrSelectedActorParentFolder();
			}

			// 選択中の項目で所属可能なフォルダがない or ActorをドラッグにSpawnされる仮のActorはフォルダを設定しない
			if(IntoFolderPath != NAME_None && !FSlateApplication::Get().IsDragDropping())
			{
				SpawnedActor->SetFolderPath(IntoFolderPath);
			}
		}
	}
}

// Mapが開かれたら呼ばれる
void UBTStdUnrealEdEngine::OnMapOpened(const FString& Filename, bool bAsTemplate)
{
	bWorldInitialized = true;
}

// World削除時に呼ばれる
void UBTStdUnrealEdEngine::OnDestroyWorld(UWorld* PreWorld)
{
	// PIE等で破棄される場合は適用しない
	if(PreWorld->WorldType != EWorldType::Editor)
	{
		return;		
	}

	bWorldInitialized = false;
}
