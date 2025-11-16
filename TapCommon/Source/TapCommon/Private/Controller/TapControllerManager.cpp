// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/TapControllerManager.h"

#include "TapCommon.h"
#include "TapUECommon.h"
#include "TUType.h"
#include "Blueprint/UserWidget.h"
#include "Controller/TapControllerTip.h"
#include "Controller/Input/TapControllerInput.h"
#include "Framework/Application/NavigationConfig.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"
#include "Framework/Application/SlateUser.h"
#include "Widgets/SViewport.h"

void SetupTapNavigation(TSharedRef<SWidget> InWidget, TSharedPtr<SWidget> Up, TSharedPtr<SWidget> Down, TSharedPtr<SWidget> Left, TSharedPtr<SWidget> Right, TSharedPtr<SWidget> Next,
                        TSharedPtr<SWidget> Previous)
{
	TSharedPtr<FNavigationMetaData> MetaData = InWidget->GetMetaData<FNavigationMetaData>();
	if ( !MetaData.IsValid() )
	{
		MetaData = MakeShared<FNavigationMetaData>();
		InWidget->AddMetadata(MetaData.ToSharedRef());
	}

	MetaData->SetNavigationExplicit(EUINavigation::Left, Left);
	MetaData->SetNavigationExplicit(EUINavigation::Right, Right);
	MetaData->SetNavigationExplicit(EUINavigation::Up, Up);
	MetaData->SetNavigationExplicit(EUINavigation::Down, Down);
	MetaData->SetNavigationExplicit(EUINavigation::Next, Next);
	MetaData->SetNavigationExplicit(EUINavigation::Previous, Previous);
}

void SetupTapNavigation(TSharedRef<SWidget> InWidget, TSharedPtr<SWidget> All)
{
	SetupTapNavigation(InWidget, All, All, All, All, All, All);
}

FTapControllerWidgetData::FTapControllerWidgetData(UUserWidget* InWidget)
	: KeyWidget(InWidget)
	, RootWidget(InWidget->GetCachedWidget())
{
	ensure(RootWidget.IsValid());
}

FTapControllerWidgetData::FTapControllerWidgetData(TSharedRef<SWidget> InWidget)
	: KeyWidget(&InWidget.Get())
	, RootWidget(InWidget)
{
	
}

FTapControllerManager::FTapControllerManager()
{
	NavConfig = MakeShared<FNavigationConfig>();

	AHUD::OnShowDebugInfo.AddRaw(this, &FTapControllerManager::ShowDebugInfo);
}

FTapControllerManager::~FTapControllerManager()
{
	AHUD::OnShowDebugInfo.RemoveAll(this);
}

FTapControllerManager& FTapControllerManager::Get()
{
	static FTapControllerManager* Mng = nullptr;
	if (!Mng)
	{
		Mng = new FTapControllerManager;
	}
	check(Mng);
	return *Mng;
}

TSharedPtr<STapControllerTip> FTapControllerManager::RegisterWidget(const TSharedRef<FTapControllerWidgetData>& NewWidgetData)
{
	UE_LOG(LogTap, Log, TEXT("RegisterWidget - ControllerTip: %s, StartupFocusWidget: %s-%s, ShowBack: %s"),
		NewWidgetData->ControllerTip.IsValid() ? TEXT("Valid") : TEXT("Invalid"),
		NewWidgetData->StartupFocusWidget.IsSet() ? TEXT("Set") : TEXT("NotSet"),
		NewWidgetData->StartupFocusWidget.IsBound() ? TEXT("Bind") : TEXT("Notbind"),
		NewWidgetData->ShowBackIcon() ? TEXT("Show") : TEXT("NotShow"));
	ensure(NewWidgetData->KeyWidget);
	ensure(NewWidgetData->RootWidget.IsValid());

	if (RegisteredWidgets.ContainsByPredicate([&NewWidgetData](TSharedRef<FTapControllerWidgetData> EachData)
	{
		return EachData->KeyWidget == NewWidgetData->KeyWidget;
	}))
	{
		UE_LOG(LogTap, Warning, TEXT("Widget exists. return null."));
		return nullptr;
	}
	
	if (!NewWidgetData->ControllerTip)
	{
		NewWidgetData->ControllerTip = SNew(STapControllerTip);
	}
	
	RegisteredWidgets.Push(NewWidgetData);
	if (IsAlreadyStarted())
	{
		UpdateRunningState(true);
#if ENGINE_MAJOR_VERSION >= 5
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float)
#else
		FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float)
