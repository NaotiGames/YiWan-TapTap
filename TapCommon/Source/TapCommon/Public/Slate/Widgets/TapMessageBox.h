// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "TUType.h"

class STapMessageBox;
DECLARE_DELEGATE_OneParam(FMessageBoxButtonDelegate, const TSharedRef<class STapMessageBox>& Box);

class STapBoxWrapper;
/**
 * 1.
 */
class TAPCOMMON_API STapMessageBox : public SCompoundWidget
{
public:
	virtual ~STapMessageBox() override;
	
	SLATE_BEGIN_ARGS(STapMessageBox)
		{
		}
	
	SLATE_ARGUMENT(FText, TitleText);

	
	SLATE_ARGUMENT(FText, ContentRichText); // 1. Message only

	SLATE_DEFAULT_SLOT(FArguments, Content) // 2. Custom Widget

	SLATE_ARGUMENT_DEFAULT(bool, bShowCloseButton){true};
	
	SLATE_ARGUMENT(FText, WhiteButtonText);

	SLATE_ARGUMENT_DEFAULT(bool, bCloseBoxAfterWhiteButtonClicked){true};

	SLATE_ARGUMENT(FText, BlueButtonText);

	SLATE_ARGUMENT_DEFAULT(bool, bCloseBoxAfterBlueButtonClicked){true};

	SLATE_EVENT(FMessageBoxButtonDelegate, OnWhiteButtonClicked);

	SLATE_EVENT(FMessageBoxButtonDelegate, OnBlueButtonClicked);

	SLATE_EVENT(FSimpleDelegate, OnCloseButtonClicked);

	SLATE_ARGUMENT_DEFAULT(TOptional<ELanguageType>, ForceControllerLanguageType){};
	
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
protected:
	FReply OnWhiteButtonClicked();

	FReply OnBlueButtonClicked();

	FReply OnCloseButtonClicked();
	
	virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;

	void OnBackButtonClicked();

	FMessageBoxButtonDelegate WhiteButtonClicked;
	FMessageBoxButtonDelegate BlueButtonClicked;
	FSimpleDelegate CloseButtonClicked;

	bool bCloseBoxAfterWhiteButtonClicked = true;
	bool bCloseBoxAfterBlueButtonClicked = true;

	TSharedPtr<STapBoxWrapper> Wrapper;
};
