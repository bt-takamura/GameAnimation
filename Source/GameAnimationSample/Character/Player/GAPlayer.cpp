// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/GAPlayer.h"

#include "PlayMontageCallbackProxy.h"
#include "SNDef.h"
#include "Animation/SNAnimInstanceBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MotionWarping/Public/MotionWarpingComponent.h"
#include "AnimationWarpingLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PoseSearch/PoseSearchLibrary.h"
#include "Chooser/Public/ChooserFunctionLibrary.h"

AGAPlayer::AGAPlayer(const FObjectInitializer& Initializer):
Super(Initializer)
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));

	SpringArmComponent->SetupAttachment(GetRootComponent());
	
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraCompoennt"));

	CameraComponent->SetupAttachment(SpringArmComponent);
}

void AGAPlayer::SetEnableTraversalAction(bool bEnable)
{
	bDoingTraversalAction = bEnable;
}

bool AGAPlayer::GetEnableTraversalAction() const
{
	return bDoingTraversalAction;
}

USpringArmComponent* AGAPlayer::GetSpringArmComponent()
{
	return SpringArmComponent;
}

UMotionWarpingComponent* AGAPlayer::GetMotionWarpingComponent()
{
	return MotionWarpingComponent;
}

UCameraComponent* AGAPlayer::GetCameraComponent()
{
	return CameraComponent;
}

void AGAPlayer::ExecTraversalAction(float TraceForwardDistance, bool& TraversalCheckFailed, bool& MontageSelectionFailed)
{
	
	FVector ActorLocation(GetActorLocation());

	FVector ActorForwardVector(GetActorForwardVector());

	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();

	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	int DrawDebugLevel = UKismetSystemLibrary::GetConsoleVariableIntValue(TEXT("DDCvar.Traversal.DrawDebugLevel"));

	int DrawDebugDuration = UKismetSystemLibrary::GetConsoleVariableIntValue(TEXT("DDCvar.Traversal.DrawDebugDuration"));

	FHitResult HitResult;
	
	UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(),
		ActorLocation,
		ActorLocation + ActorForwardVector * TraceForwardDistance,
		30.0f,
		60.0f,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		(DrawDebugLevel >= 2) ? EDrawDebugTrace::None : EDrawDebugTrace::ForDuration,
		HitResult,
		false,
		FLinearColor::Black,
		FLinearColor::Black,
		DrawDebugDuration
		);

	if(HitResult.bBlockingHit == false)
	{
		TraversalCheckFailed = true;

		MontageSelectionFailed = false;

		return;
	}
}
#if 1
TArray<UObject*> AGAPlayer::EvaluateChooser(FTraversalCheckResult TraversalCheckResult)
{
	FGATraversalChooserParams Params;
	// 現状のプレイヤーのアクションを設定
	Params.ActionType = TraversalCheckResult.ActionType;
	// 歩幅を設定
	Params.Stride = Stride;
	// 移動速度を取得
	FVector Velocity(GetCharacterMovement()->Velocity);
	// 移動速度(XY平面)を設定
	Params.Speed = Velocity.Size2D();

	Params.ObstacleHeight = TraversalCheckResult.ObstacleHeight;
	Params.ObstacleDepth = TraversalCheckResult.ObstacleDepth;
	// ここからChooserTableのEvaluate処理
	// 2024年6月時点で実験的機能なのでこれから結構変更ありそう...。
	FChooserEvaluationContext Context(UChooserFunctionLibrary::MakeChooserEvaluationContext());
	// ChooserTable評価用の構造体を設定(ChooserTable側に設定している構造体の型と同じもの)
	Context.AddStructParam(Params);
	// 評価するChooserを設定
	FInstancedStruct ChooserInstance(UChooserFunctionLibrary::MakeEvaluateChooser(TraversalAnimationChooser));
	// 評価を実行
	TArray<UObject*> Assets(UChooserFunctionLibrary::EvaluateObjectChooserBaseMulti(Context, ChooserInstance, UAnimMontage::StaticClass()));

	return Assets;
}
#endif
bool AGAPlayer::PerformMotionMatch(TArray<UObject*> SearchAssets, FTraversalCheckResult TraversalCheckResult)
{
	USNAnimInstanceBase* AnimInstanceBase(Cast<USNAnimInstanceBase>(GetAnimInstance()));

	if(AnimInstanceBase == nullptr)
	{
		SNPLUGIN_ERROR(TEXT("Animation Instance is nullptr."));

		return false;
	}

	FPoseSearchBlueprintResult Result;
	// 選択された全てのモンタージュから最良のモンタージュを検索。
	UPoseSearchLibrary::MotionMatch(AnimInstanceBase, SearchAssets, FName(TEXT("PoseHistory")), FPoseSearchFutureProperties(), Result, 42);

	const UAnimMontage* AnimMontage(Cast<UAnimMontage>(Result.SelectedAnimation));
	// ここでモンタージュがnullになる場合、データベースが無効かスキーマに問題があった可能性あり。
	if(AnimMontage == nullptr)
	{
		SNPLUGIN_ERROR(TEXT("Failed To Find Montage!"));

		return false;
	}
	// なんでBPではconstからconstなしに変換で来てるんだろう...？
	// ここのconstはずし...好きでやってるわけではないけど、他に方法が思いつかない。
	TraversalCheckResult.ChosenMontage = const_cast<UAnimMontage*>(AnimMontage);

	TraversalCheckResult.StartTime = Result.SelectedTime;

	TraversalCheckResult.PlayRate = Result.WantedPlayRate;
	// トラバーサルを実行
	ExecPerformTraversalAction(TraversalCheckResult);

	return true;
}