#endif
		{
			UE_LOG(LogTap, Log, TEXT("Delay refresh focus widget."));
			if (!IsAlreadyStarted())
			{
				UE_LOG(LogTap, Log, TEXT("Tap controller is stopped."));
				return false;
			}
			TSharedPtr<FTapControllerWidgetData> Data = FindReceiveInputWidgetData();
			FWidgetPath FocusPath;
			if (TSharedPtr<SWidget> FocusWidget = FSlateApplication::Get().GetKeyboardFocusedWidget())
			{
				FSlateApplication::Get().FindPathToWidget(FocusWidget.ToSharedRef(), FocusPath);
			}
#if ENGINE_MAJOR_VERSION >= 5
			if (!Data->RootWidget.IsValid() || !FocusPath.ContainsWidget(Data->RootWidget.Pin().Get()))
#else
			if (!Data->RootWidget.IsValid() || !FocusPath.ContainsWidget(Data->RootWidget.Pin().ToSharedRef()))
#endif			
			{
				FocusToStartupWidget();
				RefreshControllerTips();
			}
			return false;
		}));	
	}

	RefreshControllerTips();
	
	return NewWidgetData->ControllerTip.ToSharedRef();
}

void FTapControllerManager::UnregisterWidget(void* InKeyWidget)
{
	int32 Index = RegisteredWidgets.IndexOfByPredicate([InKeyWidget](const TSharedRef<FTapControllerWidgetData>& EachData){ return EachData->KeyWidget == InKeyWidget; });
	UE_LOG(LogTap, Log, TEXT("UnregisterWidget - Index: %d"), Index);
	if (Index != INDEX_NONE)
	{
		RegisteredWidgets.RemoveAt(Index);

		if (IsAlreadyStarted())
		{
			if (!FindReceiveInputWidgetData().IsValid())
			{
				UpdateRunningState(false);
			}
			else
			{
#if (ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 0)
				FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float)
#else
				FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float)
#endif
				{
					UE_LOG(LogTap, Log, TEXT("Delay refresh focus widget."));
					if (IsAlreadyStarted() && FindReceiveInputWidgetData().IsValid())
					{
						if (!FSlateApplication::Get().GetKeyboardFocusedWidget().IsValid())
						{
							FocusToStartupWidget();
						}
						RefreshControllerTips();
					}
					return false;
				}));		
			}
		}
	}
}

bool FTapControllerManager::Startup(ETapControllerType InControllerType)
{
	UE_LOG(LogTap, Log, TEXT("Tap controller manager start-up. %s"), *UEnum::GetValueAsString(InControllerType));

	if (IsAlreadyStarted())
	{
		UE_LOG(LogTap, Warning, TEXT("Already started-up."));
		return false;
	}

	ControllerType = InControllerType;
	OnTapControllerTypeChanged().Broadcast(ControllerType);

	if (FindReceiveInputWidgetData())
	{
		UpdateRunningState(true);
	}
	return true;
}

void FTapControllerManager::Shutdown()
{
	UE_LOG(LogTap, Log, TEXT("Tap controller manager shut-down ."));

	ControllerType.Reset();
	OnTapControllerTypeChanged().Broadcast(ControllerType);
	UpdateRunningState(false);
}

void FTapControllerManager::UpdateConfig(TSharedPtr<FTapControllerConfig> InConfig)
{
	Config = InConfig;
}

