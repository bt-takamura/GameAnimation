//--------------------------------------------------
//! @file   BTPersonaView.cpp
//! @brief  BTPersonaView
//! @author BT.Masaki Okuda
//--------------------------------------------------
#include "BTPersonaView.h"

#include "BTPersonaViewPrivate.h"

DEFINE_LOG_CATEGORY(LogBTPeronaView);

#define LOCTEXT_NAMESPACE "FBTPersonaViewModule"

void FBTPersonaViewModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FBTPersonaViewModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBTPersonaViewModule, BTPersonaView)