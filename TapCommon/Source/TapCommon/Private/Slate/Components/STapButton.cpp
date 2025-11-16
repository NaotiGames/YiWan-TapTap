// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Components/STapButton.h"

#include "SlateOptMacros.h"
#include "TapCommon.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STapButton::Construct(const FArguments& InArgs)
{
	SavedStyleSet = FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style;
	StyleType = InArgs._StyleType;

	if ((InArgs._Content.Widget == SNullWidget::NullWidget) && (InArgs._Text.IsBound() || !InArgs._Text.Get().IsEmpty()) )
	{
		ContentWidget = SNew(STextBlock)
			.Visibility(EVisibility::HitTestInvisible)
			.Text( InArgs._Text )
			.TextStyle(GetTextStyle());
	}
	else
	{
		ContentWidget = InArgs._Content.Widget;
	}
	
	check(ContentWidget);
	
	SButton::Construct(SButton::FArguments()
		.ButtonStyle(GetButtonStyle())
		.HAlign(InArgs._HAlign)
		.VAlign(InArgs._VAlign)
		.ContentPadding(InArgs._ContentPadding)
		.OnClicked(InArgs._OnClicked)
		[
			ContentWidget.ToSharedRef()
		]
	);
}

FVector2D STapButton::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	if (ContentWidget == SNullWidget::NullWidget)
	{
#if ENGINE_MAJOR_VERSION >= 5
		return GetBorderImage()->ImageSize;
#else
		return GetBorder()->ImageSize;
#endif
	}
	else
	{
		return SBorder::ComputeDesiredSize(LayoutScaleMultiplier);
	}
}

const FSlateBrush* STapButton::GetFocusBrush() const
{
	switch (StyleType)
	{
	case ETapButtonStyleType::Blue:
	case ETapButtonStyleType::White:
	case ETapButtonStyleType::Black:
		return FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style->GetBrush("/Button/FocusBrushHalfRound");
	case ETapButtonStyleType::RefreshBlue:
	case ETapButtonStyleType::RefreshBlack:
		return FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style->GetBrush("/Button/FocusBrushRoundCorner");
	case ETapButtonStyleType::RoundBack:
	case ETapButtonStyleType::RoundClose:
		return FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style->GetBrush("/Button/FocusBrushRound");
	case ETapButtonStyleType::SmallBack:
	case ETapButtonStyleType::SmallClose:
		return FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style->GetBrush("/Button/FocusBrushRoundCorner");
	case ETapButtonStyleType::BoldBack:
	case ETapButtonStyleType::BoldClose:
		return FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style->GetBrush("/Button/FocusBrushRoundCorner");
	case ETapButtonStyleType::LargeCopy:
		return FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style->GetBrush("/Button/FocusBrushRound");
	default: break;
	}
	return nullptr;
}

const FButtonStyle* STapButton::GetButtonStyle() const
{
	if (TSharedPtr<FSlateStyleSet> StyleSet = SavedStyleSet.Pin())
	{
		switch (StyleType)
		{
		case ETapButtonStyleType::Blue:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/BlueButtonStyle");
		case ETapButtonStyleType::White:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/WhiteButtonStyle");
		case ETapButtonStyleType::Black:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/BlackButtonStyle");
		case ETapButtonStyleType::RefreshBlue:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/RefreshBlueButtonStyle");
		case ETapButtonStyleType::RefreshBlack:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/RefreshBlackButtonStyle");
		case ETapButtonStyleType::RoundBack:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/RoundBackButtonStyle");
		case ETapButtonStyleType::RoundClose:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/RoundCloseButtonStyle");
		case ETapButtonStyleType::SmallBack:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/SmallBackButtonStyle");
		case ETapButtonStyleType::SmallClose:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/SmallCloseButtonStyle");
		case ETapButtonStyleType::BoldBack:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/BoldBackButtonStyle");
		case ETapButtonStyleType::BoldClose:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/BoldCloseButtonStyle");
		case ETapButtonStyleType::LargeCopy:
			return &StyleSet->GetWidgetStyle<FButtonStyle>("/Button/LargeCopyButtonStyle");
		default:
			break;
		}
	}
	return nullptr;
}

const FTextBlockStyle* STapButton::GetTextStyle() const
{
	if (TSharedPtr<FSlateStyleSet> StyleSet = SavedStyleSet.Pin())
	{
		switch (StyleType)
		{
		case ETapButtonStyleType::Blue:
			return &StyleSet->GetWidgetStyle<FTextBlockStyle>("/Button/BlueButtonTextStyle");
		case ETapButtonStyleType::White:
			return &StyleSet->GetWidgetStyle<FTextBlockStyle>("/Button/WhiteButtonTextStyle");
		case ETapButtonStyleType::Black:
			return &StyleSet->GetWidgetStyle<FTextBlockStyle>("/Button/BlackButtonTextStyle");
		case ETapButtonStyleType::RefreshBlue:
			break;
		case ETapButtonStyleType::RefreshBlack:
			break;
		default:
			break;
		}
	}
	return nullptr;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
