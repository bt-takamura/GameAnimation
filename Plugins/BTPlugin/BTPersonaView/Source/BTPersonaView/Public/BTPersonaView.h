//--------------------------------------------------
//! @file   BTPersonaView.h
//! @brief  BTPersonaView
//! @author BT.Masaki Okuda
//--------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBTPersonaViewModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
