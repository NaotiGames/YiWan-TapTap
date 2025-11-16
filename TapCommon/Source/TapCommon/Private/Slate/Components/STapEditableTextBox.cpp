// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Components/STapEditableTextBox.h"

#include "SlateOptMacros.h"
#include "TapCommon.h"
#include "Framework/Application/SlateUser.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STapEditableTextBox::Construct(const FArguments& InArgs)
{
	SEditableTextBox::Construct(SEditableTextBox::FArguments()
		.OnTextChanged(this, &STapEditableTextBox::OnTapEditableTextChanged)
		.OnTextCommitted(InArgs._OnTextCommitted)
		.OnVerifyTextChanged(InArgs._OnVerifyTextChanged)
		.OnTextChanged(InArgs._OnTextChanged)
		.HintText(InArgs._HintText)
		.Style(InArgs._Style ? InArgs._Style : &FTapCommonModule::Get().Style->GetWidgetStyle<FEditableTextBoxStyle>("/Common/TapEditableTextBoxStyle")));
}

TSharedPtr<SEditableText> STapEditableTextBox::GetEditableText() const
{
	return EditableText;
}

int32 STapEditableTextBox::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 NewLayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 Result = SEditableTextBox::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, NewLayerId, InWidgetStyle, bParentEnabled);

#if PLATFORM_UI_NEEDS_FOCUS_OUTLINES
	// Check if we need to show the keyboard focus ring, this is only necessary if the widget could be focused.
	if (bCanSupportFocus && SupportsKeyboardFocus())
	{
		if (TSharedPtr<FSlateUser> User  = FSlateApplication::Get().GetUser(0))
		{
			bool bShowUserFocus = User->ShouldShowFocus(EditableText.ToSharedRef());
			if (bShowUserFocus)
			{
				const FSlateBrush* BrushResource = GetFocusBrush();

				if (BrushResource != nullptr)
				{
					FSlateDrawElement::MakeBox(
						OutDrawElements,
						NewLayerId,
						AllottedGeometry.ToPaintGeometry(),
						BrushResource,
						ESlateDrawEffect::None,
						BrushResource->GetTint(InWidgetStyle)
					);
				}
			}
		}
	}
#endif
	return Result;
}

const FSlateBrush* STapEditableTextBox::GetFocusBrush() const
{
	return FTapCommonModule::Get().Style->GetBrush("/Button/FocusBrushHalfRound");
}

void STapEditableTextBox::OnTapEditableTextChanged(const FText& InText)
{
	if (OnVerifyTextChanged.IsBound())
	{
		FText OutErrorMessage;
		if (!OnVerifyTextChanged.Execute(InText, OutErrorMessage))
		{
			// Display as an error.
			SetError(OutErrorMessage);
			SetText(LastText);
			return;
		}
		else
		{
			SetError(FText::GetEmpty());
		}
	}
	LastText = InText;
	OnTextChanged.ExecuteIfBound(InText);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
