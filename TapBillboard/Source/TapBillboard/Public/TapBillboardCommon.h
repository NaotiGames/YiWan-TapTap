// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TapBillboardMarquee.h"
#include "TapBillboardTypes.h"
#include "TUError.h"
#include "TUType.h"

DECLARE_DELEGATE_OneParam(FTapBadgeDetailsResult, const FBadgeDetails& /** BadgeDetails */);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCustomLinkClicked, const FString&/** Url */);
DECLARE_DELEGATE_OneParam(FAudioOutputStateChanged, bool /** bPlaying */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBillboardMarqueeShow, const FTapBillboardMarquee& Marquee, const FTapBillboardMarqueeConfig& Config);
DECLARE_MULTICAST_DELEGATE(FOnBillboardMarqueeClose);
/**
 * TapBillboard Interface 
 */
class TAPBILLBOARD_API FTapBillboardCommon {
public:

	FOnBillboardMarqueeShow OnBillboardMarqueeShow;
	FOnBillboardMarqueeClose OnBillboardMarqueeClose;
	
	virtual ~FTapBillboardCommon() = default;

	virtual void Init(const FTUConfig& InConfig) = 0;

	virtual void OpenPanel(const FSimpleDelegate& OnSuccess, const FTapFailed& OnFailed,
	                       const FSimpleDelegate& OnClose) = 0;

	virtual void ClosePanel() = 0;

	virtual void OpenSplashPanel(const FSimpleDelegate& OnSuccess, const FTapFailed& OnFailed,
	                             const FSimpleDelegate& OnClose) = 0;

	virtual void CloseSplashPanel() = 0;

	virtual void StartFetchMarqueeData() = 0;

	virtual void StopFetchMarqueeData(bool bCloseNow) = 0;

	/**
	 * @brief Get badge details, async return FBadgeDetails
	 * @see FBadgeDetails
	 * @param OnSuccess Get detail success
	 * @param OnFailed Get detail failed
	 */
	virtual void GetBadgeDetails(const FTapBadgeDetailsResult& OnSuccess, const FTapFailed& OnFailed) = 0;

	/**
	 * @brief Register callback when custom url clicked in 'navigate' or 'splash' panel
	 * @param LinkClickedCallback Custom url clicked callback delegate
	 * @return The delegate handle use for unregister
	 */
	FDelegateHandle RegisterCustomLinkListener(const FOnCustomLinkClicked::FDelegate& LinkClickedCallback);

	/**
	 * @brief Unregister
	 * @param Handle The delegate handle returned by RegisterCustomLinkListener
	 */
	void UnregisterCustomLinkListener(FDelegateHandle Handle);

	/**
	 * @brief Register callback when audio state changed in billboard
	 * @param OnAudioStateChanged State changed delegate
	 */
	void RegisterOutputStateListener(const FAudioOutputStateChanged& OnAudioStateChanged);

	/**
	 * @brief Unregister audio state changed callback
	 */
	void UnregisterOutputStateListener();

protected:
	FAudioOutputStateChanged OnAudioOutputStateChanged;
	FOnCustomLinkClicked OnCustomLinkClicked;
	
};
