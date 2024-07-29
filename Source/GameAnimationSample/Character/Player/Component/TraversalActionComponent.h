// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAnimationSample/Character/Animation/GAMotionMatchingDef.h"
#include "Components/ActorComponent.h"
//! �ꎞ�I
#include "GameAnimationSample/Character/Player/GAPlayer.h"
//! 
#include "TraversalActionComponent.generated.h"

//USTRUCT()
//struct FGATraversalChooserParams
//{
//	GENERATED_BODY()
//
//public:
//	//!< �g���o�[�T���A�N�V�����̃^�C�v
//	TEnumAsByte<ETraversalActionType> ActionType = ETraversalActionType::None;
//
//	//!< �ړ����
//	TEnumAsByte<EStride> Stride = EStride::Walk;
//
//	//!< �ړ����x
//	float Speed = 0.0f;
//
//	//!< ��Q���̍���
//	float ObstacleHeight = 0.0f;
//
//	//!< ���ɂ����Q���Ƃ̋���
//	float ObstacleDepth = 0.0f;
//	
//};
//
//USTRUCT()
//struct FTraversalCheckResult
//{
//	GENERATED_BODY()
//
//public:
//	//!< �g���o�[�T���A�N�V�����̃^�C�v
//	TEnumAsByte<ETraversalActionType> ActionType = ETraversalActionType::None;
//
//	//!< 
//	bool HasFrontLedge = false;
//
//	//!< �t�����g���b�W�̈ʒu
//	FVector FrontLedgeLocation = FVector::ZeroVector;
//
//	//!< �t�����g���b�W�̖@��
//	FVector FrontLedgeNormal = FVector::ZeroVector;
//
//	//!< 
//	bool HasBackLedge = false;
//
//	//!< �o�b�N���b�W�̈ʒu
//	FVector BackLedgeLocation = FVector::ZeroVector;
//
//	//!< �o�b�N���b�W�̖@��
//	FVector BackLedgeNormal = FVector::ZeroVector;
//
//	//!< 
//	bool HasBackFloor = false;
//
//	//!< 
//	FVector BackFloorLocation = FVector::ZeroVector;
//
//	//!< ��Q���̍���
//	float ObstacleHeight = 0.0f;
//
//	//!< ���ɂ����Q���Ƃ̋���
//	float ObstacleDepth = 0.0f;
//
//	//!< �o�b�N���b�W�̍���
//	float BackLedgeHeight = 0.0f;
//
//	//!< 
//	TObjectPtr<UPrimitiveComponent> HitComponent;
//
//	//!< �I�����ꂽ�����^�[�W��
//	TObjectPtr<UAnimMontage> ChosenMontage;
//
//	//!< �Đ��J�n����
//	float StartTime = 0.0f;
//
//	//!< �Đ����x�{��
//	float PlayRate = 1.0f;
//
//};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEANIMATIONSAMPLE_API UTraversalActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTraversalActionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//! @{@name �g���o�[�T���A�N�V���������`�F�b�N
	UFUNCTION(BlueprintCallable, Category = "GA|Traversal")
	bool IsEnableTraversalAction() const;
	//! @}
	
	//! @{@name �g���o�[�T���A�N�V���������s
	UFUNCTION(BlueprintCallable, Category = "GA|Traversal")
	void ExecTraversalAction(float TraceForwardDistance, bool& TraversalCheckFailed, bool& MontageSelectionFailed);
	//! @}
		
private:
	//! @{@name �i�s������Block�����邩�`�F�b�N
	bool PerformFowardBlocks(FTraversalCheckResult& TraversalCheckResult, float TraceForwardDistance, int DrawDebugLevel, float DrawDebugDuration);
	//! @}
	
	//! @{@name ���b�W�̍��W�ɋ���`��
	void DrawDebugShapesAtLedgeLocation(const FTraversalCheckResult& TraversalCheckResult, int DrawDebugLevel, float DrawDebugDuration);
	//! @}

	//!< �g���o�[�T���A�N�V��������
	bool bDoingTraversalAction;
};

FORCEINLINE bool UTraversalActionComponent::IsEnableTraversalAction() const
{
	return bDoingTraversalAction;
}
