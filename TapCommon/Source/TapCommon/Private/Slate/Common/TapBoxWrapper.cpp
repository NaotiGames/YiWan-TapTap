// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Common/TapBoxWrapper.h"

#include "SlateOptMacros.h"
#include "TapCommon.h"
#include "Controller/TapControllerTip.h"
#include "Slate/Styles/TapBoxWrapperWidgetStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SSpacer.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STapBoxWrapper::Construct(const FArguments& InArgs, float Width, float Height)
{
	bool bShowWhiteButton = !InArgs._WhiteButtonText.IsEmpty();
	bool bShowBlueButton = !InArgs._BlueButtonText.IsEmpty();

	// ensure(bShowBlueButton || bShowWhiteButton); //暂不考虑没有按钮的情况
	
	TSharedRef<FSlateStyleSet> CommonStyleSet = FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style.ToSharedRef();
	
	const FTapBoxWrapperStyle& Style = InArgs._BoxStyleOverride
		? *InArgs._BoxStyleOverride
		: CommonStyleSet->GetWidgetStyle<FTapBoxWrapperStyle>(bShowBlueButton && bShowWhiteButton ? "/Common/TapBoxWrapperStyle" : "/Common/TapBoxWrapperStyleOneButton");

	TSharedRef<SHorizontalBox> ButtonBox = SNew(SHorizontalBox);
	if (bShowWhiteButton)
	{
		ButtonBox->AddSlot()
		.AutoWidth()
		[
			SNew(SBox).WidthOverride(Style.ButtonSize.X).HeightOverride(Style.ButtonSize.Y)
			[
				SAssignNew(WhiteButton, STapButton)
				.StyleType(ETapButtonStyleType::White)
				.Text(InArgs._WhiteButtonText)
				.OnClicked(InArgs._OnWhiteButtonClicked)
			]
		];
	}
	if (bShowBlueButton && bShowWhiteButton)
	{
		ButtonBox->AddSlot()
		.AutoWidth()
		[
			SNew(SSpacer)
			.Size(FVector2D(Style.ButtonGap, 1.f))
		];
	}
	if (bShowBlueButton)
	{
		ButtonBox->AddSlot()
		.AutoWidth()
		[
			SNew(SBox).WidthOverride(Style.ButtonSize.X).HeightOverride(Style.ButtonSize.Y)
			[
				SAssignNew(BlueButton, STapButton)
				.StyleType(ETapButtonStyleType::Blue)
				.Text(InArgs._BlueButtonText)
				.OnClicked(InArgs._OnBlueButtonClicked)
			]
		];
	}

	ChildSlot
	[
		SNew(SBorder)
		.Padding(0.f)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Visibility(EVisibility::Visible)
		[
			SNew(SBox)
			.WidthOverride(Width)
			.HeightOverride(Height)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SImage)
					.Image(&Style.BackgroundBrush)
				]
				+ SOverlay::Slot()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					  .Padding(Style.TitlePadding)
					  .AutoHeight()
					  .HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.TextStyle(&Style.TitleStyle)
						.Text(InArgs._TitleText)
					]
					+ SVerticalBox::Slot()
					  .HAlign(InArgs._HAlign)
					  .VAlign(InArgs._VAlign)
					  .Padding(InArgs._Padding)
					  .FillHeight(1.f)
					  .Expose(ContentSlot)
					[
						InArgs._Content.Widget
					]
					+ SVerticalBox::Slot()
					  .Padding(Style.ButtonBoxPadding)
					  .AutoHeight()
					  .HAlign(HAlign_Center)
					[
						ButtonBox
					]
				]
				+ SOverlay::Slot()
				  .HAlign(HAlign_Right)
				  .VAlign(VAlign_Top)
				  .Padding(Style.CloseButtonPadding)
				[
					InArgs._bShowCloseButton
						? SAssignNew(CloseButton, STapButton)
						.StyleType(ETapButtonStyleType::SmallClose)
						.OnClicked(InArgs._OnCloseButtonClicked)
						: SNullWidget::NullWidget
				]
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Bottom)
				[
					SAssignNew(Tip ,STapControllerTip).RenderTransform(FSlateRenderTransform(FVector2D(0.f, 42.f)))
				]
			]
		]
	];
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
