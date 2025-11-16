// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Styles/TapBoxWrapperWidgetStyle.h"


FTapBoxWrapperStyle::FTapBoxWrapperStyle()
{
}

FTapBoxWrapperStyle::~FTapBoxWrapperStyle()
{
}

const FName FTapBoxWrapperStyle::TypeName(TEXT("FTapBoxWrapperStyle"));

const FTapBoxWrapperStyle& FTapBoxWrapperStyle::GetDefault()
{
	static FTapBoxWrapperStyle Default;
	return Default;
}

void FTapBoxWrapperStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	OutBrushes.Add(&BackgroundBrush);
	TitleStyle.GetResources(OutBrushes);
}
