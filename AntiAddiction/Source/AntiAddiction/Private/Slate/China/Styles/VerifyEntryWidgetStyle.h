// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"

#include "VerifyEntryWidgetStyle.generated.h"

/**
 * 
 */
USTRUCT()
struct ANTIADDICTION_API FVerifyEntryStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FVerifyEntryStyle();
	virtual ~FVerifyEntryStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FVerifyEntryStyle& GetDefault();
	
	UPROPERTY(EditAnywhere)
	FSlateBrush Icon;
	UPROPERTY(EditAnywhere)
	FMargin IconMargin;
	
	UPROPERTY(EditAnywhere)
	FTextBlockStyle ContentStyle;
	UPROPERTY(EditAnywhere)
	FMargin ContentMargin;
	
	UPROPERTY(EditAnywhere)
	FTextBlockStyle TailStyle;
	UPROPERTY(EditAnywhere)
	FMargin TailMargin;
};

/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UVerifyEntryWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	/** The actual data describing the widget appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FVerifyEntryStyle WidgetStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&WidgetStyle);
	}
};
