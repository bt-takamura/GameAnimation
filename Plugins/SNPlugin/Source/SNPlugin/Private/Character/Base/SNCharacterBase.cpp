// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Base/SNCharacterBase.h"

#include "SNDef.h"
#include "Animation/SNAnimInstanceBase.h"
#include "Animation/Components/SNLocomotionComponent.h"
#include "Utility/SNUtility.h"

//----------------------------------------------------------------------//
//
//! @brief デフォルトコンストラクタ
//
//----------------------------------------------------------------------//
ASNCharacterBase::ASNCharacterBase(){
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// レプリケートをON
	SetReplicates(true);
	// 移動、回転などの動きをレプリケート
	SetReplicateMovement(true);
}

//----------------------------------------------------------------------//
//
//! @brief レプリケートするメンバを取得
//
//! @param OutLifetimeProps レプリケートするメンバ
//
//! @note レプリケートするメンバはここで登録しないとレプリケートされない
//
//----------------------------------------------------------------------//
void ASNCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
//	DOREPLIFETIME(ASNCharacterBase, UppderBodyCurrentState);
//	DOREPLIFETIME(ASNCharacterBase, UppderBodyPreStateName);
//	DOREPLIFETIME(ASNCharacterBase, LowerBodyCurrentState);
//	DOREPLIFETIME(ASNCharacterBase, LowerBodyPreStateName);
//	DOREPLIFETIME(ASNCharacterBase, FullBodyCurrentState);
//	DOREPLIFETIME(ASNCharacterBase, FullBodyPreStateName);
}

//----------------------------------------------------------------------//
//
//! @brief ゲームがスタートしたとき、もしくはアクターがスポーンされたときに呼ばれます。
//
//----------------------------------------------------------------------//
void ASNCharacterBase::BeginPlay(){
	
	Super::BeginPlay();
}

//----------------------------------------------------------------------//
//
//! @brief 毎フレーム呼ばれます。
//
//! @param DeltaTime 経過時間
//
//----------------------------------------------------------------------//
void ASNCharacterBase::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASNCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//----------------------------------------------------------------------//
//
//! @brief 現在のステートを設定
//
//! @param Name 新しく設定するステート
//! @param Type タイプ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::SetCurrentState(const FName& Name, ECharacterStateType Type){
	
	if(SNUtility::IsServer(GetWorld())){
		// 現在のステートを変更
		InternalSetCurrentState(Name, Type);
		// 各クライアントにマルチキャスト
		SetCurrentState_OnMulticast(Name, Type);
	} else {
		// サーバーへ通知
		SetCurrentState_OnServer(Name, Type);
	}
}

//----------------------------------------------------------------------//
//
//! @brief 現在のステートを設定(サーバーで実行)
//
//! @param Name 新しく設定するステート
//! @param Type タイプ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::SetCurrentState_OnServer_Implementation(const FName& Name, ECharacterStateType Type){
	SetCurrentState(Name, Type);
}

//----------------------------------------------------------------------//
//
//! @brief サーバーへ通知するパラメータの妥当性をチェック
//
//! @param Name 新しく設定するステート
//! @param Type タイプ
//
//! @retval true  バリデート成功
//! @retval false バリデート失敗
//
//----------------------------------------------------------------------//
bool ASNCharacterBase::SetCurrentState_OnServer_Validate(const FName& Name, ECharacterStateType Type){
	return Type >= (ECharacterStateType)0 && Type < ECharacterStateType::Num;
}


//----------------------------------------------------------------------//
//
//! @brief 現在のステートを設定(各クライアントで実行)
//
//! @param Name 新しく設定するステート
//! @param Type タイプ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::SetCurrentState_OnMulticast_Implementation(const FName& Name, ECharacterStateType Type){
	InternalSetCurrentState(Name, Type);
}

