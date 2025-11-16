// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Components/TapRoundCornersImage.h"

#include "SlateOptMacros.h"
#include "Slate/TapStyle.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STapRoundCornersImage::Construct(const FArguments& InArgs)
{
	Brush = FTapStyle::MakeRoundCornersBrush(InArgs._bHalfHeightRadius, InArgs._CornerRadius, InArgs._IconTexture);
	GCMat = Cast<UMaterialInterface>(Brush->GetResourceObject());
	SImage::Construct(SImage::FArguments().Image(Brush.Get()));
}

void STapRoundCornersImage::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(GCMat);
}

FString STapRoundCornersImage::GetReferencerName() const
{
	return TEXT("STapRoundCornersImage");
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
