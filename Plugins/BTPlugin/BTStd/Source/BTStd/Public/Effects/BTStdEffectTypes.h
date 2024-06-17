// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Curves/CurveLinearColor.h"

#include "BTStdEffectTypes.generated.h"

UENUM()
enum class EBTStdEffectParamState : uint8
{
	None UMETA(ToolTip = "未設定（パラメータ名直接記入)"),
	MAX UMETA(Hidden)
};