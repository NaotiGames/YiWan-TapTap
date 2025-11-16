// Fill out your copyright notice in the Description page of Project Settings.


#include "QrCodeScanTipButton.h"

#include "SlateOptMacros.h"
#include "TapCommon.h"
#include "TapLogin.h"
#include "Desktop/Slate/Styles/TapLoginWidgetStyle.h"
#include "Styling/SlateStyle.h"
#include "Widgets/Images/SImage.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SQrCodeScanTipButton::Construct(const FArguments& InArgs, const FText& DisplayText, const FString& Url)
{
	OnRequestTip = InArgs._OnRequestTip;
	OnRequestClose = InArgs._OnRequestClose;
	RequestUrl = Url;
	
	Style = InArgs._Style;

	check(Style);
	
	ChildSlot
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		[
			SAssignNew(ButtonImage, SImage)
		]
		+SOverlay::Slot()
		.Padding(FMargin(4.f, 3.f))
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				InArgs._bShowIcon
				? SNew(SImage)
				.Image(&Style->IconBrush)
				: SNullWidget::NullWidget
			]
			+SHorizontalBox::Slot()
			.Padding(0.f, 0.f, 0.f, 0.f)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(DisplayText)
				.TextStyle(&Style->ScanTipTextBlockStyle)
				.Justification(ETextJustify::Center)
				.Visibility(EVisibility::SelfHitTestInvisible)
			]
		]
	];

	UpdateState(false);
}

void SQrCodeScanTipButton::UpdateState(bool bOpened)
{
	if (Style)
	{
		ButtonImage->SetImage(bOpened ? &Style->OpenBrush : &Style->CloseBrush);
	}

	bOpen = bOpened;
}

void SQrCodeScanTipButton::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);

	UpdateState(true);
	OnRequestTip.ExecuteIfBound(RequestUrl, StaticCastSharedRef<SQrCodeScanTipButton>(AsShared()));
}

void SQrCodeScanTipButton::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	SCompoundWidget::OnMouseLeave(MouseEvent);
	UpdateState(false);
	OnRequestClose.ExecuteIfBound();
}

bool SQrCodeScanTipButton::SupportsKeyboardFocus() const
{
	return true;
}

const FSlateBrush* SQrCodeScanTipButton::GetFocusBrush() const
{
	return FTapCommonModule::Get().Style->GetBrush("/Button/FocusBrushHalfRound");
}

FReply SQrCodeScanTipButton::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	if (!bOpen)
	{
		UpdateState(true);
		OnRequestTip.ExecuteIfBound(RequestUrl, StaticCastSharedRef<SQrCodeScanTipButton>(AsShared()));
	}
	return FReply::Handled();
}

void SQrCodeScanTipButton::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	if (bOpen)
	{
		UpdateState(false);
		OnRequestClose.ExecuteIfBound();
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
