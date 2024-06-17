#include "BTStdEditor.h"

#include "BTStdEditorPrivate.h"

DEFINE_LOG_CATEGORY(LogBTStdEditor);

#define LOCTEXT_NAMESPACE "FBTStdEditorModule"

void FBTStdEditorModule::StartupModule()
{
}

void FBTStdEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FBTStdEditorModule, BTStdEditor)