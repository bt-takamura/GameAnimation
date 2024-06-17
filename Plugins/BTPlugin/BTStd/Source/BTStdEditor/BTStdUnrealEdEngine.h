// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "Editor/UnrealEdEngine.h"
#include "BTStdUnrealEdEngine.generated.h"

UCLASS()
class BTSTDEDITOR_API UBTStdUnrealEdEngine : public UUnrealEdEngine
{
	GENERATED_UCLASS_BODY()
	
	// 初期化
	virtual void Init(IEngineLoop* InEngineLoop) override;
	
protected:
	// Actor追加時に呼ばれる
	void OnActorSpawned(AActor* SpawnedActor);
	FDelegateHandle ActorSpawnedDelegateHandle;
	
	// Mapが開かれたら呼ばれる
	void OnMapOpened(const FString& Filename, bool bAsTemplate);
	FDelegateHandle MapOpenedDelegateHandle;

	// World削除時に呼ばれる
	void OnDestroyWorld(UWorld* PreWorld);
	FDelegateHandle DestroyWorldDelegateHandle;

protected:
	
	// ワールドの初期化が終わっているか
	bool bWorldInitialized = false;
};