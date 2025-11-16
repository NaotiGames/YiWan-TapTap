// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"

#include "TapBoxWrapperWidgetStyle.generated.h"

/**
 * 
 */
USTRUCT()
struct TAPCOMMON_API FTapBoxWrapperStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FTapBoxWrapperStyle();
	virtual ~FTapBoxWrapperStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FTapBoxWrapperStyle& GetDefault();

	UPROPERTY(EditAnywhere)
	FSlateBrush BackgroundBrush;

	UPROPERTY(EditAnywhere)
	FTextBlockStyle TitleStyle;
	
	UPROPERTY(EditAnywhere)
	FMargin TitlePadding;

	UPROPERTY(EditAnywhere)
	FMargin ButtonBoxPadding;

	UPROPERTY(EditAnywhere)
	FVector2D ButtonSize = FVector2D(ForceInitToZero);

	UPROPERTY(EditAnywhere)
	float ButtonGap = 24.f;
	
	UPROPERTY(EditAnywhere)
	FMargin CloseButtonPadding;
};

/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UTapBoxWrapperWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	/** The actual data describing the widget appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FTapBoxWrapperStyle WidgetStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&WidgetStyle);
	}
};
