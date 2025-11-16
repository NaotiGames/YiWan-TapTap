// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Desktop/Slate/Styles/QrCodeScanTipButtonWidgetStyle.h"
#include "Widgets/SCompoundWidget.h"

struct FTapLoginStyle;
class SQrCodeScanTipButton;
DECLARE_DELEGATE_TwoParams(FOnRequestTip, const FString& Url, TSharedRef<SQrCodeScanTipButton> Button);

/**
 * 
 */
class TAPLOGIN_API SQrCodeScanTipButton : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SQrCodeScanTipButton)
		{
		}

	SLATE_EVENT(FOnRequestTip, OnRequestTip);

	SLATE_EVENT(FSimpleDelegate, OnRequestClose);

	SLATE_ARGUMENT_DEFAULT(bool, bShowIcon){true};
	
	SLATE_STYLE_ARGUMENT(FQrCodeScanTipButtonStyle, Style);

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const FText& DisplayText, const FString& Url);
	
	void UpdateState(bool bOpened);
	
protected:
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

	virtual bool SupportsKeyboardFocus() const override;

	virtual const FSlateBrush* GetFocusBrush() const override;

	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	
	const FQrCodeScanTipButtonStyle* Style = nullptr;
	
	TSharedPtr<SImage> ButtonImage;
	
	bool bOpen = false;
	FString RequestUrl;
	FOnRequestTip OnRequestTip;
	FSimpleDelegate OnRequestClose;
};
