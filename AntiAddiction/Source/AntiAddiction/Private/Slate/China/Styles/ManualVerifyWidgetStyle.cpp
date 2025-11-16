// Fill out your copyright notice in the Description page of Project Settings.


#include "ManualVerifyWidgetStyle.h"


FManualVerifyStyle::FManualVerifyStyle()
{
}

FManualVerifyStyle::~FManualVerifyStyle()
{
}

const FName FManualVerifyStyle::TypeName(TEXT("FManualVerifyStyle"));

const FManualVerifyStyle& FManualVerifyStyle::GetDefault()
{
	static FManualVerifyStyle Default;
	return Default;
}

void FManualVerifyStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	ContentStyle.GetResources(OutBrushes);
	ContentScrollBoxStyle.GetResources(OutBrushes);
	InputBoxStyle.GetResources(OutBrushes);
}
