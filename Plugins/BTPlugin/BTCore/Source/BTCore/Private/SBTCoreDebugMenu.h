// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#if !(UE_BUILD_SHIPPING)

class SBTCoreDebugMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBTCoreDebugMenu){}
	SLATE_END_ARGS()

	~SBTCoreDebugMenu();

	void Construct(const FArguments& InArgs, TObjectPtr<class UBTCoreWorldSubsystem> InOwnerSubsystem);

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	bool HasFavoriteConsole(const FString& InConsoleName) const
	{
		return m_TempFavoriteConsoleList.Contains(InConsoleName);
	}

	bool HasRemovedFavoriteConsole(const FString& InConsoleName) const
	{
		return m_TempRemovedFavoriteList.Contains(InConsoleName);
	}

	bool AddUniqueFovoriteConsole(const FString& InConsoleName)
	{
		if (!HasFavoriteConsole(InConsoleName))
		{
			m_TempFavoriteConsoleList.Add(InConsoleName);
			return true;
		}

		return false;
	}

	bool AddUniqueRemovedFovoriteConsole(const FString& InConsoleName)
	{
		if (!HasRemovedFavoriteConsole(InConsoleName))
		{
			m_TempRemovedFavoriteList.Add(InConsoleName);
			return true;
		}

		return false;
	}

	bool RemoveUniqueRemovedFovoriteConsole(const FString& InConsoleName)
	{
		if (HasRemovedFavoriteConsole(InConsoleName))
		{
			m_TempRemovedFavoriteList.Remove(InConsoleName);
			return true;
		}

		return false;
	}

private:
	FReply ClickUpdateFavoriteList();
private:
	void AnimationScrollBar_OnUserScrolled(float ScrollOffset);

	float m_AnimationScrollBarPosition;

	TWeakObjectPtr<UWorld> m_World;
	TSet<FString> m_TempFavoriteConsoleList;
	TSet<FString> m_TempRemovedFavoriteList;
	TSharedPtr<SVerticalBox> m_FavoriteListBox;
};

#endif