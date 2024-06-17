// Copyright Epic Games, Inc. All Rights Reserved.

#include "SBTCoreDebugMenu.h"
#include "SlateOptMacros.h"
#include "Async/Async.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Misc/EnumRange.h"

#include "BTCorePrivate.h"
#include "BTCoreDebugTypes.h"
#include "BTCoreWorldSubsystem.h"
#include "BTCoreEngineSubsystem.h"
#include "BTCoreLocalSettings.h"

#define LOCTEXT_NAMESPACE "SBTCoreDebugMenu"

#if !(UE_BUILD_SHIPPING)

//BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

class SBTCoreDebugCommandBase : public SCompoundWidget
{
public:
	virtual ~SBTCoreDebugCommandBase() {}

protected:
	void ConstructInternal(TSharedPtr<SBTCoreDebugMenu> InDebugMenu, bool bFavoriteEnabled);
	virtual TSharedRef<SWidget> CreateValueWidget() = 0;

	bool GetIsFavoriteEnabled() const
	{
		bool Result = true;
		if (const UBTCoreLocalSettings* LocalSettings = GetDefault<UBTCoreLocalSettings>())
		{

			Result = !(LocalSettings->m_FavoriteConsoleList.Contains(m_Name));
		}

		if (Result)
		{
			if (m_DebugMenu.IsValid())
			{
				Result = !(m_DebugMenu.Pin()->HasFavoriteConsole(m_Name));
			}
		}

		return Result;
	}

	bool GetIsDiscardFavoriteEnabled() const
	{
		return !GetIsFavoriteEnabled();
	}

	virtual FReply ClickCommand() = 0;

	FReply ClickFavoriteCommand()
	{
		if (m_DebugMenu.IsValid())
		{
			m_DebugMenu.Pin()->AddUniqueFovoriteConsole(m_Name);
		}

		return FReply::Handled();
	}

	FReply ClickDiscardCommand()
	{
		if (m_DebugMenu.IsValid())
		{
			if (m_DebugMenu.Pin()->HasRemovedFavoriteConsole(m_Name))
			{
				m_DebugMenu.Pin()->RemoveUniqueRemovedFovoriteConsole(m_Name);
			}
			else
			{
				m_DebugMenu.Pin()->AddUniqueRemovedFovoriteConsole(m_Name);
			}
		}

		return FReply::Handled();
	}

	FSlateColor GetDiscardButtonForegroundColor() const
	{
		static const FName InvertedForegroundName("InvertedForeground");
		static const FName DefaultForegroundName("DefaultForeground");

		if (m_DebugMenu.IsValid())
		{
			if (m_DebugMenu.Pin()->HasRemovedFavoriteConsole(m_Name))
			{
				return FCoreStyle::Get().GetSlateColor(DefaultForegroundName);
			}

			return FCoreStyle::Get().GetSlateColor(InvertedForegroundName);
		}

		return FSlateColor::UseForeground();
	}

	FString m_Name;
	FString m_Help;

	TWeakPtr<SBTCoreDebugMenu> m_DebugMenu;
};

