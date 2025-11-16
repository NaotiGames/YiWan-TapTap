// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate/Components/STapButton.h"
#include "Widgets/SCompoundWidget.h"


class STapControllerTip;
struct FTapBoxWrapperStyle;
/**
 * Universal rounded corner background box
 * Can create inside 'Construct' method or inherit by your widget
 * Exposed 'Content slot'
 * 1. Round corner BG
 * 2. Title
 * 3. White button (optional)
 * 4. Blue button
 * 5. Close button (optional)
 */
class TAPCOMMON_API STapBoxWrapper : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STapBoxWrapper)
		: _bShowCloseButton(true)
		, _BoxStyleOverride()
	{}

		SLATE_ARGUMENT(FText, TitleText);
	
		SLATE_ARGUMENT(FText, WhiteButtonText);

		SLATE_ARGUMENT(FText, BlueButtonText);
	
		SLATE_ARGUMENT(bool, bShowCloseButton);
	
		/** Content slot */
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_ARGUMENT(EHorizontalAlignment, HAlign)
		SLATE_ARGUMENT(EVerticalAlignment, VAlign)
		SLATE_ATTRIBUTE(FMargin, Padding)
	
		/** Box wrapper style */
		SLATE_ARGUMENT(const FTapBoxWrapperStyle*, BoxStyleOverride);

	
		SLATE_EVENT(FOnClicked, OnWhiteButtonClicked);

		SLATE_EVENT(FOnClicked, OnBlueButtonClicked);
	
		SLATE_EVENT(FOnClicked, OnCloseButtonClicked);

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, float Width, float Height);

	TSharedPtr<STapButton> GetBlueButton() const { return BlueButton; }
	TSharedPtr<STapButton> GetWhiteButton() const { return WhiteButton; }
	TSharedPtr<STapButton> GetCloseButton() const { return CloseButton; }

	TSharedPtr<STapControllerTip> GetControllerTip() const { return Tip; }
	
protected:
	/** Content slot */
	SVerticalBox::FSlot* ContentSlot = nullptr;

	TSharedPtr<STapButton> BlueButton;
	TSharedPtr<STapButton> WhiteButton;
	TSharedPtr<STapButton> CloseButton;
	TSharedPtr<STapControllerTip> Tip;
};
