// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "SceneOutlinerFilters.h"
#include "BTStdEditorSceneOutlinerSubsystem.generated.h"

// SceneOutlinerの操作を拡張するためのサブシステム
UCLASS()
class BTSTDEDITOR_API UBTStdEditorSceneOutlinerSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:

	// 初期化処理
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 終了処理
	virtual void Deinitialize() override;

	// // SceneOutlinerを取得
	TSharedPtr<SSceneOutliner> GetSceneOutliner() const;

	// 選択しているフォルダまたは選択しているActorが所属するフォルダのパスを取得
	FName GetSelectedFolderOrSelectedActorParentFolder();
};