void SBTCoreDebugCommandBase::ConstructInternal(TSharedPtr<SBTCoreDebugMenu> InDebugMenu, bool bFavoriteEnabled)
{
	m_DebugMenu = InDebugMenu;

	EVisibility FavoriteVisibility = bFavoriteEnabled ? EVisibility::Visible : EVisibility::Hidden;

	const float WidgetWidth = 40.f;
	const float WidgetHeight = 40.f;

	const float MinButtonSize = 100.0f;
	const FMargin ButtonPadding(0.0f, 2.0f, 2.0f, 0.0f);

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SWrapBox)
			+ SWrapBox::Slot()
			.Padding(ButtonPadding)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.MinDesiredWidth(MinButtonSize)
					[
						SNew(SButton)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.Text(FText::FromString(m_Name))
						.ToolTipText(FText::FromString(m_Help))
						.OnClicked(this, &SBTCoreDebugCommandBase::ClickCommand)
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(WidgetWidth)
					.HeightOverride(WidgetHeight)
					[
						SNew(SButton)
						.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
						.ContentPadding(4.0f)
						.ForegroundColor(FSlateColor::UseForeground())
						.IsFocusable(false)
						.Visibility(FavoriteVisibility)
						.IsEnabled(this, &SBTCoreDebugCommandBase::GetIsFavoriteEnabled)
						.OnClicked(this, &SBTCoreDebugCommandBase::ClickFavoriteCommand)
						[
							SNew(SImage)
							.Image(FAppStyle::GetBrush(TEXT("Icons.Star")))
							.ColorAndOpacity(FSlateColor::UseForeground())
						]
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(WidgetWidth)
					.HeightOverride(WidgetHeight)
					[
						SNew(SButton)
						.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
						.ContentPadding(4.0f)
						.ForegroundColor(this, &SBTCoreDebugCommandBase::GetDiscardButtonForegroundColor)
						.IsFocusable(false)
						.Visibility(FavoriteVisibility)
						.IsEnabled(this, &SBTCoreDebugCommandBase::GetIsDiscardFavoriteEnabled)
						.OnClicked(this, &SBTCoreDebugCommandBase::ClickDiscardCommand)
						[
							SNew(SImage)
							.Image(FAppStyle::GetBrush(TEXT("Icons.Minus")))
							.ColorAndOpacity(FSlateColor::UseForeground())
						]
					]
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			CreateValueWidget()
		]
	];
}

class SBTCoreDebugBoolCommand : public SBTCoreDebugCommandBase
{
public:
	SLATE_BEGIN_ARGS(SBTCoreDebugBoolCommand) {}
	SLATE_END_ARGS()

	~SBTCoreDebugBoolCommand() {}

	void Construct(const FArguments& InArgs, TSharedPtr<SBTCoreDebugMenu> InDebugMenu, FBTCoreConsoleVariableRef* InConsoleVariable, bool bFavoriteEnabled = true);
protected:
	virtual TSharedRef<SWidget> CreateValueWidget() override
	{
		TSharedRef<SWidget> Widget =
			SNew(SCheckBox)
			.IsChecked(this, &SBTCoreDebugBoolCommand::IsChecked)
			.OnCheckStateChanged(this, &SBTCoreDebugBoolCommand::OnToggleChanged);
		return Widget;
	}
private:
	ECheckBoxState IsOwnerChecked() const
	{
		if (m_OwnerPtr)
		{
			bool Result = (*m_OwnerPtr)->GetBool();
			return Result ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
		}
		else
		{
			return ECheckBoxState::Unchecked;
		}
	}

	ECheckBoxState IsChecked() const
	{
		return m_CheckState;
	}

	void OnToggleChanged(ECheckBoxState NewState)
	{
		if (NewState != m_CheckState)
		{
			m_CheckState = NewState;
		}
	}

	virtual FReply ClickCommand() override
	{
		if (GEngine)
		{
			int Value = m_CheckState == ECheckBoxState::Checked ? 1 : 0;
			FString Result = FString::Printf(TEXT("%s %d"), *m_Name, Value);
			GEngine->Exec(nullptr, *Result);
			UE_LOG(LogBTCore, Log, TEXT("%s"), *Result);
		}
		else
		{
			UE_LOG(LogBTCore, Warning, TEXT("SBTCoreDebugBoolCommand::ClickCommand : %s"), *m_Name);
		}

		return FReply::Handled();
	}

	ECheckBoxState m_CheckState = ECheckBoxState::Undetermined;
	FBTCoreConsoleVariableRef* m_OwnerPtr = nullptr;
};

void SBTCoreDebugBoolCommand::Construct(const FArguments& InArgs, TSharedPtr<SBTCoreDebugMenu> InDebugMenu, FBTCoreConsoleVariableRef* InConsoleVariable, bool bFavoriteEnabled)
{
	check(InConsoleVariable);

	m_Name = InConsoleVariable->m_Name;
	m_Help = InConsoleVariable->m_Help;

	m_OwnerPtr = InConsoleVariable;

	m_CheckState = IsOwnerChecked();

	ConstructInternal(InDebugMenu, bFavoriteEnabled);
}

