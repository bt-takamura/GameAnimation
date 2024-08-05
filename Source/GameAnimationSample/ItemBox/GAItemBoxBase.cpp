// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/ItemBox/GAItemBoxBase.h"

#include "SNDef.h"
#include "Components/ArrowComponent.h"

//----------------------------------------------------------------------//
//
//! @brief �f�t�H���g�R���X�g���N�^
//
//----------------------------------------------------------------------//
AGAItemBoxBase::AGAItemBoxBase(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	
	LidPivotComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("LidPivot"));
	
	ItemBoxBaseComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemBoxBase"));
	
	ItemBoxLidComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemBoxLid"));
	
	LidPivotComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	
	ItemBoxBaseComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	
	ItemBoxLidComponent->AttachToComponent(LidPivotComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AGAItemBoxBase::OpenRequest()
{
	if(TimerHandle.IsValid())
	{
		SNPLUGIN_LOG(TEXT("OpenRequest : ItemBox Already Request."));

		return;
	}
	
	FTimerDelegate Delegate;

	Delegate.BindUObject(this, &AGAItemBoxBase::Open);
	
	GetWorldTimerManager().SetTimer(TimerHandle, Delegate, 0.016f, true);
}

void AGAItemBoxBase::CloseRequest()
{
	if(TimerHandle.IsValid())
	{
		SNPLUGIN_LOG(TEXT("CloseRequest : ItemBox Already Request."));

		return;
	}
	
	FTimerDelegate Delegate;

	Delegate.BindUObject(this, &AGAItemBoxBase::Close);
	
	GetWorldTimerManager().SetTimer(TimerHandle, Delegate, 0.016f, true);
}


void AGAItemBoxBase::Open()
{
	FRotator Rotator(LidPivotComponent->GetComponentRotation());

	Rotator.Yaw -= Speed;

	if(Rotator.Yaw <=-90.0f)
	{
		Rotator.Yaw =-90.0f;

		GetWorldTimerManager().ClearTimer(TimerHandle);;

		TimerHandle.Invalidate();
	}

	LidPivotComponent->SetRelativeRotation(Rotator);
}

void AGAItemBoxBase::Close()
{
	FRotator Rotator(LidPivotComponent->GetComponentRotation());

	Rotator.Yaw += Speed;

	if(Rotator.Yaw >= 0.0f)
	{
		Rotator.Yaw = 0.0f;

		GetWorldTimerManager().ClearTimer(TimerHandle);;

		TimerHandle.Invalidate();
	}

	LidPivotComponent->SetRelativeRotation(Rotator);
}


// Called when the game starts or when spawned
void AGAItemBoxBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGAItemBoxBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