//----------------------------------------------------------------------//
//
//! @brief 各クライアントへ通知するパラメータの妥当性をチェック
//
//! @param Name 新しく設定するステート
//! @param Type タイプ
//
//! @retval true  バリデート成功
//! @retval false バリデート失敗
//
//----------------------------------------------------------------------//
bool ASNCharacterBase::SetCurrentState_OnMulticast_Validate(const FName& Name, ECharacterStateType Type){
	return Type >= (ECharacterStateType)0 && Type < ECharacterStateType::Num;
}

//----------------------------------------------------------------------//
//
//! @brief 現在のステートを設定
//
//! @param Name 新しく設定するステート
//! @param Type タイプ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::InternalSetCurrentState(const FName& Name, ECharacterStateType Type){
	
	TFunction<bool(FName&, FName&, const FName&)> Function = [this](FName& PreState, FName& CurrentState, const FName& Input) -> bool
	{
		bool bChanged = false;
		
		if(CurrentState != Input){
			// 現在のステートを保存
			PreState = CurrentState;
			// ステートを変更
			CurrentState = Input;
			// 変更フラグをON
			bChanged = true;
		}
		
		return bChanged;
	};
	// 全身
	if(Type == ECharacterStateType::Full){
		
		if(Function(FullBodyPreStateName, FullBodyCurrentState, Name) == true){
			SNPLUGIN_LOG(TEXT("Full State : %s"), *FullBodyCurrentState.ToString());
		}
	} else 
	// 上半身
	if(Type == ECharacterStateType::Upper){
		
		if(Function(UppderBodyPreStateName, UppderBodyCurrentState, Name) == true){
			SNPLUGIN_LOG(TEXT("Upper State : %s"), *UppderBodyCurrentState.ToString());
		}
	} else
	// 下半身
	if(Type == ECharacterStateType::Lower){
		
		if(Function(LowerBodyPreStateName, LowerBodyCurrentState, Name) == true){
			SNPLUGIN_LOG(TEXT("Lower State : %s"), *LowerBodyCurrentState.ToString());
		}
	}
}

//----------------------------------------------------------------------//
//
//! @brief ブレンドスペースのパラメータを設定
//
//! @param Key   パラメータ名
//! @param Param パラメータ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::SetBlendspaceParam(const FName& Key, const FVector& Param){
	
	if(SNUtility::IsServer(GetWorld())){
		// ブレンドスペースのパラメータを設定
		IntervalBlendspaceParam(Key, Param);
		// 各クライアントに通知
		SetBlendspaceParam_OnMulticast(Key, Param);
	} else {
		// サーバーに通知
		SetBlendspaceParam_OnServer(Key, Param);
	}
}

//----------------------------------------------------------------------//
//
//! @brief ブレンドスペースのパラメータを設定(サーバーで実行)
//
//! @param Key   パラメータ名
//! @param Param パラメータ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::SetBlendspaceParam_OnServer_Implementation(const FName& Key, const FVector& Param){
	SetBlendspaceParam(Key, Param);
}

//----------------------------------------------------------------------//
//
//! @brief ブレンドスペースのパラメータを設定(各クライアントで実行)
//
//! @param Key   パラメータ名
//! @param Param パラメータ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::SetBlendspaceParam_OnMulticast_Implementation(const FName& Key, const FVector& Param){
	IntervalBlendspaceParam(Key, Param);
}

//----------------------------------------------------------------------//
//
//! @brief ブレンドスペースのパラメータを設定
//
//! @param Key   パラメータ名
//! @param Param パラメータ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::IntervalBlendspaceParam(const FName& Key, const FVector& Param){
	
	USNAnimInstanceBase* AnimInstance(Cast<USNAnimInstanceBase>(GetAnimInstance()));
	
	if(AnimInstance != nullptr){
		AnimInstance->SetBlendspaceParam(Key, Param);
	}
}