class SBTCoreDebugIntCommand : public SBTCoreDebugCommandBase
{
public:
	SLATE_BEGIN_ARGS(SBTCoreDebugIntCommand) {}
	SLATE_END_ARGS()

	~SBTCoreDebugIntCommand() {}

	void Construct(const FArguments& InArgs, TSharedPtr<SBTCoreDebugMenu> InDebugMenu, FBTCoreConsoleVariableRef* InConsoleVariable, bool bFavoriteEnabled = true);
protected:
	virtual TSharedRef<SWidget> CreateValueWidget() override
	{
		TSharedRef<SWidget> Widget =
			SAssignNew(m_ValueWidget, SSpinBox<int>)
			.PreventThrottling(true)
			.MinValue(m_MinValue)
			.MaxValue(m_MaxValue)
			.MinSliderValue(m_MinValue)
			.MaxSliderValue(m_MaxValue)
			.SliderExponent(1)
			.Delta(0.005f)
			.MinDesiredWidth(100.0f)
			.SupportDynamicSliderMinValue(true)
			.SupportDynamicSliderMaxValue(true);
		return Widget;
	}
protected:
	virtual FReply ClickCommand() override
	{
		if (GEngine)
		{
			FString Result = FString::Printf(TEXT("%s %d"), *m_Name, m_ValueWidget->GetValue());
			GEngine->Exec(nullptr, *Result);
			UE_LOG(LogBTCore, Log, TEXT("%s"), *Result);
		}
		else
		{
			UE_LOG(LogBTCore, Warning, TEXT("SBTCoreDebugIntCommand::ClickCommand : %s"), *m_Name);
		}

		return FReply::Handled();
	}

	int32 m_MinValue;
	int32 m_MaxValue;

	TSharedPtr<SSpinBox<int>> m_ValueWidget;
};

void SBTCoreDebugIntCommand::Construct(const FArguments& InArgs, TSharedPtr<SBTCoreDebugMenu> InDebugMenu, FBTCoreConsoleVariableRef* InConsoleVariable, bool bFavoriteEnabled)
{
	check(InConsoleVariable);

	m_Name = InConsoleVariable->m_Name;
	m_Help = InConsoleVariable->m_Help;
	m_MinValue = InConsoleVariable->m_MinValue.iValue;
	m_MaxValue = InConsoleVariable->m_MaxValue.iValue;

	ConstructInternal(InDebugMenu, bFavoriteEnabled);
}

class SBTCoreDebugFloatCommand : public SBTCoreDebugCommandBase
{
public:
	SLATE_BEGIN_ARGS(SBTCoreDebugFloatCommand) {}
	SLATE_END_ARGS()

	~SBTCoreDebugFloatCommand() {}

	void Construct(const FArguments& InArgs, TSharedPtr<SBTCoreDebugMenu> InDebugMenu, FBTCoreConsoleVariableRef* InConsoleVariable, bool bFavoriteEnabled = true);
protected:
	virtual TSharedRef<SWidget> CreateValueWidget() override
	{
		TSharedRef<SWidget> Widget =
			SAssignNew(m_ValueWidget, SSpinBox<float>)
			.PreventThrottling(true)
			.MinValue(m_MinValue)
			.MaxValue(m_MaxValue)
			.MinSliderValue(m_MinValue)
			.MaxSliderValue(m_MaxValue)
			.SliderExponent(1)
			.Delta(0.005f)
			.MinDesiredWidth(100.0f)
			.SupportDynamicSliderMinValue(true)
			.SupportDynamicSliderMaxValue(true);
		return Widget;
	}
protected:

	virtual FReply ClickCommand() override
	{
		if (GEngine)
		{
			FString Result = FString::Printf(TEXT("%s %f"), *m_Name, m_ValueWidget->GetValue());
			GEngine->Exec(nullptr, *Result);
			UE_LOG(LogBTCore, Log, TEXT("%s"), *Result);
		}
		else
		{
			UE_LOG(LogBTCore, Warning, TEXT("SBTCoreDebugFloatCommand::ClickCommand : %s"), *m_Name);
		}

		return FReply::Handled();
	}

