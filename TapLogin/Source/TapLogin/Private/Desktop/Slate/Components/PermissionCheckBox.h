// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Desktop/Slate/Styles/PermissionCheckBoxWidgetStyle.h"
#include "Widgets/SCompoundWidget.h"

struct FTapLoginStyle;
DECLARE_DELEGATE_TwoParams(FPermissionItemClicked, const FString& Permission, bool bEnable);
/**
 * 
 */
class TAPLOGIN_API SPermissionCheckBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPermissionCheckBox)
		{
		}
	
	SLATE_ARGUMENT_DEFAULT(bool, bEnable){true};
	
	SLATE_ARGUMENT_DEFAULT( ECheckBoxState, IsChecked ){ECheckBoxState::Checked};
	
	SLATE_STYLE_ARGUMENT(FPermissionCheckBoxStyle, Style);

	SLATE_EVENT(FPermissionItemClicked, OnPermissionChanged);
	
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const FString& InPermission, const FText& DisplayText);

	bool IsChecked() const;

protected:
	void OnCheckStateChanged(ECheckBoxState NewState);
	
	TSharedPtr<SCheckBox> CheckBox;

	FString Permission;

	FPermissionItemClicked OnPermissionChanged;
};
