// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SEditableTextBox.h"

/**
 * 
 */
class TAPCOMMON_API STapEditableTextBox : public SEditableTextBox
{
public:
	SLATE_BEGIN_ARGS(STapEditableTextBox)
		{
		}

	SLATE_EVENT( FOnTextCommitted, OnTextCommitted );
	
	SLATE_ATTRIBUTE( EKeyboardType, VirtualKeyboardType);

	SLATE_ATTRIBUTE( FText, HintText );

	SLATE_EVENT( FOnVerifyTextChanged, OnVerifyTextChanged );

	SLATE_EVENT(FOnTextChanged, OnTextChanged);

	SLATE_ARGUMENT_DEFAULT(const FEditableTextBoxStyle*, Style){nullptr};

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TSharedPtr<SEditableText> GetEditableText() const;

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	
	
	virtual const FSlateBrush* GetFocusBrush() const override;

	void OnTapEditableTextChanged(const FText& InText);

	FText LastText;
};
