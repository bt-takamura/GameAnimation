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
#include "KismetAnimationLibrary.h"
#include "Character/SNPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "PoseSearch/PoseSearchLibrary.h"
#include "Chooser/Public/ChooserFunctionLibrary.h"
#include "GameAnimationSample/Environment/GALevelBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/SNUtility.h"

AGAPlayer::AGAPlayer(const FObjectInitializer& Initializer):
Super(Initializer)
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));

	SpringArmComponent->SetupAttachment(GetRootComponent());
	
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraCompoennt"));

	CameraComponent->SetupAttachment(SpringArmComponent);
}

void AGAPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UpdateCamera(false);
}


void AGAPlayer::BeginPlay()	
{
	Super::BeginPlay();

	ASNPlayerController* PlayerController(SNUtility::GetPlayerController<ASNPlayerController>());

	if(PlayerController != nullptr)
	{
		PlayerController->EnabledInputType(FName(TEXT("Normal")));
	}
}

void AGAPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateMovement();

	UpdateRotation();

	UpdateCamera(true);
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

bool AGAPlayer::PerformFowardBlocks(FTraversalCheckResult& TraversalCheckResult, float TraceForwardDistance, int DrawDebugLegel, float DrawDebugDuration)
{
	FVector ActorLocation(GetActorLocation());

	FVector ActorForwardVector(GetActorForwardVector());

	FVector EndPoint(ActorLocation + ActorForwardVector * TraceForwardDistance);

	EDrawDebugTrace::Type DebugTrace = (DrawDebugLegel >= 2) ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	FHitResult HitResult;

	UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), ActorLocation, EndPoint, 30.0f, 60.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<AActor*>(), DebugTrace, HitResult, true);

	bool Result = HitResult.bBlockingHit;
	
	if(Result == true)
	{
		AGALevelBlock* LevelBlock = Cast<AGALevelBlock>(HitResult.GetActor());

		if(LevelBlock != nullptr)
		{
			TraversalCheckResult.HitComponent = HitResult.GetComponent();

			LevelBlock->GetLedgeTransform(HitResult.ImpactPoint, ActorLocation, TraversalCheckResult);

			DrawDebugShapesAtLedgeLocation(TraversalCheckResult, DrawDebugLegel, DrawDebugDuration);
		} else
		{
			Result = false;
		}
	}

	return Result;
}

void AGAPlayer::DrawDebugShapesAtLedgeLocation(const FTraversalCheckResult& TraversalCheckResult, int DrawDebugLevel, float DrawDebugDuration)
{
	if(DrawDebugLevel >= 1)
	{
		if(TraversalCheckResult.HasFrontLedge == true)
		{
			DrawDebugSphere(GetWorld(), TraversalCheckResult.FrontLedgeLocation, 10.0f, 12, FColor::Green, false, DrawDebugDuration);

			DrawDebugSphere(GetWorld(), TraversalCheckResult.BackLedgeLocation, 10.0f, 12, FColor::Blue, false, DrawDebugDuration);
		}
	}
}


bool AGAPlayer::PerformDecisionOnActorToEachEdge(FTraversalCheckResult& TraversalCheckResult, int DrawDebugLegel)
{
	if(TraversalCheckResult.HasFrontLedge == false)
	{
		return false;
	}

	FVector FrontLedgeLocation(TraversalCheckResult.FrontLedgeLocation);
	// コリジョンの半径を取得
	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	// コリジョンの高さの半分を取得
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	FVector FrontLedgeNormalOffset(TraversalCheckResult.FrontLedgeNormal * (CapsuleRadius + 2.0f));

	float HeightOffset = CapsuleHalfHeight + 2.0f;
	// 障害物との空間チェック用の位置情報を算出
	FVector HasRoomCheckFromLedgeLocation(FrontLedgeLocation + FrontLedgeNormalOffset + FVector(0.0f, 0.0f, HeightOffset));

	bool Result = PerformActorToFrontEdge(TraversalCheckResult, HasRoomCheckFromLedgeLocation, DrawDebugLegel);

	if(Result == false)
	{
		return false;
	}

	// BlockActorのスプラインから得た障害物の反対側のロケーション情報を取得
	FVector BackLedgeLocation(TraversalCheckResult.BackLedgeLocation);
	// 法線方向にコリジョンの半径分オフセットさせる
	// @@ ここ何で+2.0何だろう…？障害物のその先に移動可能な場所があるかのチェックなら
	//    Radius*2.0fでコリジョンの直径分の隙間があるかチェックしないといけないんでは？
	FVector BackLedgeDepthOffset(TraversalCheckResult.BackLedgeNormal * (CapsuleRadius + 2.0f));
	
	FVector HasRoomCheckBackLedgeLocation(BackLedgeLocation + BackLedgeDepthOffset + FVector(0.0f, 0.0f , HeightOffset));
	
	PerformObstacleDepth(TraversalCheckResult, HasRoomCheckFromLedgeLocation, HasRoomCheckBackLedgeLocation, DrawDebugLegel);

	PerformBackLedgeFloor(TraversalCheckResult, HasRoomCheckBackLedgeLocation, DrawDebugLegel);

	return true;
}


