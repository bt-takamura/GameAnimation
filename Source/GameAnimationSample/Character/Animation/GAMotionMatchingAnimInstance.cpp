// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Animation/GAMotionMatchingAnimInstance.h"

#include "PoseSearch/PoseSearchTrajectoryLibrary.h"

UGAMotionMatchingAnimInstance::UGAMotionMatchingAnimInstance()
:Super()
,TrajectoryGenerationDataIdle(FPoseSearchTrajectoryData::StaticStruct())
,TrajectoryGenerationDataMoving(FPoseSearchTrajectoryData::StaticStruct())
,Trajectory(FPoseSearchQueryTrajectory::StaticStruct())
,TrajectoryCollision(FPoseSearchTrajectory_WorldCollisionResults::StaticStruct())
{
	
}

void UGAMotionMatchingAnimInstance::SetInteractionTransform(const FTransform& Transform)
{
	InteractionTransform = Transform;
}

const FTransform& UGAMotionMatchingAnimInstance::GetInteractionTransform() const
{
	return InteractionTransform;
}

