// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/Component/TraversalActionComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameAnimationSample/Environment/GALevelBlock.h"
#include "GameAnimationSample/Environment/GATraversalBlock.h"

// Sets default values for this component's properties
UTraversalActionComponent::UTraversalActionComponent()
:bDoingTraversalAction(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTraversalActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTraversalActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//----------------------------------------------------------------------//
//
//! @brief トラバーサルアクションを実行
//
//! @param TraceForwardDistance		トレースする前方への距離
//! @param TraversalCheckFailed		Output,Traversalのチェックが失敗か
//! @param MontageSelectionFailed	Output,Montageの選択に失敗したか
//
//----------------------------------------------------------------------//
void UTraversalActionComponent::ExecTraversalAction(float TraceForwardDistance, bool& TraversalCheckFailed, bool& MontageSelectionFailed)
{
	//! Debug表示の設定を取得
	int DrawDebugLevel = UKismetSystemLibrary::GetConsoleVariableIntValue(TEXT("DDCvar.Traversal.DrawDebugLevel"));
	
	int DrawDebugDuration = UKismetSystemLibrary::GetConsoleVariableIntValue(TEXT("DDCvar.Traversal.DrawDebugDuration"));

	FTraversalCheckResult TraversalCheckResult;

	

}

//----------------------------------------------------------------------//
//
//! @brief 進行方向にBlockがあるかチェック
//
//! @retval true	進行方向にBlockあり
//! @retval false	進行方向にBlockなし
//
//! @param
//
//----------------------------------------------------------------------//
bool UTraversalActionComponent::PerformFowardBlocks(FTraversalCheckResult& TraversalCheckResult, float TraceForwardDistance, int DrawDebugLevel, float DrawDebugDuration)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		return false;
	}
	
	FVector ActorLocation = Character->GetActorLocation();
	
	FVector ActorForwardVector = Character->GetActorForwardVector();

#if 0
	FVector EndPoint = ActorLocation + ActorForwardVector * TraceForwardDistance;
#else
	FVector Velocity = Character->GetMovementComponent()->Velocity;

	FVector Remap(
				UKismetMathLibrary::MapRangeClamped(Velocity.X, 0.0f, 500.0f, 75.0f, 350.0f),
				UKismetMathLibrary::MapRangeClamped(Velocity.Y, 0.0f, 500.0f, 75.0f, 350.0f),
				UKismetMathLibrary::MapRangeClamped(Velocity.Z, 0.0f, 500.0f, 75.0f, 350.0f)
			);

	FVector EndPoint = ActorLocation + Velocity;
#endif
	EDrawDebugTrace::Type DebugTrace = (DrawDebugLevel >= 2) ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	//! 進行方向にBlockがあるかチェック
	FHitResult HitResult;
	UKismetSystemLibrary::CapsuleTraceSingle(
				GetWorld(), ActorLocation, EndPoint,
				30.0f, 60.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
				TArray<AActor*>(), DebugTrace, HitResult, true,
				FLinearColor::Black, FLinearColor::Yellow
			);

	bool Result = HitResult.bBlockingHit;

	if (Result == true)
	{
		AGALevelBlock* LevelBlock = Cast<AGALevelBlock>(HitResult.GetActor());
		
		if (LevelBlock != nullptr)
		{
			TraversalCheckResult.HitComponent = HitResult.GetComponent();

			LevelBlock->GetLedgeTransform(HitResult.ImpactPoint, ActorLocation, TraversalCheckResult);

			DrawDebugShapesAtLedgeLocation(TraversalCheckResult, DrawDebugLevel, DrawDebugDuration);
		}
		else
		{
			AGATraversalBlock* TraversalBlock = Cast<AGATraversalBlock>(HitResult.GetActor());
			if (TraversalBlock != nullptr)
			{
				TraversalCheckResult.HitComponent = HitResult.GetComponent();

				TraversalBlock->GetLedgeTransform(HitResult.ImpactPoint, ActorLocation, TraversalCheckResult);

				DrawDebugShapesAtLedgeLocation(TraversalCheckResult, DrawDebugLevel, DrawDebugDuration);
			}
			else
			{
				Result = false;
			}
		}
	}

	return Result;
}

void UTraversalActionComponent::DrawDebugShapesAtLedgeLocation(const FTraversalCheckResult& TraversalCheckResult, int DrawDebugLevel, float DrawDebugDuration)
{
	if (DrawDebugLevel >= 1)
	{
		if (TraversalCheckResult.HasFrontLedge == true)
		{
			DrawDebugSphere(GetWorld(), TraversalCheckResult.FrontLedgeLocation, 10.0f, 12, FColor::Green, false, DrawDebugDuration);
			DrawDebugSphere(GetWorld(), TraversalCheckResult.BackLedgeLocation, 10.0f, 12, FColor::Blue, false, DrawDebugDuration);
		}
	}

}

