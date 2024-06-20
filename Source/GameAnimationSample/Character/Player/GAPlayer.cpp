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

void AGAPlayer::ExecPerformTraversalAction()
{
	// パルクール用のモンタージュを再生
	UPlayMontageCallbackProxy* Proxy = UPlayMontageCallbackProxy::CreateProxyObjectForPlayMontage(GetMesh(), TraversalResult.ChosenMontage, TraversalResult.PlayRate, TraversalResult.StartTime, NAME_None);

	if(Proxy != nullptr)
	{
		Proxy->OnCompleted.AddDynamic(this, &AGAPlayer::EndPlayMontage);

		Proxy->OnInterrupted.AddDynamic(this, &AGAPlayer::EndPlayMontage);

		SNPLUGIN_LOG(TEXT("Montage is Playing & Delegate Registed."));
	} else
	{
		SNPLUGIN_ERROR(TEXT("Montage can't Play."));
	}

	bDoingTraversalAction = true;
	
	GetCapsuleComponent()->IgnoreComponentWhenMoving(TraversalResult.HitComponent, true);
	// ムーブメントモードを設定
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
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




