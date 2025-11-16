// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TapBillboardModels.h"
#include "PC/TapBillboardPC.h"
#include "TapBillboardWebTipUI.h"
#include "Blueprint/UserWidget.h"
#include "TapBillboardBrowserSplash.generated.h"

struct FWebNavigationRequest;
class USizeBox;
/**
 * 
 */
UCLASS(Abstract)
class TAPBILLBOARD_API UTapBillboardBrowserSplash : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnCustomLinkClicked::FDelegate OnCustomLinkClicked;
	FAudioOutputStateChanged OnAudioOutputStateChanged;
	
	FSimpleDelegate OnLoadComplete;
	FTapFailed OnLoadFailed;
	FSimpleDelegate OnBillboardBrowserClosed;

	void LoadUrl(FString Url);

	void SetExpireTime(int64 Time);

	void LoadSplash();

	void LoadSplashWithID(int64 SplashID);
	
protected:
	virtual void NativeOnInitialized() override;

	bool OnBeforePopup(FString Url, FString FrameName);

	bool OnBeforeNavigate(const FString& Url, const FWebNavigationRequest& Request);

	void OnLoadCompleted();

	void OnLoadError();

	void TimerRemoveSelf();

	void InternalFetchUnreadSplashCallback(const TArray<FAnnouncementGeneralData>& GeneralData);

	
	UPROPERTY(Meta = (BindWidget))
	class UNativeWidgetHost* NativeWidget;
	
	UPROPERTY(Meta = (BindWidget))
	USizeBox* SizeBox;

	UPROPERTY(Meta = (BindWidget))
	UTapBillboardWebTipUI* WebTipUI;

	FTimerHandle ExpireRemoveTimer;
	
	TSharedPtr<class SWebBrowserView> BrowserView;
};

 