	float m_MinValue;
	float m_MaxValue;

	TSharedPtr<SSpinBox<float>> m_ValueWidget;

};

void SBTCoreDebugFloatCommand::Construct(const FArguments& InArgs, TSharedPtr<SBTCoreDebugMenu> InDebugMenu, FBTCoreConsoleVariableRef* InConsoleVariable, bool bFavoriteEnabled)
{
	check(InConsoleVariable);

	m_Name = InConsoleVariable->m_Name;
	m_Help = InConsoleVariable->m_Help;
	m_MinValue = InConsoleVariable->m_MinValue.fValue;
	m_MaxValue = InConsoleVariable->m_MaxValue.fValue;

	ConstructInternal(InDebugMenu, bFavoriteEnabled);
}

class SBTCoreDebugWorldAndArgs : public SBTCoreDebugCommandBase
{
public:
	SLATE_BEGIN_ARGS(SBTCoreDebugWorldAndArgs) {}
	SLATE_END_ARGS()

	~SBTCoreDebugWorldAndArgs() {}

	void Construct(const FArguments& InArgs, TObjectPtr<UWorld> InWorld, TSharedPtr<SBTCoreDebugMenu> InDebugMenu, FBTCoreConsoleWithWorldAndArgs* InConsoleWorldAndArgs, bool bFavoriteEnabled = true);
protected:
	virtual TSharedRef<SWidget> CreateValueWidget() override
	{
		TSharedRef<SWidget> Widget =
			SAssignNew(m_ValueWidget, SEditableTextBox);
		return Widget;
	}
protected:
	virtual FReply ClickCommand() override
	{
		if (GEngine)
		{
			FString Result = FString::Printf(TEXT("%s %s"), *m_Name, *m_ValueWidget->GetText().ToString());
			GEngine->Exec(m_World.Get(), *Result);
			UE_LOG(LogBTCore, Log, TEXT("%s"), *Result);
		}
		else
		{
			UE_LOG(LogBTCore, Warning, TEXT("SBTCoreDebugWorldAndArgs::ClickCommand : %s"), *m_Name);
		}

		return FReply::Handled();
	}

	TWeakObjectPtr<UWorld> m_World;
	TSharedPtr<SEditableTextBox> m_ValueWidget;

};

void SBTCoreDebugWorldAndArgs::Construct(const FArguments& InArgs, TObjectPtr<UWorld> InWorld, TSharedPtr<SBTCoreDebugMenu> InDebugMenu, FBTCoreConsoleWithWorldAndArgs* InConsoleWorldAndArgs, bool bFavoriteEnabled)
{
	check(InConsoleWorldAndArgs);

	m_Name = InConsoleWorldAndArgs->m_Name;
	m_Help = InConsoleWorldAndArgs->m_Help;

	m_World = InWorld;
	ConstructInternal(InDebugMenu, bFavoriteEnabled);
}

SBTCoreDebugMenu::~SBTCoreDebugMenu()
{
}

