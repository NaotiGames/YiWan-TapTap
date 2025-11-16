// Fill out your copyright notice in the Description page of Project Settings.


#include "QrCodeScanTipButtonWidgetStyle.h"


FQrCodeScanTipButtonStyle::FQrCodeScanTipButtonStyle()
{
}

FQrCodeScanTipButtonStyle::~FQrCodeScanTipButtonStyle()
{
}

const FName FQrCodeScanTipButtonStyle::TypeName(TEXT("FQrCodeScanTipButtonStyle"));

const FQrCodeScanTipButtonStyle& FQrCodeScanTipButtonStyle::GetDefault()
{
	static FQrCodeScanTipButtonStyle Default;
	return Default;
}

void FQrCodeScanTipButtonStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	ScanTipTextBlockStyle.GetResources(OutBrushes);
	OutBrushes.Add(&IconBrush);
	OutBrushes.Add(&OpenBrush);
	OutBrushes.Add(&CloseBrush);
}