//----------------------------------------------------------------------//
//
//! @brief 現在のステートを取得
//
//! @param Type タイプ
//
//! @retval 現在のステート
//
//----------------------------------------------------------------------//
FName ASNCharacterBase::GetCurrentState(ECharacterStateType Type) const {
	
	if(Type == ECharacterStateType::Upper){
		return UppderBodyCurrentState;
	} else
	if(Type == ECharacterStateType::Lower){
		return LowerBodyCurrentState;
	}
	
	return FullBodyCurrentState;
}

//----------------------------------------------------------------------//
//
//! @brief 1つ前のステートを取得
//
//! @param Type タイプ
//
//! @retval 1つ前のステート
//
//----------------------------------------------------------------------//
FName ASNCharacterBase::GetPrevState(ECharacterStateType Type) const {
	
	if(Type == ECharacterStateType::Upper){
		return UppderBodyPreStateName;
	} else
	if(Type == ECharacterStateType::Lower){
		return LowerBodyPreStateName;
	}
	
	return FullBodyPreStateName;
}

//----------------------------------------------------------------------//
//
//! @brief 現在のステートかチェック
//
//! @param State ステート
//! @param Type  タイプ
//
//! @retval true  現在のステート
//! @retval false 現在のステートではない
//
//----------------------------------------------------------------------//
bool ASNCharacterBase::IsCurrentState(const FName& State, ECharacterStateType Type) const {
	
	if(Type == ECharacterStateType::Upper){
		return (UppderBodyCurrentState == State) ? true : false;
	} else
	if(Type == ECharacterStateType::Lower){
		return (LowerBodyCurrentState == State) ? true : false;
	}
	
	return (FullBodyCurrentState == State) ? true : false;
}

//----------------------------------------------------------------------//
//
//! @brief 1つ前のステートかチェック
//
//! @param State ステート
//! @param Type  タイプ
//
//! @retval true  1つ前ののステート
//! @retval false 1つ前のステートではない
//
//----------------------------------------------------------------------//
bool ASNCharacterBase::IsPreState(const FName& State, ECharacterStateType Type) const {
	
	if(Type == ECharacterStateType::Upper){
		return (UppderBodyPreStateName == State) ? true : false;
	} else
	if(Type == ECharacterStateType::Lower){
		return (LowerBodyPreStateName == State) ? true : false;
	}
	
	return (FullBodyPreStateName == State) ? true : false;
}

//----------------------------------------------------------------------//
//
//! @brief アニメーションインスタンスを取得
//
//! @retval アニメーションインスタンスへのポインタ
//
//----------------------------------------------------------------------//
UAnimInstance* ASNCharacterBase::GetAnimInstance(){
	
	UAnimInstance* Result(nullptr);
	
	USkeletalMeshComponent* SkeletalMeshComponent(GetMesh());
	
	if(SkeletalMeshComponent != nullptr){
		Result = SkeletalMeshComponent->GetAnimInstance();
	}
	
	return Result;
}

//----------------------------------------------------------------------//
//
//! @brief ロコモーションコンポーネントを取得
//
//! @retval ロコモーションコンポーネントへのポインタ
//
//----------------------------------------------------------------------//
USNLocomotionComponent* ASNCharacterBase::GetLocomotionComponent(){
	return Cast<USNLocomotionComponent>(GetComponentByClass(USNLocomotionComponent::StaticClass()));
}

//----------------------------------------------------------------------//
//
//! @brief アニメーションシーケンスを再生
//
//! @param Name      アニメーションのキー(DTに登録されている)の名前
//! @param Slot      再生するスロット名
//! @param PlayRate  再生レート
//! @param BlendIn   ブレンドインの補間時間
//! @param BlendOut  ブレンドアウトの補間時間
//! @param StartTime 開始フレーム
//! @param bLoop     ループ再生をするかどうかのフラグ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::PlaySequence(const FName& Name, const FName& Slot, float PlayRate, float BlendIn, float BlendOut, float StartTime, bool bLoop){
	// 自分がサーバーかチェック
	if(SNUtility::IsServer(GetWorld())){
		// アニメーションシーケンスを再生
		InternalPlaySequence(Name, Slot, PlayRate, BlendIn, BlendOut, StartTime, bLoop);
		// 各クライアントに通知
		PlaySequence_OnMulticast(Name, Slot, PlayRate, BlendIn, BlendOut, StartTime, bLoop);
	} else {
		// サーバーに通知
		PlaySequence_OnServer(Name, Slot, PlayRate, BlendIn, BlendOut, StartTime, bLoop);
	}
}

