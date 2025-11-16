// Fill out your copyright notice in the Description page of Project Settings.


#include "TapBillboardWebTipUI.h"

#include "TapBillboardLang.h"

void UTapBillboardWebTipUI::ShowWait() {
	SetVisibility(ESlateVisibility::Visible);
	TipButton->SetVisibility(ESlateVisibility::HitTestInvisible);
	UTexture2D* TapTexture = LoadObject<UTexture2D>(
		nullptr, TEXT("Texture2D'/TapBillboard/LoadingImage.LoadingImage'"));
	TipImage->SetBrushFromTexture(TapTexture);
	TipLabel->SetText(FText::FromString(TUBillboardLanguage::GetCurrentLang()->billboard_loading()));
	AccumulatedTime = 0;
	RotationSpeed = 180;
	LastShowWaitTime = FDateTime::Now();
}



void UTapBillboardWebTipUI::ShowLoadFail() {
	// 计算时间差
	FTimespan TimeDifference = FDateTime::Now() - LastShowWaitTime;
	double TotalSeconds = TimeDifference.GetTotalSeconds();
	if (TotalSeconds >= 2) {
		InternalShowLoadFail();
	} else if (TotalSeconds >= 0) {
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTapBillboardWebTipUI::InternalShowLoadFail, 2 - TotalSeconds);
	}
}

void UTapBillboardWebTipUI::ShowLoadSuccess() {
	SetVisibility(ESlateVisibility::Collapsed);
}

void UTapBillboardWebTipUI::NativeOnInitialized() {
	Super::NativeOnInitialized();
	TipButton->OnClicked.AddDynamic(this, &UTapBillboardWebTipUI::OnTipButtonClicked);
}

void UTapBillboardWebTipUI::NativeConstruct() {
	Super::NativeConstruct();
}

void UTapBillboardWebTipUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (RotationSpeed == 0) {
		return;
	}
	// 累积时间
	AccumulatedTime += InDeltaTime;

	// 计算旋转角度
	float RotationDelta = RotationSpeed * AccumulatedTime;

	// 设置UImage的旋转
	TipImage->SetRenderTransformAngle(RotationDelta);
}

void UTapBillboardWebTipUI::OnTipButtonClicked() {
	OnRefreshClick.ExecuteIfBound();
}

void UTapBillboardWebTipUI::InternalShowLoadFail() {
	SetVisibility(ESlateVisibility::Visible);
	TipButton->SetVisibility(ESlateVisibility::Visible);
	UTexture2D* TapTexture = LoadObject<UTexture2D>(
			nullptr, TEXT("Texture2D'/TapBillboard/RefreshImage.RefreshImage'"));
	TipImage->SetBrushFromTexture(TapTexture);
	TipLabel->SetText(FText::FromString(TUBillboardLanguage::GetCurrentLang()->billboard_network_error_retry()));
	RotationSpeed = 0;
	TipImage->SetRenderTransformAngle(0);
}