//ステップ3.2: アクタの位置からフロントレッジの位置までトレースを行い、アクタが移動できるスペースがあるかどうかを判断します。移動できるスペースがある場合はファンクションを続行します。そうでない場合は早めに終了します。
bool AGAPlayer::PerformActorToFrontEdge(FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckFromLedgeLocation, int DrawDebugLegel)
{
	// コリジョンの半径を取得
	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	// コリジョンの高さの半分を取得
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	EDrawDebugTrace::Type DebugTrace = (DrawDebugLegel >= 3) ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	
	FHitResult HitResult;

	FVector ActorLocation(GetActorLocation());
	
	UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), ActorLocation, HasRoomCheckFromLedgeLocation, CapsuleRadius, CapsuleHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<AActor*>(), DebugTrace, HitResult, true);

	bool Result = ((HitResult.bBlockingHit == false) && (HitResult.bStartPenetrating == false)) ? true : false;
	
	if(Result == true)
	{
		TraversalCheckResult.ObstacleHeight = FMath::Abs(((ActorLocation - FVector(0.0f, 0.0f, CapsuleHalfHeight)) - TraversalCheckResult.FrontLedgeLocation).Z);
	} else
	{
		TraversalCheckResult.HasFrontLedge = false;
	}

	return Result;
}

// ステップ3.4： 障害物の上を手前の棚から奥の棚までトレースし、アクターが移動できるスペースがあるか確認します。
void AGAPlayer::PerformObstacleDepth(FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckFrontLedgeLocation, const FVector& HasRoomCheckBackLedgeLocation, int DrawDebugLegel)
{
	// コリジョンの半径を取得
	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	// コリジョンの高さの半分を取得
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	FHitResult HitResult;
	
	EDrawDebugTrace::Type DebugTrace = (DrawDebugLegel >= 3) ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	// 障害物の後ろに物があるかチェック
	bool Result = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), HasRoomCheckFrontLedgeLocation, HasRoomCheckBackLedgeLocation, CapsuleRadius, CapsuleHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<AActor*>(), DebugTrace, HitResult, true);
	
	if(Result != true){
		// ステップ3.5：もし余裕があれば、前後のレッジの位置の差を利用して障害物の深さを保存する。
		// 物がない場合、ObstacleDepthに障害物の奥行(XY平面)を設定
		TraversalCheckResult.ObstacleDepth = (TraversalCheckResult.FrontLedgeLocation - TraversalCheckResult.BackLedgeLocation).Size2D();
	} else {
		// ステップ3.5：もし余裕がなければ、前方のレッジとトレースインパクトポイントの差を使って障害物の深さを保存し、後方のレッジを無効にする。
		// 奥に別の障害物がある場合はHasBackLedgeをfalseに設定
		TraversalCheckResult.HasBackLedge = false;
		// ObstacleDepthには奥にある障害物との距離(XY平面を設定
		TraversalCheckResult.ObstacleDepth =  (HitResult.ImpactPoint - TraversalCheckResult.FrontLedgeLocation).Size2D();
	}
}

//ステップ3.6 奥の棚の位置から下に向かってたどり、床を見つける（距離には障害物の高さを使う）。
//もし床があれば、その位置と後方の棚の高さ(後方の棚から床までの距離を使用)を保存する。床が見つからなければ、後方の床を無効にする。
void AGAPlayer::PerformBackLedgeFloor(FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckBackLedgeLocation, int DrawDebugLegel)
{
	FVector BackLedgeLocation(TraversalCheckResult.BackLedgeLocation);

	FVector BackLedgeNormal(TraversalCheckResult.BackLedgeNormal);

	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();

	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	float HeightOffset = TraversalCheckResult.ObstacleHeight - CapsuleHalfHeight + 50.0f;

	FVector EndPoint((BackLedgeLocation + BackLedgeNormal * (CapsuleRadius + 2.0f)) - FVector(0.0f, 0.0f, HeightOffset));

	FHitResult HitResult;

	EDrawDebugTrace::Type DebugTrace = (DrawDebugLegel >= 3) ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	
	UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), HasRoomCheckBackLedgeLocation, EndPoint, CapsuleRadius, CapsuleHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<AActor*>(), DebugTrace, HitResult, true);

	if(HitResult.bBlockingHit == true)
	{
		TraversalCheckResult.HasBackFloor = true;

		TraversalCheckResult.BackFloorLocation = HitResult.ImpactPoint;

		TraversalCheckResult.BackLedgeHeight = FMath::Abs((HitResult.ImpactPoint - TraversalCheckResult.BackLedgeLocation).Z);
	} else
	{
		TraversalCheckResult.HasBackFloor = false;
	}
}

