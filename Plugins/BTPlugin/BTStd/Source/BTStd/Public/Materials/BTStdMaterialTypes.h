// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Curves/CurveLinearColor.h"

#include "BTStdMaterialTypes.generated.h"

struct BTSTD_API FBTStdMaterialRGBAMask
{
	bool bR;
	bool bG;
	bool bB;
	bool bA;

	explicit FBTStdMaterialRGBAMask(bool InAll) { bR = bG = bB = bA = InAll; }
	explicit FBTStdMaterialRGBAMask(bool InR, bool InG, bool InB, bool InA) : bR(InR), bG(InG), bB(InB), bA(InA) {}

	bool Any() const { return bR || bG || bB || bA; }
	bool All() const { return bR && bG && bB && bA; }

	static const FBTStdMaterialRGBAMask RGBA;
	static const FBTStdMaterialRGBAMask RGB;
	static const FBTStdMaterialRGBAMask R;
	static const FBTStdMaterialRGBAMask G;
	static const FBTStdMaterialRGBAMask B;
	static const FBTStdMaterialRGBAMask A;
	static const FBTStdMaterialRGBAMask None;
};

UENUM()
enum class EBTStdMaterialParamState : uint8
{
	None UMETA(ToolTip = "未設定（パラメータ名直接記入)"),
	MAX UMETA(Hidden)
};