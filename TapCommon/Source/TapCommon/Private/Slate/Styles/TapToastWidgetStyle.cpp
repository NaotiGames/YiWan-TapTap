// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Styles/TapToastWidgetStyle.h"

FTapToastStyle::FTapToastStyle()
{
}

FTapToastStyle::~FTapToastStyle()
{
}

const FName FTapToastStyle::TypeName(TEXT("FTapToastStyle"));

const FTapToastStyle& FTapToastStyle::GetDefault()
{
	static FTapToastStyle Default;
	return Default;
}

void FTapToastStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	OutBrushes.Add(&Background);
	OutBrushes.Add(&IconBrushTemplate);
	ContentStyle.GetResources(OutBrushes);
}
