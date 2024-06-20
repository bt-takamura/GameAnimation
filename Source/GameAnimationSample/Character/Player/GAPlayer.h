// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Base/SNCharacterBase.h"
#include "GAPlayer.generated.h"

class USpringArmComponent;
class UMotionWarpingComponent;
class UCameraComponent;

UENUM(BlueprintType)
enum EStride
{
	Walk UMETA(DisplayName = "Walk", ToopTip="Walk"),
	Run UMETA(DisplayName = "Run", ToopTip="Run"),
	Sprint UMETA(DisplayName = "Sprint", ToopTip="Sprint"),
};

UENUM(BlueprintType)
enum EAnalogueMovementBehavior
{
	FixedSpeed_SingleStride UMETA(DisplayName = "Fixed Speed - Single Gait", ToopTip="Character will move at a fixed speed regardless of stick deflection."),
	FixedSpeed_WalkRun UMETA(DisplayName = "Fixed Speed - Walk / Run", ToopTip="Character will move at a fixed walking speed with slight stick deflection, and a fixed running speed at full stick deflection."),
	VariableSpeed_SingleStride UMETA(DisplayName = "Variable Speed - Single Gait", ToopTip="Full analog movement control with stick, character will remain walking or running based on gait input."),
	VariableSpeed_WalkRun UMETA(DisplayName = "Variable Speed - Walk / Run", ToopTip="Full analog movement control with stick, character will switch from walk to run gait based on stick deflection."),
};

UENUM(BlueprintType)
enum ETraversalActionType
{
	None UMETA(DisplayName = "None", ToopTip=""),
	Hurdle UMETA(DisplayName = "Hurdle", ToopTip="Traverse over a thin object and end on the ground at a similar level (Low fence)"),
	Vault UMETA(DisplayName = "Vault", ToopTip="Traverse over a thin object and end in a falling state (Tall fence, or elevated obstacle with no floor on the other side)"),
	Mantle UMETA(DisplayName = "Mantle", ToopTip="Traverse up and onto an object without passing over it"),
};

/** Please add a struct description */
USTRUCT(BlueprintType)
struct FGATraversalChooserParams
{
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ActionType", MakeStructureDefaultValue="NewEnumerator0"))
	TEnumAsByte<ETraversalActionType> ActionType;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Gait"))
	TEnumAsByte<EStride> Stride;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Speed", MakeStructureDefaultValue="0.000000"))
	float Speed;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ObstacleHeight", MakeStructureDefaultValue="0.000000"))
	float ObstacleHeight;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ObstacleDepth", MakeStructureDefaultValue="0.000000"))
	float ObstacleDepth;
};

/** Please add a struct description */
USTRUCT(BlueprintType)
struct FTraversalCheckResult
{
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ActionType", MakeStructureDefaultValue="NewEnumerator0"))
	TEnumAsByte<ETraversalActionType> ActionType;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HasFrontLedge", MakeStructureDefaultValue="False"))
	bool HasFrontLedge;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="FrontLedgeLocation", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector FrontLedgeLocation;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="FrontLedgeNormal", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector FrontLedgeNormal;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HasBackLedge", MakeStructureDefaultValue="False"))
	bool HasBackLedge;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="BackLedgeLocation", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector BackLedgeLocation;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="BackLedgeNormal", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector BackLedgeNormal;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HasBackFloor", MakeStructureDefaultValue="False"))
	bool HasBackFloor;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="BackFloorLocation", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector BackFloorLocation;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ObstacleHeight", MakeStructureDefaultValue="0.000000"))
	float ObstacleHeight;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ObstacleDepth", MakeStructureDefaultValue="0.000000"))
	float ObstacleDepth;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="BackLedgeHeight", MakeStructureDefaultValue="0.000000"))
	float BackLedgeHeight;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HitComponent", MakeStructureDefaultValue="None"))
	TObjectPtr<UPrimitiveComponent> HitComponent;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ChosenMontage", MakeStructureDefaultValue="None"))
	TObjectPtr<UAnimMontage> ChosenMontage;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="StartTime", MakeStructureDefaultValue="0.000000"))
	float StartTime;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="PlayRate", MakeStructureDefaultValue="0.000000"))
	float PlayRate;
};


/** Please add a struct description */
USTRUCT(BlueprintType)
struct FSimpleCameraParams
{
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="SpringArmLength", MakeStructureDefaultValue="300.000000"))
	float SpringArmLength;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="SocketOffset", MakeStructureDefaultValue="0.000000,0.000000,0.000000"))
	FVector SocketOffset;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="TranslationLagSpeed", MakeStructureDefaultValue="10.000000"))
	float TranslationLagSpeed;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="FieldOfView", MakeStructureDefaultValue="90.000000"))
	float FieldOfView;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="TransitionSpeed", MakeStructureDefaultValue="5.000000"))
	float TransitionSpeed;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GAMEANIMATIONSAMPLE_API AGAPlayer : public ASNCharacterBase
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
	void ExecPerformTraversalAction();

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
};