void AGAPlayer::ExecPerformTraversalAction(const FTraversalCheckResult& TraversalCheckResult)
{
	TraversalResult = TraversalCheckResult;
	// ワープターゲットを更新
	UpdateWarpTarget();
	// パルクール用のモンタージュを再生
	UPlayMontageCallbackProxy* Proxy = UPlayMontageCallbackProxy::CreateProxyObjectForPlayMontage(GetMesh(), TraversalResult.ChosenMontage, TraversalResult.PlayRate, TraversalResult.StartTime, NAME_None);
	
	if(Proxy != nullptr)
	{
		// CompleteとInterrupt(途中で他のモーションに差し込まれた)を登録し、コリジョンやムーブメントモード、フラグを戻す。
		// これが正常にされないとコリジョンを無視したままになる。
		Proxy->OnCompleted.AddDynamic(this, &AGAPlayer::EndPlayMontage);
		
		Proxy->OnInterrupted.AddDynamic(this, &AGAPlayer::EndPlayMontage);
		
		bDoingTraversalAction = true;
		
		GetCapsuleComponent()->IgnoreComponentWhenMoving(TraversalResult.HitComponent, true);
		// ムーブメントモードを設定
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		
		SNPLUGIN_LOG(TEXT("Montage is Playing & Delegate Registed."));
	} else
	{
		SNPLUGIN_ERROR(TEXT("Montage can't Play."));
	}
}

void AGAPlayer::EndPlayMontage(FName )
{
	SNPLUGIN_LOG(TEXT("End paly montage."));
	
	bDoingTraversalAction = false;

	GetCapsuleComponent()->IgnoreComponentWhenMoving(TraversalResult.HitComponent, false);

	EMovementMode Mode;

	switch(TraversalResult.ActionType)
	{
	case ETraversalActionType::None: Mode = MOVE_Walking;	break;
	case ETraversalActionType::Hurdle: Mode = MOVE_Walking; break;
	case ETraversalActionType::Vault: Mode = MOVE_Falling; break;
	case ETraversalActionType::Mantle: Mode = MOVE_Walking; break;
	default : Mode = MOVE_Falling;  break;
	}
	
	// ムーブメントモードを設定
	GetCharacterMovement()->SetMovementMode(Mode);
}

