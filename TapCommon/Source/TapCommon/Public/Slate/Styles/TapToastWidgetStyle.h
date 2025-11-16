// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"

#include "TapToastWidgetStyle.generated.h"

/**
 * 
 */
USTRUCT()
struct TAPCOMMON_API FTapToastStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FTapToastStyle();
	virtual ~FTapToastStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FTapToastStyle& GetDefault();
	
	UPROPERTY(EditAnywhere)
	FSlateBrush Background;

	UPROPERTY(EditAnywhere)
	FSlateBrush IconBrushTemplate;
	
	UPROPERTY(EditAnywhere)
	FTextBlockStyle ContentStyle;

	UPROPERTY(EditAnywhere)
	float Height = 38.f;
};

/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UTapToastWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	/** The actual data describing the widget appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FTapToastStyle WidgetStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&WidgetStyle);
	}
};
