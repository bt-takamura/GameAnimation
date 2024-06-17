#pragma once

#include "Misc/EnumRange.h"

UENUM(BlueprintType)
enum ECharacterStateType
{
	Upper = 0 UMETA(DisplayName = "Upper half of body", ToolTip="Upper half of body"),
	Lower = 1 UMETA(DisplayName = "Lower half of body", ToolTip="Lower half of body"),
	Full  = 2 UMETA(DisplayName = "Whole body", ToolTip="Whole body"),
	Num,
};

ENUM_RANGE_BY_COUNT(ECharacterStateType, ECharacterStateType::Num);