void AGAPlayer::UpdateWarpTarget()
{
	FVector FrontLedgeLocation(TraversalResult.FrontLedgeLocation);
	
	FVector FrontLedgeNormal(TraversalResult.FrontLedgeNormal);
	
	FRotator TargetRotation(UKismetMathLibrary::MakeRotFromX(UKismetMathLibrary::NegateVector(FrontLedgeNormal)));
	// 指定位置にワープを実行
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName(TEXT("FrontLedge")), FrontLedgeLocation, TargetRotation);
	
	float AnimatedDistanceFromFrontLedgeToBackLedge = 0.0f;
	
	if((TraversalResult.ActionType == ETraversalActionType::Hurdle)
	|| (TraversalResult.ActionType == ETraversalActionType::Vault)){
		
		TArray<FMotionWarpingWindowData> BackLedgeDataList;
		// @@ MotionWarpingWindowって...何？どこで設定されてるの？？
		UMotionWarpingUtilities::GetMotionWarpingWindowsForWarpTargetFromAnimation(TraversalResult.ChosenMontage, FName(TEXT("BackLedge")), BackLedgeDataList);
		
		if(BackLedgeDataList.IsEmpty() != true){
			
			const FMotionWarpingWindowData& Data(BackLedgeDataList[0]);
			// アニメーションデータからカーブ情報を取得
			// 「Distance_From_Ledge」はアニメーションシーケンス内にそういう名前でカーブが設定されている。
			UAnimationWarpingLibrary::GetCurveValueFromAnimation(TraversalResult.ChosenMontage, FName(TEXT("Distance_From_Ledge")), Data.EndTime, AnimatedDistanceFromFrontLedgeToBackLedge);
			// 指定位置にワープを実行
			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName(TEXT("BackLedge")), TraversalResult.BackLedgeLocation, FRotator::ZeroRotator);
		} else {
			// BackLedgeに関連付けているワープターゲットを削除
			MotionWarpingComponent->RemoveWarpTarget(FName(TEXT("BackLedge")));
		}
		
	} else {
		// BackLedgeに関連付けているワープターゲットを削除
		MotionWarpingComponent->RemoveWarpTarget(FName(TEXT("BackLedge")));
	}
	
	if(TraversalResult.ActionType == ETraversalActionType::Hurdle){
		
		TArray<FMotionWarpingWindowData> DataList;
		
		UMotionWarpingUtilities::GetMotionWarpingWindowsForWarpTargetFromAnimation(TraversalResult.ChosenMontage, FName(TEXT("BackFloor")), DataList);
		
		if(DataList.IsEmpty() != true){
			
			const FMotionWarpingWindowData& Data(DataList[0]);
			
			float AnimatedDistanceFromFrontLedgeToBackFloor = 0.0f;
			// アニメーションデータからカーブ情報を取得
			// 「Distance_From_Ledge」はアニメーションシーケンス内にそういう名前でカーブが設定されている。
			UAnimationWarpingLibrary::GetCurveValueFromAnimation(TraversalResult.ChosenMontage, FName(TEXT("Distance_From_Ledge")), Data.EndTime, AnimatedDistanceFromFrontLedgeToBackFloor);
			
			float Bias = FMath::Abs(AnimatedDistanceFromFrontLedgeToBackLedge - AnimatedDistanceFromFrontLedgeToBackFloor);
			
			FVector BackLedgeLocation(TraversalResult.BackLedgeLocation);
			
			FVector BackLedgeNormal(TraversalResult.BackLedgeNormal);
			
			FVector BackFloorLocation(TraversalResult.BackFloorLocation);
			
			FVector NormalOffset(BackLedgeNormal * Bias);
			
			FVector Tmp(BackLedgeLocation + NormalOffset);
			
			FVector TargetLocation(Tmp.X, Tmp.Y, BackFloorLocation.Z);
			// 指定位置にワープを実行
			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName(TEXT("BackFloor")), TargetLocation, FRotator::ZeroRotator);
		} else {
			// BackFloorに関連付けているワープターゲットを削除
			MotionWarpingComponent->RemoveWarpTarget(FName(TEXT("BackFloor")));
		}
		
	} else {
		// BackFloorに関連付けているワープターゲットを削除
		MotionWarpingComponent->RemoveWarpTarget(FName(TEXT("BackFloor")));
	}
}





