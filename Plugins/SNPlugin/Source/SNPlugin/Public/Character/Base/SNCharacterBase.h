// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/SNCharacterDef.h"

#include "SNCharacterBase.generated.h"

class USNLocomotionComponent;

//!@{@defgroup キャラクター制御
//!@{

// 上半身、下半身の制御をベースに入れるか悩みましたが…。基本機能なので入れました。
// 大したコストでもないし…。

//----------------------------------------------------------------------//
//
//! @brief キャラクター制御用ベースクラス
//
//----------------------------------------------------------------------//
UCLASS()
class SNPLUGIN_API ASNCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASNCharacterBase();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/** Returns properties that are replicated for the lifetime of the actor channel */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//! @{@name ステートの設定
	void SetCurrentState(const FName& Name, ECharacterStateType Type=ECharacterStateType::Full);
	//! @}
	
	//! @{@name 現在のステートを取得
	UFUNCTION(BlueprintCallable, Category="SN|Character")
	FName GetCurrentState(ECharacterStateType Type=ECharacterStateType::Full) const ;
	//! @}
	
	//! @{@name 1つ前のステートを取得
	UFUNCTION(BlueprintCallable, Category="SN|Character")
	FName GetPrevState(ECharacterStateType Type=ECharacterStateType::Full) const ;
	//! @}
	
	//! @{@name 現在のステートかチェック
	UFUNCTION(BlueprintCallable, Category = "SN|Character", meta=(BlueprintThreadSafe))
	bool IsCurrentState(const FName& State, ECharacterStateType Type=ECharacterStateType::Full) const;
	//! @}
	
	//! @{@name 1つ前のステートかチェック
	UFUNCTION(BlueprintCallable, Category = "SN|Character", meta=(BlueprintThreadSafe))
	bool IsPreState(const FName& State, ECharacterStateType Type=ECharacterStateType::Full) const;
	//! @}
	
	//! @{@name アニメーションインスタンスを取得
	UAnimInstance* GetAnimInstance();
	//! @}
	
	//! @{@name ロコモーションコンポーネントを取得
	USNLocomotionComponent* GetLocomotionComponent();
	//! @}
	
	//! @{@name アニメーションシーケンスを再生
	UFUNCTION(BlueprintCallable, Category="SN|Animation")
	void PlaySequence(const FName& Name, const FName& Slot, float PlayRate=1.0f, float BlendIn = 0.05f, float BlendOut = 0.05f, float StartTime=0.0f, bool bLoop=false);
	//! @}
	
	//! @{@name アニメーションモンタージュを再生
	UFUNCTION(BlueprintCallable, Category="SN|Animation")
	void PlayMontage(const FName& Name, float PlayRate=1.0f, float StartTime=0.0f);
	//! @}
	
	//! @{@name モンタージュのセクションへのジャンプ
	UFUNCTION(BlueprintCallable, Category="SN|Animation")
	void JumpMontageSection(const FName& Name, const FName& Section);
	//! @}
	
	//! @{@name ブレンドスペースのパラメータを設定
	UFUNCTION(BlueprintCallable, Category="SN|Animation")
	void SetBlendspaceParam(const FName& Key, const FVector& param);
	//! @}
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	
	//! @{@name ステートの変更
	UFUNCTION(Server, Reliable, WithValidation)
	void SetCurrentState_OnServer(const FName& Name, ECharacterStateType Type);
	
	UFUNCTION(NetMulticast,Reliable, WithValidation)
	void SetCurrentState_OnMulticast(const FName& Name, ECharacterStateType Type);
	
	void InternalSetCurrentState(const FName& Name, ECharacterStateType Type);
	//! @}
	
	//! @{@name ブレンドスペースのパラメータを設定
	UFUNCTION(Server, Reliable)
	void SetBlendspaceParam_OnServer(const FName& Key, const FVector& Param);
	
	UFUNCTION(NetMulticast, Reliable)
	void SetBlendspaceParam_OnMulticast(const FName& Key, const FVector& Param);
	
	void IntervalBlendspaceParam(const FName& Key, const FVector& Param);
	//! @}
	
	//! @{@name アニメーションシーケンスを再生
	UFUNCTION(Server, Reliable)
	void PlaySequence_OnServer(const FName& Name, const FName& Slot, float PlayRate, float BlendIn, float BlendOut, float StartTime, bool bLoop);
	
	UFUNCTION(NetMulticast, Reliable)
	void PlaySequence_OnMulticast(const FName& Name, const FName& Slot, float PlayRate, float BlendIn, float BlendOut, float StartTime, bool bLoop);
	
	void InternalPlaySequence(const FName& Name, const FName& Slot, float PlayRate, float BlendIn, float BlendOut, float StartTime, bool bLoop);
	//! @}
	
	//! @{@name アニメーションモンタージュを再生
	UFUNCTION(Server, Reliable)
	void PlayMontage_OnServer(const FName& Name, float PlayRate, float StartTime);
	
	UFUNCTION(NetMulticast, Reliable)
	void PlayMontage_OnMulticast(const FName& Name, float PlayRate, float StartTime);
	
	void InternalPlayMontage(const FName& Name, float PlayRate, float StartTime);
	//! @}
	
	//! @{@name モンタージュのセクションへのジャンプ
	UFUNCTION(Server, Reliable)
	void JumpMontageSection_OnServer(const FName& Name, const FName& Section);
	
	UFUNCTION(NetMulticast, Reliable)
	void JumpMontageSection_OnMulticast(const FName& Name, const FName& Section);
	
	void InternalJumpMontageSection(const FName& Name, const FName& Section);
	//! @}
	
	//!< 上半身の現在のステート
	UPROPERTY()
	FName UppderBodyCurrentState = NAME_None;
	
	//!< 上半身の1つ前のステート
	UPROPERTY()
	FName UppderBodyPreStateName = NAME_None;
	
	//!< 下半身の現在のステート
	UPROPERTY()
	FName LowerBodyCurrentState = NAME_None;
	
	//!< 下半身の1つ前のステート
	UPROPERTY()
	FName LowerBodyPreStateName = NAME_None;
	
	//!< 全身の現在のステート
	UPROPERTY()
	FName FullBodyCurrentState = NAME_None;
	
	//!< 全身の1つ前のステート
	UPROPERTY()
	FName FullBodyPreStateName = NAME_None;
};
//! @}
//! @}
