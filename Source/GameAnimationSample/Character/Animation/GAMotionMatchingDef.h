// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum EStride
{
	Walk UMETA(DisplayName = "Walk", ToopTip="Walk"),
	Run UMETA(DisplayName = "Run", ToopTip="Run"),
	Sprint UMETA(DisplayName = "Sprint", ToopTip="Sprint"),
};

UENUM(BlueprintType)
enum EMotionMatchingMovementMode
{
	OnGround UMETA(DisplayName = "OnGround", ToopTip="OnGround"),
	InAir UMETA(DisplayName = "InAir", ToopTip="InAir"),
};

UENUM(BlueprintType)
enum EMotionMatchingRotationMode
{
	OrientToMovement UMETA(DisplayName = "OrientToMovement", ToopTip="OrientToMovement"),
	Strafe UMETA(DisplayName = "Strafe", ToopTip="Strafe"),
};

UENUM(BlueprintType)
enum EMotionMatchingMovementState
{
	Idle UMETA(DisplayName = "Idle", ToopTip="Idle"),
	Moving UMETA(DisplayName = "Moving", ToopTip="Moving"),
};

UENUM(BlueprintType)
enum EMotionMatchingStance
{
	Stand UMETA(DisplayName = "Stand", ToopTip="Stand"),
	Crouch UMETA(DisplayName = "Crouch", ToopTip="Crouch"),
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


