// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Styles/TapToastThrobberWidgetStyle.h"


FTapToastThrobberStyle::FTapToastThrobberStyle()
{
}

FTapToastThrobberStyle::~FTapToastThrobberStyle()
{
}

const FName FTapToastThrobberStyle::TypeName(TEXT("FTapToastThrobberStyle"));

const FTapToastThrobberStyle& FTapToastThrobberStyle::GetDefault()
{
	static FTapToastThrobberStyle Default;
	return Default;
}

void FTapToastThrobberStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	// Add any brush resources here so that Slate can correctly atlas and reference them
	OutBrushes.Add(&BackGroundBrush);
	OutBrushes.Add(&PieceImage);

	ContentStyle.GetResources(OutBrushes);
}
