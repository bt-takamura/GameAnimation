#pragma once

#include "CoreTypes.h"

class USNDataAssetManager;

class ISNDLCModuleInterface:public IModuleInterface
{
public:
	
	virtual TSoftClassPtr<USNContentAssetManager> GetAssetManagerSoftClassPtr() = 0;
};
