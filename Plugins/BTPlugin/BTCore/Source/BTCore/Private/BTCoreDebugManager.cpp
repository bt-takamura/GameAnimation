// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTCoreDebugManager.h"

#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SDPIScaler.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SViewport.h"

#if !UE_BUILD_SHIPPING

#include "BTCorePrivate.h"
#include "BTCoreDebugTypes.h"
#include "BTCoreWorldSubsystem.h"

#include "SBTCoreDebugMenu.h"

static void ShowDebugMenu(const TArray<FString>& Args, UWorld* InWorld);

namespace BeeTribe::Debug
{
	BT_DECLARE_DEBUGCOMMAND_VARIABLE(bool, ShowDebugCommandList, false, 0, 1, TEXT("Debug."), TEXT("デバッグコマンドリストを表示する"), 0);
	BT_DECLARE_DEBUGCOMMAND_VARIABLE(bool, ShowDebugCommandList2, false, 0, 1, TEXT("Debug."), TEXT("デバッグコマンドリストを表示する"), 0);

	BT_DECLARE_DEBUGCOMMAND_WITH_WORLD_AND_ARGS(ShowDebugMenu, TEXT("Debug."), TEXT("デバッグメニューを表示します"), FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(ShowDebugMenu), IBTCoreConsoleManager::INVISIBILITY_PRIORITY);
}

#define LOCTEXT_NAMESPACE "BTCoreDebugManager"

void ShowDebugMenu(const TArray<FString>& Args, UWorld* InWorld)
{
	if (InWorld == nullptr)
	{
		return;
	}

	UGameViewportClient* ViewportClient = nullptr;
	bool bResult = false;
	if (InWorld->WorldType == EWorldType::Game || InWorld->WorldType == EWorldType::PIE)
	{
		ViewportClient = InWorld->GetGameViewport();
		bResult = ViewportClient != nullptr;
	}

	if (UBTCoreWorldSubsystem* Subsystem = UBTCoreWorldSubsystem::GetWorldSubsystemInternal(InWorld))
	{
		if (bResult)
		{
#if PLATFORM_WINDOWS
			if (ViewportClient)
			{
				bool bExistWindow = true;
				if (Subsystem->GetDebugMenuWidget().IsValid())
				{
					if (!Subsystem->GetDebugMenuWidget()->GetParentWidget().IsValid())
					{
						Subsystem->GetDebugMenuWidget().Reset();
						bExistWindow = false;
					}
				}
				else
				{
					bExistWindow = false;
				}

				if (!bExistWindow)
				{
					TSharedRef<SBTCoreDebugMenu> DebugView = SNew(SBTCoreDebugMenu, Subsystem);
					TSharedPtr<SBTCoreDebugMenu> DebugViewPtr = DebugView;
					Subsystem->SetDebugMenuWidget(DebugViewPtr);
					TSharedRef<SWindow> DebugWindow = SNew(SWindow)
					.Title(LOCTEXT("CreateWindow", "BeeTribe Debug Window"))
					.ClientSize(FVector2D(512.0f, 512.0f))
					[
						DebugView
					];

					FSlateApplication::Get().AddWindowAsNativeChild(DebugWindow, ViewportClient->GetWindow().ToSharedRef(), true);
				}
			}
#else
			if (ViewportClient)
			{
				if (Subsystem->GetDebugMenuWidget().IsValid())
				{
					ViewportClient->RemoveViewportWidgetContent(Subsystem->GetDebugMenuWidget().ToSharedRef());
				}
				else
				{
					TSharedRef<SBTCoreDebugMenu> DebugView = SNew(SBTCoreDebugMenu, Subsystem);
					TSharedPtr<SBTCoreDebugMenu> DebugViewPtr = DebugView;
					Subsystem->SetDebugMenuWidget(DebugViewPtr);

					ViewportClient->AddViewportWidgetContent(DebugView);
				}
			}
#endif
		}
	}
}

bool FBTCoreDebugManager::Initialize()
{
	return true;
}

void FBTCoreDebugManager::Finalize()
{
}

void FBTCoreDebugManager::Tick(float DeltaTime, bool bIsPaused)
{
	if (BeeTribe::Debug::ShowDebugCommandList)
	{
		UE_LOG(LogBTCore, Log, TEXT("デバッグコマンドリストを表示が有効です。"));
	}
}

#undef LOCTEXT_NAMESPACE

#endif