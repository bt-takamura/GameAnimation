// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/SNCharacterDef.h"
#include "SNAnimInstanceBase.generated.h"

class USNRelevantDataAsset;

//!@{@defgroup アニメーション
//!@{
//----------------------------------------------------------------------//
//
//! @brief アニメーションインスタンスのベースクラス
//
//----------------------------------------------------------------------//
UCLASS()
class SNPLUGIN_API USNAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()
public:
	
	//! @{@name 初期化処理
	virtual void NativeInitializeAnimation() override;
	//! @}
	
	//! @{@name 登録されているDAからアニメーションアセットを取得
	UFUNCTION(BlueprintCallable, Category="SN|Animation", meta=(BlueprintThreadSafe))
	const UAnimationAsset* GetAnimationAsset(const FName& Name) const ;
	//! @}
	
	//! @{@name アニメーションシーケンスを取得
	UFUNCTION(BlueprintCallable, Category="SN|Animation", meta=(BlueprintThreadSafe))
	const UAnimSequence* GetAnimationSequence(const FName& Name) const ;
	//! @}
	
	//! @{@name アニメーションモンタージュを取得
	UFUNCTION(BlueprintCallable, Category="SN|Animation", meta=(BlueprintThreadSafe))
	const UAnimMontage* GetAnimationMontage(const FName& Name) const ;
	//! @}
	
	//! @{@name ブレンドスペースを取得
	UFUNCTION(BlueprintCallable, Category="SN|Animation", meta=(BlueprintThreadSafe))
	const UBlendSpace* GetBlendSpace(const FName& Name) const ;
	//! @}
	
	//! @{@name 現在のステートかチェック
	UFUNCTION(BlueprintCallable, Category = "SN|Animation|State", meta=(BlueprintThreadSafe))
	bool IsCurrentState(const FName& State, ECharacterStateType Type=ECharacterStateType::Full) const;
	//! @}
	
	//! @{@name 1つ前のステートかチェック
	UFUNCTION(BlueprintCallable, Category = "SN|Animation|State", meta=(BlueprintThreadSafe))
	bool IsPreState(const FName& State, ECharacterStateType Type=ECharacterStateType::Full) const;
	//! @}
	
	//! @{@name ブレンドスペース用のパラメータを取得
	UFUNCTION(BlueprintPure, Category="SN|Animation|Blendspace", meta=(BlueprintThreadSafe))
	FVector GetBlendspaceParam(const FName& Key);
	//! @}
	
	//! @{@name アニメーションシーケンスを再生
	UFUNCTION(BlueprintCallable, Category="SN|Animation", meta=(BlueprintThreadSafe))
	void PlayAnimationSequence(const FName& Name, const FName& Slot, float PlayRate=1.0f, float BlendIn = 0.05f, float BlendOut = 0.05f, float StartTime=0.0f, bool bLoop=false);
	//! @}
	
	//! @{@name アニメーションモンタージュを再生
	UFUNCTION(BlueprintCallable, Category="SN|Animation", meta=(BlueprintThreadSafe))
	void PlayAnimationMontage(const FName& Name, float PlayRate=1.0f, float StartTime=0.0f);
	//! @}
	
	//! @{@name モンタージュのセクションへのジャンプ
	UFUNCTION(BlueprintCallable, Category="SN|Animation", meta=(BlueprintThreadSafe))
	void JumpAnimationMontageSection(const FName& Name, const FName& Section);
	//! @}
	
	//! @{@name ブレンドスペースのパラメータを設定
	void SetBlendspaceParam(const FName& Key, const FVector& param);
	//! @}

	void BindMontageEndDelegate(FOnMontageEnded& InOnMontageEnded, UAnimMontage* Montage = nullptr);
	
private:
	
	//! @{@name 登録されているDAからアニメーションアセットを取得
	UAnimationAsset* GetAnimationAsset(const FName& Name);
	//! @}
	
	//! @{@name アニメーションシーケンスを取得
	UAnimSequence* GetAnimationSequence(const FName& Name);
	//! @}
	
	//! @{@name アニメーションモンタージュを取得
	UAnimMontage* GetAnimationMontage(const FName& Name);
	//! @}
	
	//! @{@name アニメーションアセットの非同期ロード終了時に呼ばれます。
	void FinishLoadedAnimationAsset();
	//! @}
	
	//!< アニメーションアセット管理用データアセット
	UPROPERTY(EditAnywhere, Category="Asset")
	TObjectPtr<USNRelevantDataAsset> AnimationAssetList = nullptr;
	
	//!< ブレンドスペース用のパラメータマップ
	UPROPERTY()
	TMap<FName, FVector> BlendspaceParam;
};

//----------------------------------------------------------------------//
//
//! @brief アニメーションシーケンスを取得
//
//! @param Name アニメーションシーケンス名
//
//! @retval アニメーションシーケンス
//
//----------------------------------------------------------------------//
FORCEINLINE UAnimSequence* USNAnimInstanceBase::GetAnimationSequence(const FName& Name){
	return Cast<UAnimSequence>(GetAnimationAsset(Name));
}

//----------------------------------------------------------------------//
//
//! @brief アニメーションモンタージュを取得
//
//! @param Name アニメーションモンタージュ名
//
//! @retval アニメーションモンタージュ
//
//----------------------------------------------------------------------//
FORCEINLINE UAnimMontage* USNAnimInstanceBase::GetAnimationMontage(const FName& Name){
	return Cast<UAnimMontage>(GetAnimationAsset(Name));
}
//! @}
//! @}
