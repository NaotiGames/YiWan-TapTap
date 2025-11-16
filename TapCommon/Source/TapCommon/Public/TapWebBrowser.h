// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TapWidget.h"
#include "TapWebBrowser.generated.h"

class UTapButton;
struct FWebNavigationRequest;
class SWebBrowser;
class UImage;
class UTextBlock;
class UNativeWidgetHost;

DECLARE_DELEGATE(FOnCloseWebBrowserClicked);

/**
 * 
 */
UCLASS()
class TAPCOMMON_API UTapWebBrowser : public UTapWidget
{
	GENERATED_BODY()
public:
	virtual void LoadURL(const FString& InURL);
	
	void UpdateRetryPanelVisibility(ESlateVisibility NewVisibility);

	FOnCloseWebBrowserClicked OnCloseWebBrowserClicked;

	TSharedPtr<SWebBrowser> GetInnerWebBrowser() const;

	void ExecuteJavascript(const FString& ScriptText);

	bool CanGoBack() const;

	bool IsLoaded() const;
	
	UFUNCTION()
	virtual void GoBack();

	UFUNCTION()
	virtual void Close();

	UFUNCTION()
	virtual void Reload();
	
protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeDestruct() override;
	
	virtual void OnURLChanged(const FText& NewURL);

	virtual void OnTitleChanged(const FText& NewTitle);

	virtual void OnLoadStarted();
	
	virtual void OnLoadCompleted();

	virtual void OnLoadError();

	virtual bool OnBeforeNavigation(const FString& URL, const FWebNavigationRequest& Request);

	virtual bool OnBeforePopup(FString URL,FString FrameName);

	virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;

	
	UPROPERTY(Meta = (BindWidget))
	UNativeWidgetHost* WebBrowser;

	UPROPERTY(Meta = (BindWidgetOptional))
	UTapButton* BTN_GoBack;

	UPROPERTY(Meta = (BindWidgetOptional))
	UTapButton* BTN_Close;
	
	UPROPERTY(Meta = (BindWidgetOptional))
	UTapButton* BTN_Retry;

	UPROPERTY(Meta = (BindWidgetOptional))
	UTextBlock* TB_Retry;

	UPROPERTY(Meta = (BindWidgetOptional))
	UTextBlock* TB_ErrorTip;

	UPROPERTY(Meta = (BindWidgetOptional))
	UImage* ErrorTipIcon;

	UPROPERTY(Meta = (BindWidgetOptional))
	UPanelWidget* RetryPanel;

private:
	
	void HandleOnURLChanged(const FText& NewURL);

	void HandleOnTitleChanged(const FText& NewTitle);

	void HandleOnLoadStarted();
	
	void HandleOnLoadCompleted();

	void HandleOnLoadError();
};

