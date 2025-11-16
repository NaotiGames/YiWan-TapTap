// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Components/TapButton.h"

#include "UMG/Components/TapButtonSlot.h"

void UTapButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UTapButton::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyButton.Reset();
}

void UTapButton::PostLoad()
{
	Super::PostLoad();

	if ( GetChildrenCount() > 0 )
	{
		//TODO UMG Pre-Release Upgrade, now buttons have slots of their own.  Convert existing slot to new slot.
		if ( UPanelSlot* PanelSlot = GetContentSlot() )
		{
			UTapButtonSlot* ButtonSlot = Cast<UTapButtonSlot>(PanelSlot);
			if ( ButtonSlot == NULL )
			{
				ButtonSlot = NewObject<UTapButtonSlot>(this);
				ButtonSlot->Content = GetContentSlot()->Content;
				ButtonSlot->Content->Slot = ButtonSlot;
				Slots[0] = ButtonSlot;
			}
		}
	}
}

#if WITH_EDITOR
const FText UTapButton::GetPaletteCategory()
{
	return NSLOCTEXT("TapTap", "Common", "Common");
}
#endif

TSharedRef<SWidget> UTapButton::RebuildWidget()
{
	MyButton = SNew(STapButton)
	.StyleType(StyleType)
	.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked));

	if ( GetChildrenCount() > 0 )
	{
		Cast<UTapButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
	}
	
	return MyButton.ToSharedRef();
}

#if WITH_ACCESSIBILITY
TSharedPtr<SWidget> UTapButton::GetAccessibleWidget() const
{
	return MyButton;
}
#endif


UClass* UTapButton::GetSlotClass() const
{
	return UTapButtonSlot::StaticClass();
}

void UTapButton::OnSlotAdded(UPanelSlot* InSlot)
{
	if ( MyButton.IsValid() )
	{
		CastChecked<UTapButtonSlot>(InSlot)->BuildSlot(MyButton.ToSharedRef());
	}
}

void UTapButton::OnSlotRemoved(UPanelSlot* InSlot)
{
	if ( MyButton.IsValid() )
	{
		MyButton->SetContent(SNullWidget::NullWidget);
	}
}

FReply UTapButton::SlateHandleClicked()
{
	OnClicked.Broadcast();
	return FReply::Handled();
}
