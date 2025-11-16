// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate/Common/TapBoxWrapper.h"
#include "Widgets/SCompoundWidget.h"


class STapEditableTextBox;
class AAUImpl;
/**
 * 
 */
class TAPCOMMON_API STapClientVerifyTipWidget : public SCompoundWidget
{
public:
	virtual ~STapClientVerifyTipWidget() override;
	
	SLATE_BEGIN_ARGS(STapClientVerifyTipWidget)
		{
		}

	SLATE_EVENT(FSimpleDelegate, OnClose)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	
	virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;
	
	TSharedPtr<STapBoxWrapper> Wrapper;
	TSharedPtr<STextBlock> TipTextBlock;
	
	bool bFallback = true;
	FSimpleDelegate OnClose;

	TWeakPtr<FSlateStyleSet> SavedStyleSet;


	FReply OnQuitClicked();
	FReply OnDownLoadClicked();
	
	void SetInitErrorType(int ErrorType);

};