//----------------------------------------------------------------------//
//
//! @brief アニメーションシーケンスを再生(サーバーで実行)
//
//! @param Name      アニメーションのキー(DTに登録されている)の名前
//! @param Slot      再生するスロット名
//! @param PlayRate  再生レート
//! @param BlendIn   ブレンドインの補間時間
//! @param BlendOut  ブレンドアウトの補間時間
//! @param StartTime 開始フレーム
//! @param bLoop     ループ再生をするかどうかのフラグ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::PlaySequence_OnServer_Implementation(const FName& Name, const FName& Slot, float PlayRate, float BlendIn, float BlendOut, float StartTime, bool bLoop){
	PlaySequence(Name, Slot, PlayRate, BlendIn, BlendOut, StartTime, bLoop);
}

//----------------------------------------------------------------------//
//
//! @brief アニメーションシーケンスを再生(各クライアントで実行)
//
//! @param Name      アニメーションのキー(DTに登録されている)の名前
//! @param Slot      再生するスロット名
//! @param PlayRate  再生レート
//! @param BlendIn   ブレンドインの補間時間
//! @param BlendOut  ブレンドアウトの補間時間
//! @param StartTime 開始フレーム
//! @param bLoop     ループ再生をするかどうかのフラグ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::PlaySequence_OnMulticast_Implementation(const FName& Name, const FName& Slot, float PlayRate, float BlendIn, float BlendOut, float StartTime, bool bLoop){
	// アニメーションシーケンスを再生
	InternalPlaySequence(Name, Slot, PlayRate, BlendIn, BlendOut, StartTime, bLoop);
}

//----------------------------------------------------------------------//
//
//! @brief アニメーションシーケンスを再生
//
//! @param Name      アニメーションのキー(DTに登録されている)の名前
//! @param Slot      再生するスロット名
//! @param PlayRate  再生レート
//! @param BlendIn   ブレンドインの補間時間
//! @param BlendOut  ブレンドアウトの補間時間
//! @param StartTime 開始フレーム
//! @param bLoop     ループ再生をするかどうかのフラグ
//
//----------------------------------------------------------------------//
void ASNCharacterBase::InternalPlaySequence(const FName& Name, const FName& Slot, float PlayRate, float BlendIn, float BlendOut, float StartTime, bool bLoop){
	
	USNAnimInstanceBase* AnimInstance(Cast<USNAnimInstanceBase>(GetAnimInstance()));
	
	if(AnimInstance != nullptr){
		AnimInstance->PlayAnimationSequence(Name, Slot, PlayRate, BlendIn, BlendOut, StartTime, bLoop);
	}
}

//----------------------------------------------------------------------//
//
//! @brief アニメーションモンタージュを再生
//
//! @param Name      アニメーションのキー(DTに登録されている)の名前
//! @param PlayRate  再生レート
//! @param StartTime 開始フレーム
//
//----------------------------------------------------------------------//
void ASNCharacterBase::PlayMontage(const FName& Name, float PlayRate, float StartTime){
	// 自分がサーバーかチェック
	if(SNUtility::IsServer(GetWorld())){
		// モンタージュを再生
		InternalPlayMontage(Name, PlayRate, StartTime);
		// 各クライアントに通知
		PlayMontage_OnMulticast(Name, PlayRate, StartTime);
	} else {
		// サーバーに通知
		PlayMontage_OnServer(Name, PlayRate, StartTime);
	}
}