void AGAPlayer::DetermineTraversalAction(FTraversalCheckResult& TraversalCheckResult)
{
	if((TraversalCheckResult.HasFrontLedge == true)
	&& (TraversalCheckResult.HasBackLedge == true)
	&& (TraversalCheckResult.HasBackFloor != true)
	&& (UKismetMathLibrary::InRange_FloatFloat(TraversalCheckResult.ObstacleHeight, 50.0f, 125.0f))
	&& (TraversalCheckResult.ObstacleDepth < 59.0f)
	)
	{
		TraversalCheckResult.ActionType = ETraversalActionType::Vault;		
	} else
	if((TraversalCheckResult.HasFrontLedge == true)
	&& (TraversalCheckResult.HasBackLedge == true)
	&& (TraversalCheckResult.HasBackFloor == true)
	&& (UKismetMathLibrary::InRange_FloatFloat(TraversalCheckResult.ObstacleHeight, 50.0f, 125.0f))
	&& (TraversalCheckResult.ObstacleDepth < 59.0f)
	&& (TraversalCheckResult.BackLedgeHeight > 50.0f)
	)
	{
		TraversalCheckResult.ActionType = ETraversalActionType::Hurdle;
	} else
	if((TraversalCheckResult.HasFrontLedge == true)
	&& (UKismetMathLibrary::InRange_FloatFloat(TraversalCheckResult.ObstacleHeight, 50.0f, 275.0f))
	&& (TraversalCheckResult.ObstacleDepth >= 59.0f)
	)
	{
		TraversalCheckResult.ActionType = ETraversalActionType::Mantle;
	} else {
		TraversalCheckResult.ActionType = ETraversalActionType::None;
	}
}

void AGAPlayer::DebugPrintTraversalResult(int DrawDebugLevel, FTraversalCheckResult TraversalCheckResult)
{
	if(DrawDebugLevel >= 1)
	{
		FString Str(TEXT("Has Front Ledget : "));
		
		Str += (TraversalCheckResult.HasFrontLedge == true) ? TEXT("True") : TEXT("False");
		
		Str += "\n";
		
		Str += TEXT("Has Back Ledge : ");
		
		Str += (TraversalCheckResult.HasBackLedge == true) ? TEXT("True") : TEXT("False");
		
		Str += "\n";
		
		Str += TEXT("Has Back Floor : ");
		
		Str += (TraversalCheckResult.HasBackFloor == true) ? TEXT("True") : TEXT("False");
		
		Str += "\n";
		
		Str += TEXT("Obstacle Height :");
		
		Str += FString::SanitizeFloat(TraversalCheckResult.ObstacleHeight);
		
		Str += "\n";
		
		Str += TEXT("Obstacle Depth : ");
		
		Str += FString::SanitizeFloat(TraversalCheckResult.ObstacleDepth);
		
		Str += "\n";
		
		Str += TEXT("Back Ledge Height: ");
		
		Str += FString::SanitizeFloat(TraversalCheckResult.BackLedgeHeight);
		
		UKismetSystemLibrary::PrintString(GetWorld(), Str, true, false);
		
		FString CurrentAction(StaticEnum<ETraversalActionType>()->GetValueAsString(TraversalCheckResult.ActionType));
		
		UKismetSystemLibrary::PrintString(GetWorld(), CurrentAction, true, false);
	}
}

//----------------------------------------------------------------------//
//
//! @brief トラバーサル・チェックの条件に一致するすべてのモンタージュを選択するために、チューザーを評価する。
//
//! @param TraversalCheckResult 
//
//! @retval 
//
//----------------------------------------------------------------------//
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

void AGAPlayer::UpdateMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = CalculateMaxSpeed();

	GetCharacterMovement()->MaxWalkSpeedCrouched = GetCharacterMovement()->MaxWalkSpeed;
}

