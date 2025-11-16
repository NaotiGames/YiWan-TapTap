// Fill out your copyright notice in the Description page of Project Settings.


#include "TapLoginWidgetStyle.h"


FTapLoginStyle::FTapLoginStyle()
{
}

FTapLoginStyle::~FTapLoginStyle()
{
}

const FName FTapLoginStyle::TypeName(TEXT("FTapLoginStyle"));

const FTapLoginStyle& FTapLoginStyle::GetDefault()
{
	static FTapLoginStyle Default;
	return Default;
}

void FTapLoginStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	TitleTextBlockStyle.GetResources(OutBrushes);
	SubtitleTextBlockStyle.GetResources(OutBrushes);
	QrCodeDescriptionStyle.GetResources(OutBrushes);
	WebAuthTipTextBlockStyle.GetResources(OutBrushes);
	PermissionTitleTextBlockStyle.GetResources(OutBrushes);
	WebAuthTextBlockStyle.GetResources(OutBrushes);
	
	PermissionCheckBoxStyle.GetResources(OutBrushes);
	QrCodeScanTipButtonStyle.GetResources(OutBrushes);
}