//----------------------------------------------------------------------//
//
//! @brief アニメーションモンタージュを再生(サーバーで実行)
//
//! @param Name      アニメーションのキー(DTに登録されている)の名前
//! @param PlayRate  再生レート
//! @param StartTime 開始フレーム
//
//----------------------------------------------------------------------//
void ASNCharacterBase::PlayMontage_OnServer_Implementation(const FName& Name, float PlayRate, float StartTime){
	PlayMontage(Name, PlayRate, StartTime);	
}

//----------------------------------------------------------------------//
//
//! @brief アニメーションモンタージュを再生(各クライアントで実行)
//
//! @param Name      アニメーションのキー(DTに登録されている)の名前
//! @param PlayRate  再生レート
//! @param StartTime 開始フレーム
//
//----------------------------------------------------------------------//
void ASNCharacterBase::PlayMontage_OnMulticast_Implementation(const FName& Name, float PlayRate, float StartTime){
	InternalPlayMontage(Name, PlayRate, StartTime);
}

//----------------------------------------------------------------------//
//
//! @brief アニメーションモンタージュを再生
//
//! @param Name      アニメーションのキー(DTに登録されている)の名前
//! @param PlayRate  再生レート
//! @param StartTime 開始フレーム
//
//----------------------------------------------------------------------//
void ASNCharacterBase::InternalPlayMontage(const FName& Name, float PlayRate, float StartTime){
	
	USNAnimInstanceBase* AnimInstance(Cast<USNAnimInstanceBase>(GetAnimInstance()));
	
	if(AnimInstance != nullptr){
		AnimInstance->PlayAnimationMontage(Name, PlayRate, StartTime);
	}
}

//----------------------------------------------------------------------//
//
//! @brief モンタージュのセクションへのジャンプ
//
//! @param Name    アニメーションのキー(DTに登録されている)の名前
//! @param Section セクション名
//
//----------------------------------------------------------------------//
void ASNCharacterBase::JumpMontageSection(const FName& Name, const FName& Section){
	// 自分がサーバーかチェック
	if(SNUtility::IsServer(GetWorld())){
		// 指定セクションへジャンプ
		InternalJumpMontageSection(Name, Section);
		// 各クライアントに通知
		JumpMontageSection_OnMulticast(Name, Section);
	} else {
		// サーバーへ通知
		JumpMontageSection_OnServer(Name, Section);
	}
}

//----------------------------------------------------------------------//
//
//! @brief モンタージュのセクションへのジャンプ(サーバー側で実行)
//
//! @param Name    アニメーションのキー(DTに登録されている)の名前
//! @param Section セクション名
//
//----------------------------------------------------------------------//
void ASNCharacterBase::JumpMontageSection_OnServer_Implementation(const FName& Name, const FName& Section){
	JumpMontageSection(Name, Section);
}

//----------------------------------------------------------------------//
//
//! @brief モンタージュのセクションへのジャンプ(各クライアント側で実行)
//
//! @param Name    アニメーションのキー(DTに登録されている)の名前
//! @param Section セクション名
//
//----------------------------------------------------------------------//
void ASNCharacterBase::JumpMontageSection_OnMulticast_Implementation(const FName& Name, const FName& Section){
	InternalJumpMontageSection(Name, Section);
}

//----------------------------------------------------------------------//
//
//! @brief モンタージュのセクションへのジャンプ
//
//! @param Name    アニメーションのキー(DTに登録されている)の名前
//! @param Section セクション名
//
//----------------------------------------------------------------------//
void ASNCharacterBase::InternalJumpMontageSection(const FName& Name, const FName& Section){
	
	USNAnimInstanceBase* AnimInstance(Cast<USNAnimInstanceBase>(GetAnimInstance()));
	
	if(AnimInstance != nullptr){
		AnimInstance->JumpAnimationMontageSection(Name, Section);
	}
}
