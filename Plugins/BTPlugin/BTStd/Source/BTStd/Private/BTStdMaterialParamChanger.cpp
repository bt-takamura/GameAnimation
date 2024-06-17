// Copyright Epic Games, Inc. All Rights Reserved.

#include "Materials/BTStdMaterialParamChanger.h"

FBTStdMaterialParamChangerHandle::FBTStdMaterialParamChangerHandle(FBTStdMaterialParamChangerBase* InChanger, uint64 InId)
{
	m_pChanger = InChanger;
	m_UniqueId = InId;
}

bool FBTStdMaterialParamChangerHandle::IsValid() const
{
	return !!Get();
}

void FBTStdMaterialParamChangerHandle::Reset()
{
	m_pChanger = nullptr;
	m_UniqueId = 0;
}

FBTStdMaterialParamChangerBase* FBTStdMaterialParamChangerHandle::Get() const
{
	return m_pChanger;
}

FBTStdMaterialParamChangerBase* FBTStdMaterialParamChangerHandle::operator->() const
{
	return Get();
}