// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Components/TapButtonSlot.h"

#include "Components/Widget.h"
#include "Slate/Components/STapButton.h"

UTapButtonSlot::UTapButtonSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Padding = FMargin(4, 2);

	HorizontalAlignment = HAlign_Center;
	VerticalAlignment = VAlign_Center;
}

void UTapButtonSlot::SetPadding(FMargin InPadding)
{
	Padding = InPadding;
	if ( Button.IsValid() )
	{
		Button.Pin()->SetContentPadding(InPadding);
	}
}

void UTapButtonSlot::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	HorizontalAlignment = InHorizontalAlignment;
	if ( Button.IsValid() )
	{
		Button.Pin()->SetHAlign(InHorizontalAlignment);
	}
}

void UTapButtonSlot::SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
{
	VerticalAlignment = InVerticalAlignment;
	if ( Button.IsValid() )
	{
		Button.Pin()->SetVAlign(InVerticalAlignment);
	}
}

void UTapButtonSlot::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	SetPadding(Padding);
	SetHorizontalAlignment(HorizontalAlignment);
	SetVerticalAlignment(VerticalAlignment);
}

void UTapButtonSlot::BuildSlot(TSharedRef<STapButton> InButton)
{
	Button = InButton;

	Button.Pin()->SetContentPadding(Padding);
	Button.Pin()->SetHAlign(HorizontalAlignment);
	Button.Pin()->SetVAlign(VerticalAlignment);

	Button.Pin()->SetContent(Content ? Content->TakeWidget() : SNullWidget::NullWidget);
}

void UTapButtonSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	Button.Reset();
}
