// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"

#include "TapMessageBoxWidgetStyle.generated.h"

/**
 * 
 */
USTRUCT()
struct TAPCOMMON_API FTapMessageBoxStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FTapMessageBoxStyle();
	virtual ~FTapMessageBoxStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FTapMessageBoxStyle& GetDefault();

	UPROPERTY(EditAnywhere)
	FTextBlockStyle RichTextBlockStyle;

	UPROPERTY(EditAnywhere)
	FMargin ContentPadding;
};

/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UTapMessageBoxWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	/** The actual data describing the widget appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FTapMessageBoxStyle WidgetStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&WidgetStyle);
	}
};
