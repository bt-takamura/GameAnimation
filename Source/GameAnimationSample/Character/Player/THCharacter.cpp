// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimationSample/Character/Player/THCharacter.h"

#include "Character/SNPlayerController.h"
#include "Utility/SNUtility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameAnimationSample/Character/Player/Component/MMLocomotionComponent.h"
#include "GameAnimationSample/Character/Player/Component/ClimbActionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ATHCharacter::ATHCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));

	SpringArmComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	CameraComponent->SetupAttachment(SpringArmComponent);

	ClimbActionComponent = CreateDefaultSubobject<UClimbActionComponent>(TEXT("ClimbActionComponent"));
}

void ATHCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCamera(true);
}

void ATHCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATHCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//! タイマーの解放
	FTimerManager& TimerManager = GetWorldTimerManager();

	TimerManager.ClearTimer(TimerHandle);
	TimerManager.ClearAllTimersForObject(this);
}

//----------------------------------------------------------------------//
//
//! @brief クライム開始時の掴まる姿勢の設定
//
//! @param HitResult	TryAction時の衝突判定の結果
//! @param OutTransform 設定する掴まる姿勢情報
//
//----------------------------------------------------------------------//
void ATHCharacter::SetClimbGraspTransform(FHitResult& HitResult, FTransform& OutTransform)
{
	FVector Location = HitResult.ImpactPoint;
	Location.Z = GetActorLocation().Z + ClimbHeight;

	FVector RotateVector = HitResult.ImpactNormal * FVector(-1.0f, -1.0f, 0.0f);
	FRotator Rotator = RotateVector.ToOrientationRotator();

	OutTransform.SetLocation(Location);
	OutTransform.SetRotation(Rotator.Quaternion());
}

//----------------------------------------------------------------------//
//
//! @brief Climb中の移動処理
//! 
//! @param InputValue	コントローラー3Dスティック入力値
//
//----------------------------------------------------------------------//
void ATHCharacter::ClimbMove(const FVector2D& InputValue)
{
	//! 左右移動
	FVector RightVector = GetActorRightVector();
	AddMovementInput(RightVector, InputValue.X);

	//! 上下移動
	FVector UpVector = GetActorUpVector();
	AddMovementInput(UpVector, InputValue.Y);
}


void ATHCharacter::BeginPlay()
{
	Super::BeginPlay();

	ASNPlayerController* PlayerController = SNUtility::GetPlayerController<ASNPlayerController>();
	if (PlayerController != nullptr)
	{
		PlayerController->EnabledInputType(FName(TEXT("Normal")));
	}

	//! 落下時のイベント関数を追加
	LandedDelegate.AddDynamic(this, &ATHCharacter::OnLandedDelegate);
}

void ATHCharacter::UpdateMovement()
{
	Super::UpdateMovement();

	//! 飛行モードの移動速度を更新(クライムアクションで移動するため)
	GetCharacterMovement()->MaxFlySpeed = ClimbActionComponent->CalcMaxFlySpeed();
}

void ATHCharacter::UpdateRotation()
{
	Super::UpdateRotation();
	
	if (ClimbActionComponent->IsClimbing() == true)
	{
		ClimbActionComponent->SetClimbRotation();
	}
}

void ATHCharacter::UpdateCamera(bool bInterpolate)
{
	FCameraParams* CameraParams = nullptr;

	//! 注視モード
	bool WantsToAction = GetMMLocomotionComponent()->GetWantsToAim();
	if (WantsToAction == true)
	{
		CameraParams = &CamStyleAim;
	}
	else
	{
		CameraParams = &CamStyleClose;
	}

	//! ストレイフモード
	WantsToAction = GetMMLocomotionComponent()->GetWantsToStrafe();
	if(WantsToAction == false)
	{
		CameraParams = &CamStyleFar;
	}

	FCameraParams TargetCameraParams;
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

//----------------------------------------------------------------------//
//
//! @brief 着地時の処理、デリゲートに追加する関数
//! 
//! @param Hit	着地を示す結果
//
//----------------------------------------------------------------------//
void ATHCharacter::OnLandedDelegate(const FHitResult& Hit)
{
	FVector LandSpeed = GetCharacterMovement()->Velocity;
	GetMMLocomotionComponent()->SetLandSpeed(LandSpeed);

	GetMMLocomotionComponent()->SetJustLanded(true);

	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.SetTimer(TimerHandle, this, &ATHCharacter::TurnOffJustLandded, 0.3f, false);

}

void ATHCharacter::TurnOffJustLandded()
{
	GetMMLocomotionComponent()->SetJustLanded(false);
}