bool FTapControllerManager::IsAlreadyStarted() const
{
	return ControllerType.IsSet();
}

TOptional<ETapControllerType> FTapControllerManager::GetControllerType() const
{
	return ControllerType;
}

FOnTapControllerTypeChanged& FTapControllerManager::OnTapControllerTypeChanged()
{
	return ControllerTypeChanged;
}

bool FTapControllerManager::IsTapControllerRunning() const
{
	return ControllerInput.IsValid();
}

bool FTapControllerManager::UpdateRunningState(bool bNewRunning)
{
	if (IsTapControllerRunning() != bNewRunning)
	{
		TapUECommon::PreTapControllerRunningStateChange.Broadcast(bNewRunning);
		if (bNewRunning)
		{
			UE_LOG(LogTap, Log, TEXT("Tap controller manager run."))

			FSlateApplication::Get().OnFocusChanging().AddRaw(this, &FTapControllerManager::OnFocusChanged);

			ControllerInput = MakeShared<FTapControllerInput>();
			ControllerInput->SetDeadZone(GetConfig().DeadZone);
			ControllerInput->SetMaxSpeed(GetConfig().CursorMaxSpeed);
			ControllerInput->ScrollSpeed = GetConfig().ScrollMaxSpeed;
			
			if (!FSlateApplication::Get().RegisterInputPreProcessor(ControllerInput))
			{
				UE_LOG(LogTap, Warning, TEXT("Register controller input failed."));
			}
			
			if (TSharedPtr<FNavigationConfig> Nav = GetNavigationConfig())
			{
				if (TSharedPtr<FSlateUser> User = FSlateApplication::Get().GetUser(0))
				{
					User->SetUserNavigationConfig(Nav.ToSharedRef());
				}
			}
			ChangeInputModeFocus();
			if (const FSlateBrush* Brush = FTapCommonModule::Get().Style->GetBrush("/Controller/DefaultFocusBrush"))
			{
#if ENGINE_MAJOR_VERSION >= 5
				const_cast<FSlateStyleSet&>(static_cast<const FSlateStyleSet&>(FAppStyle::Get())).Set<FSlateBrush>("FocusRectangle", const_cast<FSlateBrush*>(Brush));
#else
				FCoreStyle::SetFocusBrush(const_cast<FSlateBrush*>(Brush));
#endif
			}
			else
			{
				ensure(false);
			}
			
		}
		else
		{
			UE_LOG(LogTap, Log, TEXT("Tap controller manager stop."));

			FSlateApplication::Get().OnFocusChanging().RemoveAll(this);

			CanScrollWidget.Reset();
			CursorInstigatorWidget.Reset();
			bCanScroll = false;
			
			FSlateApplication::Get().UnregisterInputPreProcessor(ControllerInput);
			ControllerInput.Reset();
		}
		TapUECommon::PostTapControllerRunningStateChanged.Broadcast(bNewRunning);
		return true;
	}
	return false;
}

void FTapControllerManager::ChangeInputModeCursor(TSharedPtr<SWidget> InstigatorWidget)
{
	UE_LOG(LogTap, Log, TEXT("Tap controller manager Input: Cursor."));
	ensure(InstigatorWidget);
	
	CursorInstigatorWidget = InstigatorWidget;
	InputMode = ETapInputMode::Cursor;
	
	TSharedPtr<FSlateUser> User = FSlateApplication::Get().GetUser(0);
	if (User)
	{
		User->SetCursorVisibility(true);
		const FGeometry& Geometry = InstigatorWidget->GetCachedGeometry();
		User->SetCursorPosition(Geometry.GetAbsolutePosition() + (Geometry.GetAbsoluteSize() / 2.f));
	}
}

void FTapControllerManager::ChangeInputModeFocus()
{
	UE_LOG(LogTap, Log, TEXT("Tap controller manager Input: Focus."));

	TSharedPtr<FSlateUser> User = FSlateApplication::Get().GetUser(0);
	if (User)
	{
		User->SetCursorVisibility(false);
	}
	CursorInstigatorWidget.Reset();
	
	InputMode = ETapInputMode::Focus;
}

