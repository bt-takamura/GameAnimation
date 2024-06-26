// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Base/SNPlayerBase.h"
#include "GameAnimationSample/Character/Animation/GAMotionMatchingDef.h"
#include "GAPlayer.generated.h"

class UChooserTable;
class USpringArmComponent;
class UMotionWarpingComponent;
class UCameraComponent;


/** Please add a struct description */
USTRUCT(BlueprintType)
struct FGATraversalChooserParams
{
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ActionType", MakeStructureDefaultValue="NewEnumerator0"))
	TEnumAsByte<ETraversalActionType> ActionType = ETraversalActionType::None;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Gait"))
	TEnumAsByte<EStride> Stride = EStride::Walk;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Speed", MakeStructureDefaultValue="0.000000"))
	float Speed = 0.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ObstacleHeight", MakeStructureDefaultValue="0.000000"))
	float ObstacleHeight = 0.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ObstacleDepth", MakeStructureDefaultValue="0.000000"))
	float ObstacleDepth = 0.0f;
};

/** Please add a struct description */
USTRUCT(BlueprintType)
struct FTraversalCheckResult
{
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ActionType", MakeStructureDefaultValue="NewEnumerator0"))
	TEnumAsByte<ETraversalActionType> ActionType = ETraversalActionType::None;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HasFrontLedge", MakeStructureDefaultValue="False"))
	bool HasFrontLedge = false;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="FrontLedgeLocation", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector FrontLedgeLocation = FVector::ZeroVector;;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="FrontLedgeNormal", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector FrontLedgeNormal = FVector::ZeroVector;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HasBackLedge", MakeStructureDefaultValue="False"))
	bool HasBackLedge = false;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="BackLedgeLocation", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector BackLedgeLocation = FVector::ZeroVector;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="BackLedgeNormal", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector BackLedgeNormal = FVector::ZeroVector;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HasBackFloor", MakeStructureDefaultValue="False"))
	bool HasBackFloor = false;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="BackFloorLocation", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector BackFloorLocation = FVector::ZeroVector;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ObstacleHeight", MakeStructureDefaultValue="0.000000"))
	float ObstacleHeight = 0.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ObstacleDepth", MakeStructureDefaultValue="0.000000"))
	float ObstacleDepth = 0.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="BackLedgeHeight", MakeStructureDefaultValue="0.000000"))
	float BackLedgeHeight = 0.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HitComponent", MakeStructureDefaultValue="None"))
	TObjectPtr<UPrimitiveComponent> HitComponent;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ChosenMontage", MakeStructureDefaultValue="None"))
	TObjectPtr<UAnimMontage> ChosenMontage;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="StartTime", MakeStructureDefaultValue="0.000000"))
	float StartTime = 0.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="PlayRate", MakeStructureDefaultValue="0.000000"))
	float PlayRate = 1.0f;
};


