// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "UI/SNUIDef.h"
#include "SNSceneStateTreeTaskBase.generated.h"

struct FStreamableHandle;
class  USNUserWidgetBase;
class ASNSceneBase;

//----------------------------------------------------------------------//
//
//! @brief シーン用のステートツリーベースクラス
//
//----------------------------------------------------------------------//
UCLASS()
class SNPLUGIN_API USNSceneStateTreeTaskBase : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
protected:
	
	//! @{@name タスク開始
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	//! @}
	
	//! @{@name タスク終了
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	//! @}
	
	//! @{@name ウィジェットへのポインタを取得
	template<class T>
	T*		GetHudWidget();
	
	template<class T>
	TArray<TObjectPtr<T>>	GetHudWidgets();
	//! @}

	ASNSceneBase* GetCurrentScene() const ;

	bool SetWidgetLayer(EWidgetLayer Layer, USNUserWidgetBase* Widget);

	bool SetVisibleWidget(EWidgetLayer Layer, USNUserWidgetBase* Widget);
	
protected:
	
	//! @{@name ウィジェットクラスのロード後に呼ばれる処理
	virtual void HudPostLoad();
	//! @}
	
private:
	
	//! @{@name ウィジェットクラスの非同期ロード処理
	void LoadHudClassASync();
	//! @}
	
	//! @{@name ウィジェットクラスのロード後に呼ばれる処理
	void FinishLoadHudClass();
	//! @}
	
	//!< ウィジェットのクラス情報リスト
	UPROPERTY(EditAnywhere, Category="Scene|UI")
	TArray<TSoftClassPtr<USNUserWidgetBase>> HudClassList;
	
	//!< ウィジェットのインスタンスリスト
	UPROPERTY()
	TArray<TObjectPtr<USNUserWidgetBase>> HudInsntaceList;
	
	//!< 非同期ロード用のハンドル
	TSharedPtr<FStreamableHandle> HudStreamHundle = nullptr;
};

//----------------------------------------------------------------------//
//
//! @brief ウィジェットへのポインタを取得
//
//! @retval ウィジェットへのポインタ
//
//----------------------------------------------------------------------//
template <class T>
FORCEINLINE T* USNSceneStateTreeTaskBase::GetHudWidget(){
	
	for(USNUserWidgetBase* Widget:HudInsntaceList){
		
		if(T* Result = Cast<T>(Widget)){
			return Result;
		}
	}
	
	return nullptr;
}

//----------------------------------------------------------------------//
//
//! @brief ウィジェットへのポインタを取得
//
//! @retval ウィジェットへのポインタリスト
//
//----------------------------------------------------------------------//
template<class T>
FORCEINLINE TArray<TObjectPtr<T>> USNSceneStateTreeTaskBase::GetHudWidgets(){
	
	TArray<TObjectPtr<T>> Widgets;
	
	for(USNUserWidgetBase* Widget:HudInsntaceList){
		
		if(T* Result = Cast<T>(Widget)){
			Widgets.Add(Result);
		}
	}
	
	return Widgets;
}
