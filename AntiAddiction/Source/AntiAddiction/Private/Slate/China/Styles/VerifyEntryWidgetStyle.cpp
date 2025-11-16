// Fill out your copyright notice in the Description page of Project Settings.


#include "VerifyEntryWidgetStyle.h"


FVerifyEntryStyle::FVerifyEntryStyle()
{
}

FVerifyEntryStyle::~FVerifyEntryStyle()
{
}

const FName FVerifyEntryStyle::TypeName(TEXT("FVerifyEntryStyle"));

const FVerifyEntryStyle& FVerifyEntryStyle::GetDefault()
{
	static FVerifyEntryStyle Default;
	return Default;
}

void FVerifyEntryStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	OutBrushes.Add(&Icon);
	ContentStyle.GetResources(OutBrushes);
	TailStyle.GetResources(OutBrushes);
}
