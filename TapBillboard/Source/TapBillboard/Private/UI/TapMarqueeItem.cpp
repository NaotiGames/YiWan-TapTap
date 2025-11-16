// Fill out your copyright notice in the Description page of Project Settings.


#include "TapMarqueeItem.h"

#include "TapBillboardCommon.h"
#include "TapBillboardModule.h"
#include "TapUrlResourceLoader.h"
#include "Animation/UMGSequencePlayer.h"
#include "Components/BorderSlot.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "PC/TapBillboardPC.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"

void UTapMarqueeItem::UpdateItem(const FText& Content, float InStartupOffset, int64 Id, bool bMarkReadWhenFinished)
{
	ContentLabel->SetText(Content);
	MarkReadID = Id;
	bMarkRead = bMarkReadWhenFinished;
}

void UTapMarqueeItem::UpdateStyle(const FAnnouncementStyleData& StyleData)
{
	FColor TextColor = FColor::FromHex(StyleData.default_text_color);
	ContentLabel->SetColorAndOpacity(FSlateColor(TextColor));
	FTapUrlResourceLoader::LoadImageTexture(StyleData.icon.url, nullptr, FTapUrlResourceLoaderTextureDelegate::CreateUObject(this, &UTapMarqueeItem::UpdateIconTexture));
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FAnnouncementLayout& Layout = ViewportSize.X > ViewportSize.Y ? StyleData.horizontal : StyleData.vertical;
	if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(MarginWidget->Slot))
	{
		BorderSlot->SetPadding(FMargin(Layout.scroll_margin_x, Layout.scroll_margin_y));
	}
}

void UTapMarqueeItem::UpdateIconTexture(UTexture2D* Texture) {
	Icon->SetBrushFromTexture(Texture, true);		
}

void UTapMarqueeItem::UpdateFont(UFont* Font) {
	if (Font == nullptr) {
		return;
	}
	FSlateFontInfo Info;
#if ENGINE_MAJOR_VERSION > 4
	Info = ContentLabel->GetFont();
#else
	Info = ContentLabel->Font;
#endif
	Info.FontObject = Font;
	ContentLabel->SetFont(Info);
}

void UTapMarqueeItem::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (UPanelWidget* Widget = GetParent())
	{
		FSlateRect ParentRect = Widget->GetCachedGeometry().GetRenderBoundingRect();
		float SelfRight = MyGeometry.GetRenderBoundingRect().Right;

		if (SelfRight < ParentRect.Left)
		{
			if (bMarkRead)
			{
	if (auto Interface = StaticCastSharedPtr<FTapBillboardPC>(FTapBillboardModule::GetTapBillboardInterface()))
				{
					Interface->Rest_AnnouncementsMarkRead({MarkReadID}, FSimpleDelegate(), FTapFailed());
					Interface->PopMarqueeData(MarkReadID);
				}
			}
			OutsideOfParent.ExecuteIfBound(this);
		}
	}
}

void UTapMarqueeItem::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

