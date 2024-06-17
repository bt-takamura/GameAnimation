// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SNAnimInstanceBase.h"

#include "SNDef.h"
#include "Character/Base/SNCharacterBase.h"
#include "Data/SNRelevantDataAsset.h"

void USNAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if(AnimationAssetList != nullptr)
	{
		AnimationAssetList->GetAssetLoadFinishedEvent().AddUObject(this, &USNAnimInstanceBase::FinishLoadedAnimationAsset);

		AnimationAssetList->Setup();

		SNPLUGIN_LOG(TEXT("Animation Native Initialize is Done."));
	}
}

bool USNAnimInstanceBase::IsCurrentState(const FName& State, ECharacterStateType Type) const
{
	ASNCharacterBase* Character(Cast<ASNCharacterBase>(TryGetPawnOwner()));

	if(Character == nullptr)
	{
		return false;
	}

	return Character->IsCurrentState(State, Type);
}

bool USNAnimInstanceBase::IsPreState(const FName& State, ECharacterStateType Type) const
{
	ASNCharacterBase* Character(Cast<ASNCharacterBase>(TryGetPawnOwner()));

	if(Character == nullptr)
	{
		return false;
	}

	return Character->IsPreState(State, Type);
}

FVector USNAnimInstanceBase::GetBlendspaceParam(const FName& Key)
{
	FVector Result(FVector::ZeroVector);

	if(BlendspaceParam.Find(Key) != nullptr)
	{
		Result = BlendspaceParam[Key];
	}

	return Result;
}

void USNAnimInstanceBase::SetBlendspaceParam(const FName& Key, const FVector& param)
{
	if(BlendspaceParam.Find(Key) != nullptr)
	{
		BlendspaceParam[Key]  = param;
	} else
	{
		BlendspaceParam.Add(Key, param);
	}
}


void USNAnimInstanceBase::FinishLoadedAnimationAsset()
{

}

UAnimationAsset* USNAnimInstanceBase::GetAnimationAsset(const FName& Name)
{
	UAnimationAsset* AnimationAsset(nullptr);
	
	if(AnimationAssetList != nullptr)
	{
		AnimationAsset = Cast<UAnimationAsset>(AnimationAssetList->GetAsset(Name));	
	}

	return AnimationAsset;
}

const UAnimationAsset* USNAnimInstanceBase::GetAnimationAsset(const FName& Name) const
{
	UAnimationAsset* AnimationAsset(nullptr);
	
	if(AnimationAssetList != nullptr)
	{
		AnimationAsset = Cast<UAnimationAsset>(AnimationAssetList->GetAsset(Name));	
	}

	return AnimationAsset;
}

const UAnimSequence* USNAnimInstanceBase::GetAnimationSequence(const FName& Name) const
{
	return Cast<UAnimSequence>(GetAnimationAsset(Name));
}

const UAnimMontage* USNAnimInstanceBase::GetAnimationMontage(const FName& Name) const
{
	return Cast<UAnimMontage>(GetAnimationAsset(Name));
}

const UBlendSpace* USNAnimInstanceBase::GetBlendSpace(const FName& Name) const
{
	return Cast<UBlendSpace>(GetAnimationAsset(Name));
}

void USNAnimInstanceBase::PlayAnimationSequence(const FName& Name, const FName& Slot, float PlayRate, float BlendIn, float BlendOut, float StartTime, bool bLoop)
{
	UAnimSequence* Sequence(GetAnimationSequence(Name));

	if(Sequence != nullptr)
	{
		PlaySlotAnimationAsDynamicMontage_WithBlendArgs(Sequence, Slot, FAlphaBlendArgs(BlendIn), FAlphaBlendArgs(BlendOut), PlayRate);
	}
}

void USNAnimInstanceBase::PlayAnimationMontage(const FName& Name, float PlayRate, float StartTime)
{
	UAnimMontage* Montage(GetAnimationMontage(Name));

	if(Montage != nullptr)
	{
		Montage_Play(Montage, PlayRate, EMontagePlayReturnType::MontageLength, StartTime, true);
	}
}

void USNAnimInstanceBase::JumpAnimationMontageSection(const FName& Name, const FName& Section)
{
	UAnimMontage* Montage(GetAnimationMontage(Name));

	if(Montage != nullptr)
	{
		Montage_JumpToSection(Section, Montage);		
	}
}



