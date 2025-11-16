// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TapControllerTypes.h"
#include "Types/NavigationMetaData.h"
#include "TapControllerManager.generated.h"

class FTapControllerInput;
class STapControllerTip;
class UUserWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTapControllerTypeChanged, TOptional<ETapControllerType> NewType);
DECLARE_DELEGATE_RetVal_TwoParams(TSharedPtr<SWidget>, FTapGetNavigationWidget, EUINavigation Direction, TSharedPtr<SWidget> SourceWidget);
DECLARE_DELEGATE_RetVal_OneParam(bool, FTapControllerScrollEvent, const FVector2D& Offset);



UENUM()
enum class ETapInputMode : uint8
{
	Focus,
	Cursor,
};


class TAPCOMMON_API FTapUINavigationBuilder
{
public:
	FTapUINavigationBuilder() = delete;
	FTapUINavigationBuilder(TSharedRef<SWidget>&& InWidget)
		: Widget(*InWidget)
	{
		MetaData = InWidget->GetMetaData<FNavigationMetaData>();
		if ( !MetaData.IsValid() )
		{
			MetaData = MakeShared<FNavigationMetaData>();
			InWidget->AddMetadata(MetaData.ToSharedRef());
		}
	}

	FORCEINLINE FTapUINavigationBuilder& Up(const TSharedPtr<SWidget>& InWidget)
	{
		MetaData->SetNavigationExplicit(EUINavigation::Up, InWidget);
		return *this;
	}
	FORCEINLINE FTapUINavigationBuilder& Down(const TSharedPtr<SWidget>& InWidget)
	{
		MetaData->SetNavigationExplicit(EUINavigation::Down, InWidget);
		return *this;
	}
	FORCEINLINE FTapUINavigationBuilder& Left(const TSharedPtr<SWidget>& InWidget)
	{
		MetaData->SetNavigationExplicit(EUINavigation::Left, InWidget);
		return *this;
	}
	FORCEINLINE FTapUINavigationBuilder& Right(const TSharedPtr<SWidget>& InWidget)
	{
		MetaData->SetNavigationExplicit(EUINavigation::Right, InWidget);
		return *this;
	}
	FORCEINLINE FTapUINavigationBuilder& Next(const TSharedPtr<SWidget>& InWidget)
	{
		MetaData->SetNavigationExplicit(EUINavigation::Next, InWidget);
		return *this;
	}
	FORCEINLINE FTapUINavigationBuilder& Previous(const TSharedPtr<SWidget>& InWidget)
	{
		MetaData->SetNavigationExplicit(EUINavigation::Previous, InWidget);
		return *this;
	}

	SWidget& Widget;
	TSharedPtr<FNavigationMetaData> MetaData;
};

TAPCOMMON_API void SetupTapNavigation(TSharedRef<SWidget> InWidget, TSharedPtr<SWidget> Up, TSharedPtr<SWidget> Down, TSharedPtr<SWidget> Left, TSharedPtr<SWidget> Right, TSharedPtr<SWidget> Next, TSharedPtr<SWidget> Previous);
TAPCOMMON_API void SetupTapNavigation(TSharedRef<SWidget> InWidget, TSharedPtr<SWidget> All);

struct TAPCOMMON_API FTapControllerWidgetData
{
	FTapControllerWidgetData() = delete;
	FTapControllerWidgetData(UUserWidget* InWidget);
	FTapControllerWidgetData(TSharedRef<SWidget> InWidget);

	FSimpleDelegate OnBackButtonClicked;
	TAttribute<TSharedPtr<SWidget>> StartupFocusWidget;
	TSharedPtr<STapControllerTip> ControllerTip;

protected:
	void* KeyWidget = nullptr;
	TWeakPtr<SWidget> RootWidget;
	bool ShowBackIcon() const { return OnBackButtonClicked.IsBound(); }
	friend class FTapControllerManager;
	friend class FTapControllerInput;
};

class TAPCOMMON_API FTapControllerManager
{
public:
	FTapControllerManager();

	~FTapControllerManager();
	
	static FTapControllerManager& Get();

	TSharedPtr<STapControllerTip> RegisterWidget(const TSharedRef<FTapControllerWidgetData>& NewWidgetData);
	void UnregisterWidget(void* InKeyWidget);
	
	bool Startup(ETapControllerType InControllerType);
	void Shutdown();
	void UpdateConfig(TSharedPtr<FTapControllerConfig> InConfig);
	bool IsAlreadyStarted() const;
	
	TOptional<ETapControllerType> GetControllerType() const;
	FOnTapControllerTypeChanged& OnTapControllerTypeChanged();

	bool IsTapControllerRunning() const;
	bool UpdateRunningState(bool bNewRunning);
	
	void ChangeInputModeCursor(TSharedPtr<SWidget> InstigatorWidget);
	void ChangeInputModeFocus();
	TSharedPtr<SWidget> GetCursorInstigatorWidget() const { return CursorInstigatorWidget.Pin(); }
	ETapInputMode GetInputMode() const { return InputMode; }
	TSharedPtr<SWidget> GetCanScrollWidget() const { return CanScrollWidget.Pin(); }

	TSharedPtr<FTapControllerWidgetData> FindReceiveInputWidgetData() const;
	void FocusToStartupWidget();

protected:
	TSharedPtr<FNavigationConfig> GetNavigationConfig() const;

	void RefreshControllerTips();
	
	void OnFocusChanged(const FFocusEvent& Event, const FWeakWidgetPath& OldPath, const TSharedPtr<SWidget>& OldWidget, const FWidgetPath& NewPath, const TSharedPtr<SWidget>& NewWidget);

	const FTapControllerConfig& GetConfig() const { return Config ? *Config : FTapControllerConfig::DefaultConfig; }
	
	static TSharedPtr<SWidget> FindCanScrollWidget(const FWidgetPath& InWidgetPath);

	static bool PathContainsWebBrowser(const FWidgetPath& InWidgetPath);
	
	void ShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);
	
	TSharedPtr<FTapControllerInput> ControllerInput;
	
	ETapInputMode InputMode = ETapInputMode::Cursor;
	
	TWeakPtr<SWidget> CursorInstigatorWidget;
	TWeakPtr<SWidget> CanScrollWidget;
	bool bCanScroll = false;
	
	TArray<TSharedRef<FTapControllerWidgetData>> RegisteredWidgets;

	FOnTapControllerTypeChanged ControllerTypeChanged;
	TOptional<ETapControllerType> ControllerType;

	TSharedPtr<FTapControllerConfig> Config;
	TSharedPtr<FNavigationConfig> NavConfig;
};
