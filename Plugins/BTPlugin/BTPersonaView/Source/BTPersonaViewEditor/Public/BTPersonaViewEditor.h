//--------------------------------------------------
//! @file   BTPersonaViewEditor.h
//! @brief  BTPersonaViewEditor
//! @author BT.Masaki Okuda
//--------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


class FBTPersonaViewEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
};