/** Please add a struct description */
USTRUCT(BlueprintType)
struct FSimpleCameraParams
{
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="SpringArmLength", MakeStructureDefaultValue="300.000000"))
	float SpringArmLength = 0.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="SocketOffset", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector SocketOffset = FVector::ZeroVector;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="TranslationLagSpeed", MakeStructureDefaultValue="10.000000"))
	float TranslationLagSpeed = 0.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="FieldOfView", MakeStructureDefaultValue="90.000000"))
	float FieldOfView = 0.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="TransitionSpeed", MakeStructureDefaultValue="5.000000"))
	float TransitionSpeed = 0.0f;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GAMEANIMATIONSAMPLE_API AGAPlayer : public ASNPlayerBase
{
	GENERATED_BODY()

public:

	AGAPlayer(const FObjectInitializer& Initializer);
	
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void SetEnableTraversalAction(bool bEnable);
	
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	bool GetEnableTraversalAction() const ;

	UFUNCTION(BlueprintPure, Category="GA|Components")
	USpringArmComponent* GetSpringArmComponent();

	UFUNCTION(BlueprintPure, Category="GA|Components")
	UMotionWarpingComponent* GetMotionWarpingComponent();

	UFUNCTION(BlueprintPure, Category="GA|Components")
	UCameraComponent* GetCameraComponent();

	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void ExecTraversalAction(float TraceForwardDistance, bool& TraversalCheckFailed, bool& MontageSelectionFailed);

	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	bool PerformFowardBlocks(UPARAM(ref) FTraversalCheckResult& TraversalCheckResult, float TraceForwardDistance, int DrawDebugLegel, float DrawDebugDuration);
	
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	bool PerformDecisionOnActorToEachEdge(UPARAM(ref) FTraversalCheckResult& TraversalCheckResult, int DrawDebugLegel);
	
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	bool PerformActorToFrontEdge(UPARAM(ref) FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckFromLedgeLocation, int DrawDebugLegel);
	
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void PerformObstacleDepth(UPARAM(ref) FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckFrontLedgeLocation, const FVector& HasRoomCheckBackLedgeLocation, int DrawDebugLegel);
		
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void PerformBackLedgeFloor(UPARAM(ref) FTraversalCheckResult& TraversalCheckResult, const FVector& HasRoomCheckBackLedgeLocation, int DrawDebugLegel);
	
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void DetermineTraversalAction(UPARAM(ref) FTraversalCheckResult& TraversalCheckResult);

	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void DrawDebugShapesAtLedgeLocation(const FTraversalCheckResult& TraversalCheckResult, int DrawDebugLevel, float DrawDebugDuration);
	
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void DebugPrintTraversalResult(int DrawDebugLevel,UPARAM(ref) FTraversalCheckResult& TraversalCheckResult);
	
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void ExecPerformTraversalAction(const FTraversalCheckResult& TraversalCheckResult);

	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void UpdateWarpTarget();
#if 1
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	TArray<UObject*> EvaluateChooser(UPARAM(ref) FTraversalCheckResult& TraversalCheckResult);
#endif
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	bool PerformMotionMatch(TArray<UObject*> SearchAssets, UPARAM(ref) FTraversalCheckResult& TraversalCheckResult);
	
	UFUNCTION(BlueprintCallable, Category="GA|Traversal", BlueprintPure)
	float GetTraversalForwardTraceDistance() const ;

	UFUNCTION(BlueprintCallable, Category="GA|Traversal", BlueprintPure)
	FVector2D GetMovementInputScaleValue(const FVector2D& Input) const ;

	UFUNCTION(BlueprintCallable, Category="GA|Traversal", BlueprintPure)
	EStride GetDesiredStride() const ;
	
	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void UpdateMovement();

	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void UpdateRotation();

	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void UpdateCamera(bool bInterpolate);

	UFUNCTION(BlueprintCallable, Category="GA|Traversal", BlueprintPure)
	float CalculateMaxSpeed();

	UFUNCTION(BlueprintCallable, Category="GA|Traversal")
	void SetInteractTransform(UPARAM(ref) FTraversalCheckResult& TraversalCheckResult);
	
private:
	UFUNCTION()
	void EndPlayMontage(FName NotifyName);

public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="GA|Default")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="GA|Default")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="GA|Default")
	TObjectPtr<UCameraComponent> CameraComponent;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Input")
	TEnumAsByte<EAnalogueMovementBehavior> MovementStickMode = EAnalogueMovementBehavior::FixedSpeed_SingleStride;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GA|Camera")
	FSimpleCameraParams CamStyleFar;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GA|Camera")
	FSimpleCameraParams CamStyleClose;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GA|Camera")
	FSimpleCameraParams CamStyleAim;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Input")
	float AnalogWalkRunThreshold = 0.7f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Input")
	bool bWantsToSprint = false;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Input")
	bool bWantsToWalk = false;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Movement")
	TObjectPtr<UCurveFloat> StrafeSpeedMapCurveObject;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Movement")
	TEnumAsByte<EStride> Stride;

	/** X = Forward Speed, Y = Strafe Speed, Z = Backwards Speed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GA|Movement")
	FVector WalkSpeed;

	/** X = Forward Speed, Y = Strafe Speed, Z = Backwards Speed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GA|Movement")
	FVector RunSpeed;

	/** X = Forward Speed, Y = Strafe Speed, Z = Backwards Speed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GA|Movement")
	FVector SprintSpeed;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Input")
	bool bWantsToStrafe = true;

	/** X = Forward Speed, Y = Strafe Speed, Z = Backwards Speed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GA|Movement")
	FVector CrouchSpeed;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GA|Camera")
	float CameraDistanceMag = 1.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Movement")
	bool bJustLanded = false;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Movement", meta=(BlueprintThreadSafe ))
	FVector LandSpeed;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Input")
	bool bWantsToAim = false;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Traversal")
	FTraversalCheckResult TraversalResult;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="GA|Traversal")
	bool bDoingTraversalAction;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="GA|Traversal")
	TObjectPtr<UChooserTable> TraversalAnimationChooser = nullptr;

protected:
	virtual void BeginPlay() override;
public:
	
	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaSeconds) override;
};
