// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate/Common/TapBoxWrapper.h"
#include "Widgets/SCompoundWidget.h"
#include "Model/China/AAURealNameConfigModel.h"


struct FAAUAuthIdentifyWord;
class SFocusableScrollBox;
class STapEditableTextBox;
class AAUImpl;
/**
 * 
 */
class ANTIADDICTION_API SManualVerifyWidget : public SCompoundWidget
{
public:
	virtual ~SManualVerifyWidget() override;
	
	SLATE_BEGIN_ARGS(SManualVerifyWidget)
		{
		}

	SLATE_EVENT(FSimpleDelegate, OnClose)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<AAUImpl>& InImpl, const FAAUAuthIdentifyWord& Word);

	FReply OnCommitButtonClicked();

	FReply OnCloseButtonClicked();

	virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;

	void OnBackButtonClicked();

	TSharedPtr<STapEditableTextBox> NameBox;
	TSharedPtr<STapEditableTextBox> CardIdBox;

	TSharedPtr<STapBoxWrapper> Wrapper;
	TSharedPtr<SFocusableScrollBox> FocusableScrollBox;
	
	TSharedPtr<AAUImpl> Impl;
	bool bFallback = true;
	FSimpleDelegate OnClose;
};