void SBTCoreDebugMenu::Construct(const FArguments& InArgs, TObjectPtr<UBTCoreWorldSubsystem> InOwnerSubsystem)
{
	check(InOwnerSubsystem->GetWorld());

	m_World = InOwnerSubsystem->GetWorld();
	TSharedRef<SVerticalBox> ConsoleCommandListBox = SNew(SVerticalBox);
	TSharedRef<SVerticalBox> FavoriteConsoleCommandListBox = SAssignNew(m_FavoriteListBox, SVerticalBox); 
	TArray<FString> ConsoleCommandNames;
	TArray<FString> ConsoleWorldAndArgsNames;

	m_TempFavoriteConsoleList.Reset();
	m_TempRemovedFavoriteList.Reset();

	typedef union compare_value
	{
		FBTCoreConsoleVariableRef* VariablePtr;
		FBTCoreConsoleWithWorldAndArgs* WorldAndArgsPtr;
	} FCompareValue;

	if (UBTCoreEngineSubsystem* Subsystem = GEngine ? GEngine->GetEngineSubsystem<UBTCoreEngineSubsystem>() : nullptr)
	{
		Subsystem->GetConsoleCommandNames(ConsoleCommandNames);
		Subsystem->GetConsoleWithWorldAndArgsNames(ConsoleWorldAndArgsNames);

		const float MinButtonSize = 100.0f;
		const FMargin ButtonPadding(0.0f, 2.0f, 2.0f, 0.0f);

		TArray<FString> FavoriteConsoleList;
		TArray<FString> FavoriteConsoleVariableList;
		TArray<FString> FavoriteConsoleWorldAndArgsList;
		if (const UBTCoreLocalSettings* LocalSettings = GetDefault<UBTCoreLocalSettings>())
		{
			FavoriteConsoleList.Append(LocalSettings->m_FavoriteConsoleList);
			m_TempFavoriteConsoleList.Append(LocalSettings->m_FavoriteConsoleList);
		}

		for (const FString& Name : FavoriteConsoleList)
		{
			if (ConsoleCommandNames.Contains(Name))
			{
				FavoriteConsoleVariableList.Add(Name);
			}
			else
			{
				FavoriteConsoleWorldAndArgsList.Add(Name);
			}
		}

		TArray<FCompareValue> ConsoleVariableRefs;
		ConsoleVariableRefs.Reserve(ConsoleCommandNames.Num());
		for (const FString& ConsoleCommandName : ConsoleCommandNames)
		{
			FBTCoreConsoleVariableRef* ConsoleVariableRef = Subsystem->GetConsoleVariableRef(ConsoleCommandName);
			if (ConsoleVariableRef == nullptr || ConsoleVariableRef->m_Priority <= IBTCoreConsoleManager::INVISIBILITY_PRIORITY)
			{
				continue;
			}

			FCompareValue Value;
			Value.VariablePtr = ConsoleVariableRef;

			ConsoleVariableRefs.Add(Value);
		}

		for (const FString& Name : FavoriteConsoleVariableList)
		{
			FBTCoreConsoleVariableRef* ConsoleVariableRef = Subsystem->GetConsoleVariableRef(Name);
			if (ConsoleVariableRef == nullptr || ConsoleVariableRef->m_Priority <= IBTCoreConsoleManager::INVISIBILITY_PRIORITY)
			{
				continue;
			}

			if (ConsoleVariableRef->m_Type == FBTCoreConsoleVariableRef::ConsoleVariableType::Bool)
			{
				FavoriteConsoleCommandListBox->AddSlot()
					.AutoHeight()
					[
						SNew(SBTCoreDebugBoolCommand, SharedThis(this), ConsoleVariableRef, false)
					];
			}
			else if (ConsoleVariableRef->m_Type == FBTCoreConsoleVariableRef::ConsoleVariableType::Int)
			{
				FavoriteConsoleCommandListBox->AddSlot()
					.AutoHeight()
					[
						SNew(SBTCoreDebugIntCommand, SharedThis(this), ConsoleVariableRef, false)
					];
			}
			else if (ConsoleVariableRef->m_Type == FBTCoreConsoleVariableRef::ConsoleVariableType::Float)
			{
				FavoriteConsoleCommandListBox->AddSlot()
					.AutoHeight()
					[
						SNew(SBTCoreDebugFloatCommand, SharedThis(this), ConsoleVariableRef, false)
					];
			}
		}

		for (const FString& Name : FavoriteConsoleWorldAndArgsList)
		{
			FBTCoreConsoleWithWorldAndArgs* ConsoleWorldAndArgs = Subsystem->GetConsoleWithWorldAndArgs(Name);
			if (ConsoleWorldAndArgs == nullptr || ConsoleWorldAndArgs->m_Priority <= IBTCoreConsoleManager::INVISIBILITY_PRIORITY)
			{
				continue;
			}

			FavoriteConsoleCommandListBox->AddSlot()
			.AutoHeight()
			[
				SNew(SBTCoreDebugWorldAndArgs, InOwnerSubsystem->GetWorld(), SharedThis(this), ConsoleWorldAndArgs, false)
			];
		}

		ConsoleVariableRefs.Sort([](const FCompareValue& A, const FCompareValue& B) {
			return A.VariablePtr->m_Priority > B.VariablePtr->m_Priority;
			});

		for (FCompareValue& Value : ConsoleVariableRefs)
		{
			if (Value.VariablePtr->m_Type == FBTCoreConsoleVariableRef::ConsoleVariableType::Bool)
			{
				ConsoleCommandListBox->AddSlot()
					.AutoHeight()
					[
						SNew(SBTCoreDebugBoolCommand, SharedThis(this), Value.VariablePtr)
					];
			}
			else if (Value.VariablePtr->m_Type == FBTCoreConsoleVariableRef::ConsoleVariableType::Int)
			{
				ConsoleCommandListBox->AddSlot()
					.AutoHeight()
					[
						SNew(SBTCoreDebugIntCommand, SharedThis(this), Value.VariablePtr)
					];
			}
			else if (Value.VariablePtr->m_Type == FBTCoreConsoleVariableRef::ConsoleVariableType::Float)
			{
				ConsoleCommandListBox->AddSlot()
					.AutoHeight()
					[
						SNew(SBTCoreDebugFloatCommand, SharedThis(this), Value.VariablePtr)
					];
			}
		}

		TArray<FCompareValue> ConsoleWorldAndArgsList;
		ConsoleWorldAndArgsList.Reserve(ConsoleWorldAndArgsNames.Num());
		for (const FString& ConsoleWorldAndArgsName : ConsoleWorldAndArgsNames)
		{
			FBTCoreConsoleWithWorldAndArgs* ConsoleWorldAndArgs = Subsystem->GetConsoleWithWorldAndArgs(ConsoleWorldAndArgsName);
			if (ConsoleWorldAndArgs == nullptr || ConsoleWorldAndArgs->m_Priority <= IBTCoreConsoleManager::INVISIBILITY_PRIORITY)
			{
				continue;
			}

			FCompareValue Value;
			Value.WorldAndArgsPtr = ConsoleWorldAndArgs;
			ConsoleWorldAndArgsList.Add(Value);
		}

		ConsoleWorldAndArgsList.Sort([](const FCompareValue& A, const FCompareValue& B) {
			return A.WorldAndArgsPtr->m_Priority > B.WorldAndArgsPtr->m_Priority;
			});

		for (FCompareValue& Value : ConsoleWorldAndArgsList)
		{
			ConsoleCommandListBox->AddSlot()
			.AutoHeight()
			[
				SNew(SBTCoreDebugWorldAndArgs, InOwnerSubsystem->GetWorld(), SharedThis(this), Value.WorldAndArgsPtr)
			];
		}
	}

	ChildSlot
	[
		SNew(SBorder)
		.Padding(FMargin(37.5, 37, 38.5, 37.5))
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SScrollBox)
			.Orientation(Orient_Vertical)
			.ScrollBarAlwaysVisible(true)
			.OnUserScrolled(this, &SBTCoreDebugMenu::AnimationScrollBar_OnUserScrolled)
			+ SScrollBox::Slot()
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					[
						SNew(SButton)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.Text(FText::FromString(TEXT("お気に入りを更新")))
						.OnClicked(this, &SBTCoreDebugMenu::ClickUpdateFavoriteList)
					]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					[
						SNew(STextBlock)
						.Justification(ETextJustify::Center)
						.Text(FText::FromString(TEXT("お気に入り")))
					]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					FavoriteConsoleCommandListBox
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					[
						SNew(STextBlock)
						.Justification(ETextJustify::Center)
						.Text(FText::FromString(TEXT("コマンドリスト")))
					]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					ConsoleCommandListBox
				]
			]
		]
	];
}

