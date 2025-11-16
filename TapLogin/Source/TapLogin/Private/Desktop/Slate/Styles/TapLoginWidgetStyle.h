// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PermissionCheckBoxWidgetStyle.h"
#include "QrCodeScanTipButtonWidgetStyle.h"
#include "Styling/SlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"

#include "TapLoginWidgetStyle.generated.h"

/**
 * 
 */
USTRUCT()
struct TAPLOGIN_API FTapLoginStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FTapLoginStyle();
	virtual ~FTapLoginStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FTapLoginStyle& GetDefault();

	UPROPERTY(EditAnywhere)
	FTextBlockStyle TitleTextBlockStyle;

	UPROPERTY(EditAnywhere)
	FMargin TitlePadding;

	UPROPERTY(EditAnywhere)
	FTextBlockStyle SubtitleTextBlockStyle;

	UPROPERTY(EditAnywhere, Category = "QrCode")
	FMargin QrCodePadding;

	UPROPERTY(EditAnywhere, Category = "QrCode")
	FMargin QrCodeDescription2Padding;
	
	UPROPERTY(EditAnywhere, Category = "QrCode")
	FTextBlockStyle QrCodeDescriptionStyle;

	UPROPERTY(EditAnywhere, Category = "QrCode")
	FTextBlockStyle QrCodeDescriptionStyle2;

	UPROPERTY(EditAnywhere, Category = "QrCode")
	FSlateBrush TipContainerBrush;
	
	UPROPERTY(EditAnywhere, Category = "QrCode")
	FQrCodeScanTipButtonStyle QrCodeScanTipButtonStyle;

	UPROPERTY(EditAnywhere, Category = "QrCode|Retry")
	FSlateBrush RetryBrush;

	UPROPERTY(EditAnywhere, Category = "QrCode|Retry")
	FMargin RetryContentPadding;
	
	UPROPERTY(EditAnywhere, Category = "QrCode|Retry")
	FSlateBrush QrCodeRetryIconBrush;
	
	UPROPERTY(EditAnywhere, Category = "QrCode|Retry")
	FTextBlockStyle QrCodeRetryTextBlockStyle;
	
	UPROPERTY(EditAnywhere, Category = "QrCode|Retry")
	FTextBlockStyle QrCodeRetryDescriptionStyle;

	UPROPERTY(EditAnywhere, Category = "WebAuth")
	FMargin NoPermissionPadding;
	
	UPROPERTY(EditAnywhere, Category = "WebAuth")
	FSlateBrush NoPermissionBrush;

	UPROPERTY(EditAnywhere, Category = "WebAuth")
	FMargin WebAuthButtonPadding;
	
	UPROPERTY(EditAnywhere, Category = "WebAuth")
	FTextBlockStyle WebAuthTextBlockStyle;

	UPROPERTY(EditAnywhere, Category = "WebAuth")
	FSlateBrush WebAuthArrow;

	UPROPERTY(EditAnywhere, Category = "WebAuth")
	TEnumAsByte<EHorizontalAlignment> WebAuthHAlignment = HAlign_Center;
	
	UPROPERTY(EditAnywhere, Category = "WebAuth")
	FTextBlockStyle WebAuthTipTextBlockStyle;

	UPROPERTY(EditAnywhere, Category = "WebAuth")
	FTextBlockStyle WebAuthTipTextBlockStyle2;

	UPROPERTY(EditAnywhere, Category = "WebAuth")
	FMargin WebAuthTip2Padding;
	
	UPROPERTY(EditAnywhere, Category = "Permission")
	FMargin PermissionItemPadding;

	UPROPERTY(EditAnywhere, Category = "Permission")
	FTextBlockStyle PermissionTitleTextBlockStyle;
	
	UPROPERTY(EditAnywhere, Category = "Permission")
	FPermissionCheckBoxStyle PermissionCheckBoxStyle;
};

/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UTapLoginWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	/** The actual data describing the widget appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FTapLoginStyle WidgetStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&WidgetStyle);
	}
};