TSharedPtr<FTapControllerWidgetData> FTapControllerManager::FindReceiveInputWidgetData() const
{
	return RegisteredWidgets.Num() > 0 ? RegisteredWidgets.Last() : TSharedPtr<FTapControllerWidgetData>();
}

void FTapControllerManager::FocusToStartupWidget()
{
	UE_LOG(LogTap, Log, TEXT("Tap controller manager Focus to startup widget."));
	ensure(IsTapControllerRunning());
	if (TSharedPtr<FTapControllerWidgetData> Data = FindReceiveInputWidgetData())
	{
		if (Data->StartupFocusWidget.Get() == FSlateApplication::Get().GetKeyboardFocusedWidget())
		{
			UE_LOG(LogTap, Log, TEXT("Clear focus."));
			FSlateApplication::Get().ClearKeyboardFocus();
		}
		if (FSlateApplication::Get().SetKeyboardFocus(Data->StartupFocusWidget.Get(), EFocusCause::Navigation))
		{
			UE_LOG(LogTap, Log, TEXT("Tap controller manager Focus to startup widget: %s"), Data->StartupFocusWidget.Get() ? *Data->StartupFocusWidget.Get()->ToString() : TEXT("None"));
		}
	}
}

TSharedPtr<FNavigationConfig> FTapControllerManager::GetNavigationConfig() const
{
	return NavConfig;
}

void FTapControllerManager::RefreshControllerTips()
{
	if (TSharedPtr<FTapControllerWidgetData> Data = FindReceiveInputWidgetData())
	{
		for (int32 i = 0; i < RegisteredWidgets.Num(); ++i)
		{
			if (RegisteredWidgets[i]->ControllerTip)
			{
				RegisteredWidgets[i]->ControllerTip->SetVisibility(RegisteredWidgets[i] == Data ? EVisibility::HitTestInvisible : EVisibility::Hidden);
			}
		}

		if (Data->ControllerTip)
		{
			ETapControllerTipIconType Types = ETapControllerTipIconType::Accept;

			if(Data->ShowBackIcon())
			{
				Types |= ETapControllerTipIconType::Back;
			}
		
			TSharedPtr<SWidget> FocusWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();
			if(bCanScroll)
			{
				Types |= ETapControllerTipIconType::Scroll;
			}
			Data->ControllerTip->UpdateTipIconTypes(Types);
		}
	}
}



void FTapControllerManager::OnFocusChanged(const FFocusEvent& Event, const FWeakWidgetPath& OldPath, const TSharedPtr<SWidget>& OldWidget, const FWidgetPath& NewPath,
                                           const TSharedPtr<SWidget>& NewWidget)
{
	UE_LOG(LogTap, Log, TEXT("OnFocusChanged."));
	ensure(IsTapControllerRunning());

	if (NewWidget)
	{
		
		if (PathContainsWebBrowser(NewPath))
		{
			if (GetInputMode() != ETapInputMode::Cursor)
			{
				ChangeInputModeCursor(NewWidget);
			}
		}
		else
		{
			if (GetInputMode() != ETapInputMode::Focus)
			{
				ChangeInputModeFocus();
			}
		}
	}

	TSharedPtr<SWidget> ScrollWidget = FindCanScrollWidget(NewPath);
	bool bNewScroll = ScrollWidget.IsValid();
	if (bNewScroll != bCanScroll)
	{
		CanScrollWidget = ScrollWidget;
		bCanScroll = bNewScroll;
		RefreshControllerTips();
	}
}

