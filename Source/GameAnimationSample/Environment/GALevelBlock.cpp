// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Environment/GALevelBlock.h"

#include "Components/SplineComponent.h"
#include "GameAnimationSample/Character/Player/GAPlayer.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGALevelBlock::AGALevelBlock():Super()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Tickを止める
	//PrimaryActorTick.bStartWithTickEnabled = false;
	
	SetActorTickEnabled(false);
}

USplineComponent* AGALevelBlock::FindClosestLedgeToActor(const FVector& ActorLocation)
{
	if(Ledges.IsEmpty() == true)
	{
		return nullptr;
	}

	float ClosestDistance = 0.0f;
	
	int ClosestIndex = 0, Count = 0;
	
	for (USplineComponent* SplineComponent : Ledges)
	{
		FVector ClosestLocation(SplineComponent->FindLocationClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::World));

		FVector ClosestUpVector(SplineComponent->FindUpVectorClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::World));

		float Distance = FVector::Distance(ClosestLocation + ClosestUpVector * 10.0f, ActorLocation);

		if((Distance < ClosestDistance) || (Count == 0))
		{
			ClosestDistance = Distance;

			ClosestIndex = Count;
		}

		++Count;
	}

	return Ledges[ClosestIndex];
}

void AGALevelBlock::GetLedgeTransform(FVector HitLocation, FVector ActorLocation, FTraversalCheckResult& TraversalTraceResultInOut)
{
	USplineComponent* ClosestLedge(FindClosestLedgeToActor(ActorLocation));

	if(ClosestLedge == nullptr)
	{
		TraversalTraceResultInOut.HasFrontLedge = false;

		return;
	}
	
	if(ClosestLedge->GetSplineLength() >= MinLedgeWidth)
	{
		
	} else
	{
		TraversalTraceResultInOut.HasFrontLedge = false;

		return;
	}

	FVector ClosestLocation(ClosestLedge->FindLocationClosestToWorldLocation(HitLocation, ESplineCoordinateSpace::Local));

	float Distance = ClosestLedge->GetDistanceAlongSplineAtLocation(ClosestLocation, ESplineCoordinateSpace::Local);

	Distance = FMath::Clamp(Distance, MinLedgeWidth * 0.5f, ClosestLedge->GetSplineLength() - MinLedgeWidth*0.5f);

	FTransform FrontLedgeTransform(ClosestLedge->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World, false));

	TraversalTraceResultInOut.HasFrontLedge = true;

	TraversalTraceResultInOut.FrontLedgeLocation = FrontLedgeTransform.GetLocation();

	TraversalTraceResultInOut.FrontLedgeNormal = UKismetMathLibrary::GetUpVector(FrontLedgeTransform.Rotator());
	
	USplineComponent* OppositeLedge(OppositeLedges.Find(ClosestLedge) != nullptr ? OppositeLedges[ClosestLedge] : nullptr);

	if(OppositeLedge == nullptr)
	{
		TraversalTraceResultInOut.HasBackLedge = false;

		return;
	}
	
	FTransform BackLedgeTransform(OppositeLedge->FindTransformClosestToWorldLocation(TraversalTraceResultInOut.FrontLedgeLocation, ESplineCoordinateSpace::World, false));

	TraversalTraceResultInOut.HasBackLedge = true;

	TraversalTraceResultInOut.BackLedgeLocation = BackLedgeTransform.GetLocation();

	TraversalTraceResultInOut.BackLedgeNormal = UKismetMathLibrary::GetUpVector(BackLedgeTransform.Rotator());
}


