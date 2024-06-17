// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNode/AnimNode_SNSequencePlayer.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimationPoseData.h"
#include "Animation/ExposedValueHandler.h"
#include "Logging/TokenizedMessage.h"

#if WITH_EDITORONLY_DATA
#include "Animation/AnimBlueprintGeneratedClass.h"
#endif

#define LOCTEXT_NAMESPACE "AnimNode_SNSequencePlayer"

/////////////////////////////////////////////////////
// FAnimSequencePlayerNode

bool FAnimNode_SNSequencePlayer::SetSequence(UAnimSequenceBase* InSequence){
	
	Sequence = InSequence;
	
	return true;
}

bool FAnimNode_SNSequencePlayer::SetLoopAnimation(bool bInLoopAnimation)
{
#if WITH_EDITORONLY_DATA
	bLoopAnimation = bInLoopAnimation;
#endif
	
	if(bool* bLoopAnimationPtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(bool, bLoopAnimation))
	{
		*bLoopAnimationPtr = bInLoopAnimation;
		return true;
	}

	return false;
}

UAnimSequenceBase* FAnimNode_SNSequencePlayer::GetSequence() const
{	
	return Sequence;
}

float FAnimNode_SNSequencePlayer::GetPlayRateBasis() const
{
	return GET_ANIM_NODE_DATA(float, PlayRateBasis);
}

float FAnimNode_SNSequencePlayer::GetPlayRate() const
{
	return GET_ANIM_NODE_DATA(float, PlayRate);
}

const FInputScaleBiasClampConstants& FAnimNode_SNSequencePlayer::GetPlayRateScaleBiasClampConstants() const
{
	return GET_ANIM_NODE_DATA(FInputScaleBiasClampConstants, PlayRateScaleBiasClampConstants);
}

float FAnimNode_SNSequencePlayer::GetStartPosition() const
{
	return GET_ANIM_NODE_DATA(float, StartPosition);
}

bool FAnimNode_SNSequencePlayer::IsLooping() const
{
	return GET_ANIM_NODE_DATA(bool, bLoopAnimation);
}

bool FAnimNode_SNSequencePlayer::GetStartFromMatchingPose() const
{
	return GET_ANIM_NODE_DATA(bool, bStartFromMatchingPose);
}

FName FAnimNode_SNSequencePlayer::GetGroupName() const
{
	return GET_ANIM_NODE_DATA(FName, GroupName);
}

EAnimGroupRole::Type FAnimNode_SNSequencePlayer::GetGroupRole() const
{
	return GET_ANIM_NODE_DATA(TEnumAsByte<EAnimGroupRole::Type>, GroupRole);
}

EAnimSyncMethod FAnimNode_SNSequencePlayer::GetGroupMethod() const
{
	return GET_ANIM_NODE_DATA(EAnimSyncMethod, Method);
}

bool FAnimNode_SNSequencePlayer::GetIgnoreForRelevancyTest() const
{
	return GET_ANIM_NODE_DATA(bool, bIgnoreForRelevancyTest);
}

bool FAnimNode_SNSequencePlayer::SetGroupName(FName InGroupName)
{
#if WITH_EDITORONLY_DATA
	GroupName = InGroupName;
#endif

	if(FName* GroupNamePtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(FName, GroupName))
	{
		*GroupNamePtr = InGroupName;
		return true;
	}

	return false;
}

bool FAnimNode_SNSequencePlayer::SetGroupRole(EAnimGroupRole::Type InRole)
{
#if WITH_EDITORONLY_DATA
	GroupRole = InRole;
#endif

	if(TEnumAsByte<EAnimGroupRole::Type>* GroupRolePtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(TEnumAsByte<EAnimGroupRole::Type>, GroupRole))
	{
		*GroupRolePtr = InRole;
		return true;
	}

	return false;
}

bool FAnimNode_SNSequencePlayer::SetGroupMethod(EAnimSyncMethod InMethod)
{
#if WITH_EDITORONLY_DATA
	Method = InMethod;
#endif

	if(EAnimSyncMethod* MethodPtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(EAnimSyncMethod, Method))
	{
		*MethodPtr = InMethod;
		return true;
	}

	return false;
}

bool FAnimNode_SNSequencePlayer::SetIgnoreForRelevancyTest(bool bInIgnoreForRelevancyTest)
{
#if WITH_EDITORONLY_DATA
	bIgnoreForRelevancyTest = bInIgnoreForRelevancyTest;
#endif

	if(bool* bIgnoreForRelevancyTestPtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(bool, bIgnoreForRelevancyTest))
	{
		*bIgnoreForRelevancyTestPtr = bInIgnoreForRelevancyTest;
		return true;
	}

	return false;
}

bool FAnimNode_SNSequencePlayer::SetStartPosition(float InStartPosition)
{
	if(float* StartPositionPtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(float, StartPosition))
	{
		*StartPositionPtr = InStartPosition;
		return true;
	}

	return false;
}


bool FAnimNode_SNSequencePlayer::SetPlayRate(float InPlayRate)
{
	if(float* PlayRatePtr = GET_INSTANCE_ANIM_NODE_DATA_PTR(float, PlayRate))
	{
		*PlayRatePtr = InPlayRate;
		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE


