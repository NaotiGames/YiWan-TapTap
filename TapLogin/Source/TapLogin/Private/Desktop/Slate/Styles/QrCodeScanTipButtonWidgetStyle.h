// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"

#include "QrCodeScanTipButtonWidgetStyle.generated.h"

/**
 * 
 */
USTRUCT()
struct TAPLOGIN_API FQrCodeScanTipButtonStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FQrCodeScanTipButtonStyle();
	virtual ~FQrCodeScanTipButtonStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FQrCodeScanTipButtonStyle& GetDefault();

	UPROPERTY(EditAnywhere)
	FTextBlockStyle ScanTipTextBlockStyle;

	UPROPERTY(EditAnywhere)
	FSlateBrush IconBrush;

	UPROPERTY(EditAnywhere)
	FSlateBrush OpenBrush;

	UPROPERTY(EditAnywhere)
	FSlateBrush CloseBrush;
};

/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UQrCodeScanTipButtonWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	/** The actual data describing the widget appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FQrCodeScanTipButtonStyle WidgetStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&WidgetStyle);
	}
};
