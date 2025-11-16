// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Styles/TapMessageBoxWidgetStyle.h"


FTapMessageBoxStyle::FTapMessageBoxStyle()
{
}

FTapMessageBoxStyle::~FTapMessageBoxStyle()
{
}

const FName FTapMessageBoxStyle::TypeName(TEXT("FTapMessageBoxStyle"));

const FTapMessageBoxStyle& FTapMessageBoxStyle::GetDefault()
{
	static FTapMessageBoxStyle Default;
	return Default;
}

void FTapMessageBoxStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	RichTextBlockStyle.GetResources(OutBrushes);
}
