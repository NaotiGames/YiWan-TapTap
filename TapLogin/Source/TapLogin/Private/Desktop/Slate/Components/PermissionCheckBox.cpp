// Fill out your copyright notice in the Description page of Project Settings.


#include "PermissionCheckBox.h"

#include "SlateOptMacros.h"
#include "TapLogin.h"
#include "Components/SlateWrapperTypes.h"
#include "Desktop/Slate/Styles/TapLoginWidgetStyle.h"
#include "Styling/SlateStyle.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPermissionCheckBox::Construct(const FArguments& InArgs, const FString& InPermission, const FText& DisplayText)
{
	Permission = InPermission;
	OnPermissionChanged = InArgs._OnPermissionChanged;
	check(InArgs._Style);
	SetVisibility(InArgs._bEnable ? EVisibility::Visible : EVisibility::HitTestInvisible);
	ChildSlot
	[
		SAssignNew(CheckBox, SCheckBox)
		.Style(&InArgs._Style->PermissionCheckBoxStyle)
		.IsChecked(InArgs._IsChecked)
		.OnCheckStateChanged(this, &SPermissionCheckBox::OnCheckStateChanged)
		[
			SNew(STextBlock)
			.Text(DisplayText)
			.TextStyle(&InArgs._Style->PermissionTextBlockStyle)
		]
	];
}

bool SPermissionCheckBox::IsChecked() const
{
	return CheckBox ? CheckBox->GetCheckedState() == ECheckBoxState::Checked : false;
}

void SPermissionCheckBox::OnCheckStateChanged(ECheckBoxState NewState)
{
	OnPermissionChanged.ExecuteIfBound(Permission, NewState == ECheckBoxState::Checked);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
