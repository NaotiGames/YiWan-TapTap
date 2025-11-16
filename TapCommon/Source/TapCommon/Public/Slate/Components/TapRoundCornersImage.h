// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Images/SImage.h"

/**
 * 
 */
class TAPCOMMON_API STapRoundCornersImage : public SImage, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(STapRoundCornersImage)
		{
		}

	SLATE_ARGUMENT_DEFAULT(bool, bHalfHeightRadius){false};
	
	SLATE_ARGUMENT_DEFAULT(float, CornerRadius){8.f};
	
	SLATE_ARGUMENT_DEFAULT(UTexture2D*, IconTexture){nullptr};

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual FString GetReferencerName() const override;

	TSharedPtr<FSlateBrush> Brush;

	UMaterialInterface* GCMat = nullptr;
};