void AGAPlayer::UpdateRotation()
{
	if(bWantsToStrafe == true)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;

		GetCharacterMovement()->bOrientRotationToMovement = false;
	} else
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;

		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	if(GetCharacterMovement()->IsFalling() == true)
	{
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 200.0f, 0.0f);
	} else
	{
		GetCharacterMovement()->RotationRate = FRotator(0.0f, -1.0f, 0.0f);
	}
}

void AGAPlayer::UpdateCamera(bool bInterpolate)
{
	FSimpleCameraParams* CameraParams = nullptr;

	if(bWantsToAim == true)
	{
		CameraParams = &CamStyleAim;
	} else
	{
		CameraParams = &CamStyleClose;
	}

	if(bWantsToStrafe == false)
	{
		CameraParams =&CamStyleFar;
	}

	FSimpleCameraParams TargetCameraParams;

	TargetCameraParams.SpringArmLength = CameraDistanceMag * CameraParams->SpringArmLength;
	TargetCameraParams.SocketOffset = CameraDistanceMag * CameraParams->SocketOffset;
	TargetCameraParams.FieldOfView = CameraParams->FieldOfView;
	TargetCameraParams.TranslationLagSpeed = (bInterpolate == true) ? CameraParams->TranslationLagSpeed : -1.0f;
	TargetCameraParams.TransitionSpeed = (bInterpolate == true) ? CameraParams->TransitionSpeed : -1.0f;

	GetCameraComponent()->FieldOfView = UKismetMathLibrary::FInterpTo(GetCameraComponent()->FieldOfView, TargetCameraParams.FieldOfView, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), TargetCameraParams.TransitionSpeed);

	GetSpringArmComponent()->TargetArmLength = UKismetMathLibrary::FInterpTo(GetSpringArmComponent()->TargetArmLength, TargetCameraParams.SpringArmLength, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), TargetCameraParams.TransitionSpeed);

	GetSpringArmComponent()->CameraLagSpeed = UKismetMathLibrary::FInterpTo(GetSpringArmComponent()->CameraLagSpeed, TargetCameraParams.TranslationLagSpeed, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), TargetCameraParams.TransitionSpeed);

	GetSpringArmComponent()->SocketOffset = UKismetMathLibrary::VInterpTo(GetSpringArmComponent()->SocketOffset, TargetCameraParams.SocketOffset, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), TargetCameraParams.TransitionSpeed);
}


float AGAPlayer::CalculateMaxSpeed()
{
	FVector Velocity(GetCharacterMovement()->Velocity);

	float Direction = FMath::Abs(UKismetAnimationLibrary::CalculateDirection(Velocity, GetActorRotation()));
	
	float StafeSpeedMap = StrafeSpeedMapCurveObject->GetFloatValue(Direction);

	FVector Speed(FVector::ZeroVector);

	if(GetCharacterMovement()->IsCrouching() == true)
	{
		Speed = CrouchSpeed;
	} else
	{
		switch(Stride)
		{
		case EStride::Walk: Speed = WalkSpeed; break;
		case EStride::Run: Speed = RunSpeed; break;
		case EStride::Sprint: Speed = SprintSpeed; break;
		default: break;
		}
	}

	float Result = 0.0f;
	
	if(StafeSpeedMap < 1.0f)
	{
		Result = UKismetMathLibrary::MapRangeClamped(StafeSpeedMap, 0.0f, 1.0f, Speed.X, Speed.Y);	
	} else
	{
		Result = UKismetMathLibrary::MapRangeClamped(StafeSpeedMap, 1.0f, 2.0f, Speed.Y, Speed.Z);
	}

	return Result;
}



EStride AGAPlayer::GetDesiredStride() const
{
	return EStride::Run;
}


float AGAPlayer::GetTraversalForwardTraceDistance() const
{
	FVector Velocity(GetMovementComponent()->Velocity);

	FVector Vector(UKismetMathLibrary::Quat_UnrotateVector(GetActorRotation().Quaternion(), Velocity));

	float Result = UKismetMathLibrary::MapRangeClamped(Vector.X, 0.0f, 500.0f, 75.0f, 350.0f);

	return Result;
}

FVector2D AGAPlayer::GetMovementInputScaleValue(const FVector2D& Input) const
{
	FVector2D PostNormal(Input.GetSafeNormal());

	FVector2D Result(PostNormal);

	switch(MovementStickMode)
	{
	case EAnalogueMovementBehavior::VariableSpeed_SingleStride:
	case EAnalogueMovementBehavior::VariableSpeed_WalkRun: Result = Input; break;
	default: break;
	}

	return Result;
}





