// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TapBillboardCommon.h"

/**
 * 
 */
class TAPBILLBOARD_API FTapBillboard : public FTapBillboardCommon
{
public:
	FTapBillboard();

	virtual ~FTapBillboard() override;
	
	virtual void Init(const FTUConfig& InConfig) override;

	virtual void OpenPanel(const FSimpleDelegate& OnSuccess, const FTapFailed& OnFailed, const FSimpleDelegate& OnClose) override;

	virtual void ClosePanel() override;
	
	virtual void GetBadgeDetails(const FTapBadgeDetailsResult& OnSuccess, const FTapFailed& OnFailed) override;

	virtual void OpenSplashPanel(const FSimpleDelegate& OnSuccess, const FTapFailed& OnFailed, const FSimpleDelegate& OnClose) override;

	virtual void CloseSplashPanel() override;

	virtual void StartFetchMarqueeData() override;

	virtual void StopFetchMarqueeData(bool bCloseNow) override;

	void HandleCustomLinkClickedEvent(const FString& Url);

	void HandleAudioStatusChangedEvent(bool bNewPlaying);

	void HandleMarqueeShowEvent(const FString& MarqueeStr, const FString& ConfigStr);

	void HandleMarqueeCloseEvent();

};

