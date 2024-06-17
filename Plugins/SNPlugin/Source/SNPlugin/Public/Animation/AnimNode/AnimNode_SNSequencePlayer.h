// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNode_SequencePlayer.h"
#include "AnimNode_SNSequencePlayer.generated.h"

/**
 * 
 */
USTRUCT(BlueprintInternalUseOnly)
struct SNPLUGIN_API FAnimNode_SNSequencePlayer : public FAnimNode_SequencePlayerBase
{
	GENERATED_BODY()

protected:
	
	friend class UAnimGraphNode_SNSequencePlayer;
	
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, DisallowedClasses="/Script/Engine.AnimMontage"))
	FName AnimationName;
	
#if WITH_EDITORONLY_DATA
	// The group name that we synchronize with (NAME_None if it is not part of any group). 
	UPROPERTY(EditAnywhere, Category=Sync, meta=(FoldProperty))
	FName GroupName = NAME_None;

	// The role this node can assume within the group (ignored if GroupName is not set)
	UPROPERTY(EditAnywhere, Category=Sync, meta=(FoldProperty))
	TEnumAsByte<EAnimGroupRole::Type> GroupRole = EAnimGroupRole::CanBeLeader;

	// How this node will synchronize with other animations.
	UPROPERTY(EditAnywhere, Category=Sync, meta=(FoldProperty))
	EAnimSyncMethod Method = EAnimSyncMethod::DoNotSync;

	// If true, "Relevant anim" nodes that look for the highest weighted animation in a state will ignore this node
	UPROPERTY(EditAnywhere, Category=Relevancy, meta=(FoldProperty, PinHiddenByDefault))
	bool bIgnoreForRelevancyTest = false;
#endif

	//!< 再生するアニメーションシーケンスアセット
	UPROPERTY(meta = (PinHiddenByDefault, DisallowedClasses="/Script/Engine.AnimMontage"))
	TObjectPtr<UAnimSequenceBase> Sequence = nullptr;

#if WITH_EDITORONLY_DATA
	// The Basis in which the PlayRate is expressed in. This is used to rescale PlayRate inputs.
	// For example a Basis of 100 means that the PlayRate input will be divided by 100.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	float PlayRateBasis = 1.0f;

	// The play rate multiplier. Can be negative, which will cause the animation to play in reverse.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	float PlayRate = 1.0f;
	
	// Additional scaling, offsetting and clamping of PlayRate input.
	// Performed after PlayRateBasis.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (DisplayName="PlayRateScaleBiasClamp", FoldProperty))
	FInputScaleBiasClampConstants PlayRateScaleBiasClampConstants;

	UPROPERTY()
	FInputScaleBiasClamp PlayRateScaleBiasClamp_DEPRECATED;

	// The start position between 0 and the length of the sequence to use when initializing. When looping, play will still jump back to the beginning when reaching the end.
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	float StartPosition = 0.0f;

	// Should the animation loop back to the start when it reaches the end?
	UPROPERTY(EditAnywhere, Category = Settings, meta = (PinHiddenByDefault, FoldProperty))
	bool bLoopAnimation = true;

	// Use pose matching to choose the start position. Requires experimental PoseSearch plugin.
	UPROPERTY(EditAnywhere, Category = PoseMatching, meta = (PinHiddenByDefault, FoldProperty))
	bool bStartFromMatchingPose = false;
#endif

public:
	// FAnimNode_SequencePlayerBase interface
	virtual bool SetSequence(UAnimSequenceBase* InSequence) override;
	virtual bool SetLoopAnimation(bool bInLoopAnimation) override;
	virtual UAnimSequenceBase* GetSequence() const override;
	virtual float GetPlayRateBasis() const override;
	virtual float GetPlayRate() const override;
	virtual const FInputScaleBiasClampConstants& GetPlayRateScaleBiasClampConstants() const override;
	virtual float GetStartPosition() const override;
	virtual bool GetStartFromMatchingPose() const override;
	virtual bool SetStartPosition(float InStartPosition) override;
	virtual bool SetPlayRate(float InPlayRate) override;

	// FAnimNode_AssetPlayerBase interface
	virtual FName GetGroupName() const override;
	virtual EAnimGroupRole::Type GetGroupRole() const override;
	virtual EAnimSyncMethod GetGroupMethod() const override;
	virtual bool IsLooping() const override;
	virtual bool GetIgnoreForRelevancyTest() const override;
	virtual bool SetGroupName(FName InGroupName) override;
	virtual bool SetGroupRole(EAnimGroupRole::Type InRole) override;
	virtual bool SetGroupMethod(EAnimSyncMethod InMethod) override;
	virtual bool SetIgnoreForRelevancyTest(bool bInIgnoreForRelevancyTest) override;
};
