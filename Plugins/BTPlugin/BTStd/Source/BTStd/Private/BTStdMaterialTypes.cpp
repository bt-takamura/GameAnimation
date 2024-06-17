// Copyright Epic Games, Inc. All Rights Reserved.

#include "Materials/BTStdMaterialTypes.h"

const FBTStdMaterialRGBAMask FBTStdMaterialRGBAMask::RGBA = FBTStdMaterialRGBAMask(true, true, true, true);
const FBTStdMaterialRGBAMask FBTStdMaterialRGBAMask::RGB = FBTStdMaterialRGBAMask(true, true, true, false);
const FBTStdMaterialRGBAMask FBTStdMaterialRGBAMask::R = FBTStdMaterialRGBAMask(true, false, false, false);
const FBTStdMaterialRGBAMask FBTStdMaterialRGBAMask::G = FBTStdMaterialRGBAMask(false, true, false, false);
const FBTStdMaterialRGBAMask FBTStdMaterialRGBAMask::B = FBTStdMaterialRGBAMask(false, false, true, false);
const FBTStdMaterialRGBAMask FBTStdMaterialRGBAMask::A = FBTStdMaterialRGBAMask(false, false, false, true);
const FBTStdMaterialRGBAMask FBTStdMaterialRGBAMask::None = FBTStdMaterialRGBAMask(false, false, false, false);