TSharedPtr<SWidget> FTapControllerManager::FindCanScrollWidget(const FWidgetPath& InWidgetPath)
{
	for (int32 WidgetIndex = InWidgetPath.Widgets.Num() - 1; WidgetIndex >= 0; --WidgetIndex)
	{
		FName TypeName = InWidgetPath.Widgets[WidgetIndex].Widget->GetType();
		if (TypeName == TEXT("SWebBrowser") || TypeName == TEXT("SScrollBox") || TypeName == TEXT("SFocusableScrollBox"))
		{
			return InWidgetPath.Widgets[WidgetIndex].Widget;
		}
	}
	return nullptr;
}

bool FTapControllerManager::PathContainsWebBrowser(const FWidgetPath& InWidgetPath)
{
	for (int32 WidgetIndex = InWidgetPath.Widgets.Num() - 1; WidgetIndex >= 0; --WidgetIndex)
	{
		FName TypeName = InWidgetPath.Widgets[WidgetIndex].Widget->GetType();
		if (TypeName == TEXT("SWebBrowser"))
		{
			return true;
		}
	}
	return false;
}

void FTapControllerManager::ShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
	static const FName NAME_TapController("TapController");
	if (CanScrollWidget.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, CanScrollWidget.Pin()->GetCachedGeometry().GetAbsolutePosition().ToString());
	}
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FSlateApplication::Get().GetCursorPos().ToString());

	if (Canvas && HUD->ShouldDisplayDebug(NAME_TapController))
	{
		FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
		DisplayDebugManager.SetFont(GEngine->GetMediumFont());
		DisplayDebugManager.SetDrawColor(FColor::Yellow);
		DisplayDebugManager.DrawString(TEXT("Tap Controller"));

		DisplayDebugManager.SetFont(GEngine->GetTinyFont());

		DisplayDebugManager.SetDrawColor(IsAlreadyStarted() ? FColor::Green : FColor::Red);
		DisplayDebugManager.DrawString(TEXT("Is Tap Controller Startup"));

		DisplayDebugManager.SetDrawColor(IsTapControllerRunning() ? FColor::Green : FColor::Red);
		DisplayDebugManager.DrawString(TEXT("Is Tap Controller Running"));

		DisplayDebugManager.SetDrawColor(FColor::White);
		DisplayDebugManager.DrawString(FString::Printf(TEXT("ControllerType: %s"), ControllerType ? *UEnum::GetValueAsString(ControllerType.GetValue()) : TEXT("None")));
		DisplayDebugManager.DrawString(FString::Printf(TEXT("InputMode: %s"), *UEnum::GetValueAsString(InputMode)));
		TSharedPtr<SWidget> Focus = FSlateApplication::Get().GetKeyboardFocusedWidget();
		DisplayDebugManager.DrawString(FString::Printf(TEXT("Focus: %s"), Focus ? *Focus->ToString() : TEXT("None")));

		DisplayDebugManager.DrawString(FString::Printf(TEXT("CursorInstigatorWidget: %s"), CursorInstigatorWidget.IsValid() ? *CursorInstigatorWidget.Pin()->ToString() : TEXT("None")));
		DisplayDebugManager.DrawString(FString::Printf(TEXT("CanScrollWidget: %s"), CanScrollWidget.IsValid() ? *CanScrollWidget.Pin()->ToString() : TEXT("None")));
		DisplayDebugManager.DrawString(FString::Printf(TEXT("bCanScroll: %d"), bCanScroll));
		
		DisplayDebugManager.DrawString(TEXT("RegistedWidgets:"));

		for (int32 i = 0; i < RegisteredWidgets.Num(); ++i)
		{
			if (TSharedPtr<SWidget> RootWidget = RegisteredWidgets[i]->RootWidget.Pin())
			{
				if (RootWidget->GetType() == TEXT("SObjectWidget"))
				{
					DisplayDebugManager.DrawString(FString::Printf(TEXT("    %d.%s"), i, *static_cast<UWidget*>(RegisteredWidgets[i]->KeyWidget)->GetName()));
				}
				else
				{
					DisplayDebugManager.DrawString(FString::Printf(TEXT("    %d.%s"), i, *RootWidget->ToString()));
				}
			}
		}
	}
}
