// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TapBillboardModels.h"
#include "TapBillboardWebTipUI.h"
#include "PC/TapBillboardPC.h"
#include "TapBillboardBrowserNavigate.generated.h"

class USizeBox;
struct FTUError;
struct FWebNavigationRequest;

DECLARE_DELEGATE_OneParam(FGetRedDot, int32);
/**
 * 
 */
UCLASS(Abstract)
class TAPBILLBOARD_API UTapBillboardBrowserNavigate : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnCustomLinkClicked::FDelegate OnCustomLinkClicked;
	FAudioOutputStateChanged OnAudioOutputStateChanged;

	FSimpleDelegate OnLoadComplete;
	FTapFailed OnLoadFailed;
	FSimpleDelegate OnBillboardBrowserClosed;

	void LoadUrl(FString Url);

	void Close();
	
protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	bool OnBeforePopup(FString Url, FString FrameName);

	bool OnBeforeNavigate(const FString& Url, const FWebNavigationRequest& Request);

	void OnLoadCompleted();

	void OnLoadError();
	
	UFUNCTION()
	void JSNotifyRedDot(const FString& ResultString);

	void UpdateCloseButtonDPIScale(float NewScale);
	
	UPROPERTY(Meta = (BindWidget))
	class UNativeWidgetHost* NativeWidget;

	UPROPERTY(Meta = (BindWidget))
	UTapBillboardWebTipUI* WebTipUI;

	UPROPERTY(Meta = (BindWidget))
	class UTapPlatformButton* CloseButton;

	UPROPERTY(Meta = (BindWidget))
	class UScaleBox* DPIBox;

	UPROPERTY(Meta = (BindWidget))
	USizeBox* SizeBox;

	TSharedPtr<class SWebBrowserView> BrowserView;
};


