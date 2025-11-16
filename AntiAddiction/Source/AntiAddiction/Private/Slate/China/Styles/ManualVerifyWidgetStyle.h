// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"

#include "ManualVerifyWidgetStyle.generated.h"

/**
 * 
 */
USTRUCT()
struct ANTIADDICTION_API FManualVerifyStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FManualVerifyStyle();
	virtual ~FManualVerifyStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FManualVerifyStyle& GetDefault();

	UPROPERTY(EditAnywhere)
	FTextBlockStyle ContentStyle;
	UPROPERTY(EditAnywhere)
	FScrollBoxStyle ContentScrollBoxStyle;
	UPROPERTY(EditAnywhere)
	FMargin ContentMargin;

	UPROPERTY(EditAnywhere)
	FEditableTextBoxStyle InputBoxStyle;
};

/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UManualVerifyWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	/** The actual data describing the widget appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FManualVerifyStyle WidgetStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&WidgetStyle);
	}
};
