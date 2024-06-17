// Copyright Epic Games, Inc. All Rights Reserved.

#include "SNPlugin.h"

#define LOCTEXT_NAMESPACE "FSNPluginModule"

void FSNPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FSNPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

//----------------------------------------------------------------------//
//
//! @brief ワールドへのポインタを取得
//
//! @retval ワールドへのポインタ
//
//----------------------------------------------------------------------//
UWorld* GetPrimaryWorld(){
	
	UWorld* ReturnVal = nullptr;
	
	if(GEngine != nullptr){
		
		for(auto It = GEngine->GetWorldContexts().CreateConstIterator(); It; ++It){
			
			const FWorldContext& Context = *It;
			
			if((Context.WorldType == EWorldType::Game || Context.WorldType == EWorldType::PIE) && Context.World()){
				
				ReturnVal = Context.World();
				
				break;
			}
		}
	}
	
	return ReturnVal;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSNPluginModule, SNPlugin)