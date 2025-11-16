// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Components/TapRoundCornersBorder.h"

#include "SlateOptMacros.h"
#include "Slate/TapStyle.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STapRoundCornersBorder::Construct(const FArguments& InArgs)
{
	Brush = FTapStyle::MakeRoundCornersBrush(InArgs._bHalfHeightRadius, InArgs._CornerRadius, InArgs._IconTexture);
	GCMat = Cast<UMaterialInterface>(Brush->GetResourceObject());
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(Brush.Get())
		.HAlign(InArgs._HAlign)
		.VAlign(InArgs._VAlign)
		.Padding(InArgs._Padding)
		.BorderBackgroundColor(InArgs._BorderBackgroundColor)
		[
			InArgs._Content.Widget
		]);
}

void STapRoundCornersBorder::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(GCMat);
}

FString STapRoundCornersBorder::GetReferencerName() const
{
	return TEXT("STapRoundCornersBorder");
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