FReply SBTCoreDebugMenu::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	return FReply::Handled();
}
void SBTCoreDebugMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{}

FReply SBTCoreDebugMenu::ClickUpdateFavoriteList()
{
	TArray<FString> ConsoleCommandNames;
	TArray<FString> ConsoleWorldAndArgsNames;

	if (UBTCoreEngineSubsystem* Subsystem = GEngine ? GEngine->GetEngineSubsystem<UBTCoreEngineSubsystem>() : nullptr)
	{
		Subsystem->GetConsoleCommandNames(ConsoleCommandNames);
		Subsystem->GetConsoleWithWorldAndArgsNames(ConsoleWorldAndArgsNames);

		TArray<FString> AddConsoleNames;

		if (UBTCoreLocalSettings* LocalSettings = GetMutableDefault<UBTCoreLocalSettings>())
		{
			TSet<FString> UniqueFavoriteNames(LocalSettings->m_FavoriteConsoleList);
			UniqueFavoriteNames.Append(m_TempFavoriteConsoleList);

			bool IsClearList = false;
			for (const FString& Name : m_TempRemovedFavoriteList)
			{
				if (UniqueFavoriteNames.Contains(Name))
				{
					IsClearList = true;
				}
			}

			for (const FString& Name : m_TempRemovedFavoriteList)
			{
				UniqueFavoriteNames.Remove(Name);
				if (LocalSettings->m_FavoriteConsoleList.Contains(Name))
				{
					LocalSettings->m_FavoriteConsoleList.Remove(Name);
				}
			}

			if (IsClearList)
			{
				m_FavoriteListBox->ClearChildren();
			}
			else
			{
				for (const FString& Name : LocalSettings->m_FavoriteConsoleList)
				{
					if (UniqueFavoriteNames.Contains(Name))
					{
						UniqueFavoriteNames.Remove(Name);
					}
				}
			}

			for (const FString& Name : UniqueFavoriteNames)
			{
				AddConsoleNames.Add(Name);
				if (!LocalSettings->m_FavoriteConsoleList.Contains(Name))
				{
					LocalSettings->m_FavoriteConsoleList.Add(Name);
				}

				if (ConsoleCommandNames.Contains(Name))
				{
					FBTCoreConsoleVariableRef* ConsoleVariableRef = Subsystem->GetConsoleVariableRef(Name);
					if (ConsoleVariableRef == nullptr || ConsoleVariableRef->m_Priority <= IBTCoreConsoleManager::INVISIBILITY_PRIORITY)
					{
						continue;
					}

					if (ConsoleVariableRef->m_Type == FBTCoreConsoleVariableRef::ConsoleVariableType::Bool)
					{
						m_FavoriteListBox->AddSlot()
						.AutoHeight()
						[
							SNew(SBTCoreDebugBoolCommand, SharedThis(this), ConsoleVariableRef, false)
						];
					}
					else if (ConsoleVariableRef->m_Type == FBTCoreConsoleVariableRef::ConsoleVariableType::Int)
					{
						m_FavoriteListBox->AddSlot()
						.AutoHeight()
						[
							SNew(SBTCoreDebugIntCommand, SharedThis(this), ConsoleVariableRef, false)
						];
					}
					else if (ConsoleVariableRef->m_Type == FBTCoreConsoleVariableRef::ConsoleVariableType::Float)
					{
						m_FavoriteListBox->AddSlot()
						.AutoHeight()
						[
							SNew(SBTCoreDebugFloatCommand, SharedThis(this), ConsoleVariableRef, false)
						];
					}
				}
				else if (ConsoleWorldAndArgsNames.Contains(Name))
				{
					FBTCoreConsoleWithWorldAndArgs* ConsoleWorldAndArgs = Subsystem->GetConsoleWithWorldAndArgs(Name);
					if (ConsoleWorldAndArgs == nullptr || ConsoleWorldAndArgs->m_Priority <= IBTCoreConsoleManager::INVISIBILITY_PRIORITY)
					{
						continue;
					}

					m_FavoriteListBox->AddSlot()
					.AutoHeight()
					[
						SNew(SBTCoreDebugWorldAndArgs, m_World.Get(), SharedThis(this), ConsoleWorldAndArgs, false)
					];
				}
			}
		}
	}

	m_TempFavoriteConsoleList.Reset();
	m_TempRemovedFavoriteList.Reset();

	return FReply::Handled();
}

void SBTCoreDebugMenu::AnimationScrollBar_OnUserScrolled(float ScrollOffset)
{
	m_AnimationScrollBarPosition = ScrollOffset;
}

#endif

#undef LOCTEXT_NAMESPACE