// Fill out your copyright notice in the Description page of Project Settings.


#include "TapBillboardMarqueeUI.h"
#include "JsonWrapper.h"
#include "TapBillboardModule.h"
#include "TapUrlResourceLoader.h"
#include "Animation/UMGSequencePlayer.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBoxSlot.h"


#if PLATFORM_ANDROID || PLATFORM_IOS
#define MARQUEE_HEIGHT (57.6f)
#else
#define MARQUEE_HEIGHT (40.f)
#endif

void UTapBillboardMarquee::UpdateStyle(const FAnnouncementStyleData& StyleData)
{
	if (GEngine && GEngine->GameViewport)
	{
		FColor BGColor = FColor::FromHex(StyleData.content_background_color);
		BGColor.A = 127;
		BGLeft->SetColorAndOpacity(BGColor);
		BGWidget->SetColorAndOpacity(BGColor);
		BGRight->SetColorAndOpacity(BGColor);
		
		if (UCanvasPanelSlot* BGSlot = Cast<UCanvasPanelSlot>(LayoutOverlay->Slot))
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			const FAnnouncementLayout& Layout = ViewportSize.X > ViewportSize.Y ? StyleData.horizontal : StyleData.vertical;
			if (Layout.scroll_width_type == TEXT("fixed"))
			{
				BGLeft->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				BGRight->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

				FVector2D Size;
				GEngine->GameViewport->GetViewportSize(Size);
				const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(GEngine->GameViewport);
				float Scale = Size.X / 812.f / DPIScale;
				float Width = Layout.scroll_width * Scale + 30.f;
				if (UOverlaySlot* OSlot = Cast<UOverlaySlot>(ItemPanel->Slot))
				{
					OSlot->SetPadding(FMargin(15.f, 0.f));
				}
				if (Layout.position == TEXT("topLeft"))
				{
					BGSlot->SetAnchors(FAnchors(0.f, 0.f));
					BGSlot->SetAlignment(FVector2D(0.f, 0.f));
					BGSlot->SetOffsets(FMargin(Layout.position_offset_x, Layout.position_offset_y, Width, MARQUEE_HEIGHT));
				}
				else if(Layout.position == TEXT("top"))
				{
					BGSlot->SetAnchors(FAnchors(0.5f, 0.f));
					BGSlot->SetAlignment(FVector2D(0.5f, 0.f));
					BGSlot->SetOffsets(FMargin(Layout.position_offset_x, Layout.position_offset_y, Width, MARQUEE_HEIGHT));
				}
				else if(Layout.position == TEXT("topRight"))
				{
					BGSlot->SetAnchors(FAnchors(1.f, 0.f));
					BGSlot->SetAlignment(FVector2D(1.f, 0.f));
					BGSlot->SetOffsets(FMargin(-Layout.position_offset_x, Layout.position_offset_y, Width, MARQUEE_HEIGHT));
				}
				else if(Layout.position == TEXT("bottomLeft"))
				{
					BGSlot->SetAnchors(FAnchors(0.f, 1.f));
					BGSlot->SetAlignment(FVector2D(0.f, 1.f));
					BGSlot->SetOffsets(FMargin(Layout.position_offset_x, -Layout.position_offset_y, Width, MARQUEE_HEIGHT));
				}
				else if(Layout.position == TEXT("bottom"))
				{
					BGSlot->SetAnchors(FAnchors(0.5f, 1.f));
					BGSlot->SetAlignment(FVector2D(0.5f, 1.f));
					BGSlot->SetOffsets(FMargin(Layout.position_offset_x, -Layout.position_offset_y, Width, MARQUEE_HEIGHT));
				}
				else
				{
					ensure(Layout.position == TEXT("bottomRight"));
					BGSlot->SetAnchors(FAnchors(1.f, 1.f));
					BGSlot->SetAlignment(FVector2D(1.f, 1.f));
					BGSlot->SetOffsets(FMargin(-Layout.position_offset_x, -Layout.position_offset_y, Width, MARQUEE_HEIGHT));
				}
			}
			else if(Layout.scroll_width_type == TEXT("fill"))
			{
				BGLeft->SetVisibility(ESlateVisibility::Collapsed);
				BGRight->SetVisibility(ESlateVisibility::Collapsed);
				if (UOverlaySlot* OSlot = Cast<UOverlaySlot>(ItemPanel->Slot))
				{
					OSlot->SetPadding(FMargin(0.f));
				}
				if (Layout.position == TEXT("top"))
				{
					BGSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 0.f));
					BGSlot->SetAlignment(FVector2D(0.f, 0.f));
					BGSlot->SetOffsets(FMargin(0.f, Layout.position_offset_y, 0.f, MARQUEE_HEIGHT));
				}
				else
				{
					ensure(Layout.position == TEXT("bottom"));
					BGSlot->SetAnchors(FAnchors(0.f, 1.f, 1.f, 1.f));
					BGSlot->SetAlignment(FVector2D(0.f, 1.f));
					BGSlot->SetOffsets(FMargin(0.f, -Layout.position_offset_y, 0.f, MARQUEE_HEIGHT));
				}
			}
		}
		else
		{
			ensure(false);
		}
	}
}

void UTapBillboardMarquee::StartAnnouncementLoop()
{
	if (IsLoopRunning())
	{
		return;
	}
	
	NextAnnouncement();
}

void UTapBillboardMarquee::StopAnnouncementLoop()
{
	if (!IsLoopRunning())
	{
		return;
	}
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DelayTimer);
	}
	CurrentLoop = nullptr;
}

void UTapBillboardMarquee::UpdateScrollSpeed(float NewSpeed)
{
	ScrollSpeed = NewSpeed;
}

bool UTapBillboardMarquee::NextAnnouncement()
{
	if (const FAnnouncementDetailData* DataP = BillboardInterface->FindNextMarqueeData(ScrollingIds))
	{
		CurrentLoop = MakeShared<FAnnouncementDetailData>(*DataP);
		BillboardInterface->OnBillboardMarqueeShow.Broadcast(FTapBillboardMarquee(*DataP), FTapBillboardMarqueeConfig(BillboardInterface->GetMarqueeStyle()));
	}
	else
	{
		CurrentLoop.Reset();
	}
	
	if (CurrentLoop)
	{
		LeftLoopCount = BillboardInterface->GetMarqueeStyle().repeat_count;
		ShowMarqueeItem(*CurrentLoop, LeftLoopCount == 1);//还剩最后一次的时候发送已读
		return true;
	}// else 正在显示的等待结束
	return false;
}

void UTapBillboardMarquee::ShowMarqueeItem(const FAnnouncementDetailData& DetailData, bool bMarkReadWhenFinished)
{
	FText Content;
	TArray<FMarqueeContentElement> Results;
	if (FJsonObjectConverter::JsonArrayStringToUStruct(DetailData.content, &Results, 0, 0))
	{
		FString ContentStr;
		for (const FMarqueeContentElement& Elm : Results)
		{
			if (Elm.type == TEXT("paragraph"))
			{
				for (const FMarqueeContentChild& Child : Elm.children)
				{
					ContentStr += Child.text;
				}
			}
		}
		Content = FText::FromString(ContentStr);
	}

	if (ensure(ItemClass))
	{
		if (UTapMarqueeItem* UI = CreateWidget<UTapMarqueeItem>(this, ItemClass))
		{
			UI->UpdateItem(Content, ItemPanel->GetCachedGeometry().GetLocalSize().X, DetailData.id, bMarkReadWhenFinished);

			const FAnnouncementStyleData& StyleData = BillboardInterface->GetMarqueeStyle();
			UI->UpdateStyle(StyleData);
			UI->UpdateFont(DFont);
			UI->OutsideOfParent.BindUObject(this, &UTapBillboardMarquee::OnItemClipped);
			ItemPanel->RemoveChild(PlaceHolderRight);
			if (UScrollBoxSlot* NewSlot = Cast<UScrollBoxSlot>(ItemPanel->AddChild(UI)))
			{
				Offset = 0.f;
				NewSlot->SetHorizontalAlignment(HAlign_Fill);
				NewSlot->SetVerticalAlignment(VAlign_Fill);
				
				ItemPanel->AddChild(PlaceHolderRight);
				float SizeX = ItemPanel->GetCachedGeometry().Size.X;
				PlaceHolderRight->SetSize(FVector2D(SizeX + 100.f, 1.f));
			}
			else
			{
				ensure(false);
			}

			ScrollingIds.Add(DetailData.id);
		}
	}
}

void UTapBillboardMarquee::OnItemClipped(UTapMarqueeItem* Item)
{
	Item->OutsideOfParent.Unbind();
	auto MarkReadID = Item->MarkReadID;
	Item->RemoveFromParent();
	--LeftLoopCount;
	if (LeftLoopCount == 0)
	{
		ScrollingIds.RemoveSingle(MarkReadID);
		if (NextAnnouncement() == false) {
			if (ItemPanel->GetChildrenCount() == 2)
			{
				ensure(ItemPanel->GetChildAt(0) == PlaceHolder && ItemPanel->GetChildAt(ItemPanel->GetChildrenCount() - 1) == PlaceHolderRight);
				StopAnnouncementLoop();
				RemoveFromParent();
			}
		}
	}
	else
	{
		ShowMarqueeItem(*CurrentLoop, LeftLoopCount == 1);
	}
}

void UTapBillboardMarquee::OnOrientationChanged(int32 Orientation)
{
	const FAnnouncementStyleData& StyleDate = BillboardInterface->GetMarqueeStyle();
	UpdateStyle(StyleDate);
	for (int32 i = 0; i < ItemPanel->GetChildrenCount(); ++i)
	{
		if (UTapMarqueeItem* Item = Cast<UTapMarqueeItem>(ItemPanel->GetChildAt(i)))
		{
			Item->UpdateStyle(StyleDate);
		}
	}
}

void UTapBillboardMarquee::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	BillboardInterface = StaticCastSharedPtr<FTapBillboardPC>(FTapBillboardModule::GetTapBillboardInterface());
	auto StyleData = BillboardInterface->GetMarqueeStyle();
	UpdateStyle(StyleData);
	FTapUrlResourceLoader::LoadFileData(StyleData.custom_font, FTapUrlResourceLoaderProgressDelegate(),
	                                    FTapUrlResourceLoaderCompletedDelegate::CreateUObject(this, &UTapBillboardMarquee::DownloadFont));
	UpdateScrollSpeed(BillboardInterface->GetMarqueeSpeed());

	FCoreDelegates::ApplicationReceivedScreenOrientationChangedNotificationDelegate.AddUObject(this, &UTapBillboardMarquee::OnOrientationChanged);
}

void UTapBillboardMarquee::NativeConstruct()
{
	Super::NativeConstruct();
	GetWorld()->GetTimerManager().SetTimer(DelayTimer, this, &UTapBillboardMarquee::StartAnnouncementLoop, 0.5f);
}

void UTapBillboardMarquee::NativeDestruct()
{
	Super::NativeDestruct();
	StopAnnouncementLoop();
	BillboardInterface->OnBillboardMarqueeClose.Broadcast();
}

void UTapBillboardMarquee::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	float SizeX = ItemPanel->GetCachedGeometry().Size.X;
#if ENGINE_MAJOR_VERSION > 4
	if (SizeX != PlaceHolder->GetSize().X)
#else
	if (SizeX != PlaceHolder->Size.X)
#endif
	{
		PlaceHolder->SetSize(FVector2D(SizeX, 1.f));
	}
#if ENGINE_MAJOR_VERSION > 4
	if (SizeX + 100.f != PlaceHolderRight->GetSize().X)
#else
	if (SizeX + 100.f != PlaceHolderRight->Size.X)
#endif
	{
		PlaceHolderRight->SetSize(FVector2D(SizeX + 100.f, 1.f));
	}

	Offset += ScrollSpeed * InDeltaTime;
	ItemPanel->SetScrollOffset(Offset);
}

void UTapBillboardMarquee::UpdateFont(UFont* Font) {
	DFont = Font;
	for (UWidget* Child : ItemPanel->GetAllChildren()) {
		// Child
		UTapMarqueeItem* Item = Cast<UTapMarqueeItem>(Child);
		// Item->UpdateFont();
	}
}

void UTapBillboardMarquee::DownloadFont(const TArray<uint8>& Buffer, bool bIsLocal) {
	if (Buffer.Num() == 0) {
		return;
	}
	TArray<uint8> Data = Buffer;
	UFontFace* FontFace = NewObject<UFontFace>();
	FontFace->LoadingPolicy = EFontLoadingPolicy::Inline;
	FontFace->FontFaceData = FFontFaceData::MakeFontFaceData(MoveTemp(Data));

	DFont = NewObject<UFont>(GetTransientPackage(), NAME_None, RF_Transient);
	DFont->FontCacheType = EFontCacheType::Runtime;
	FTypefaceEntry& TypefaceEntry = DFont->CompositeFont.DefaultTypeface.Fonts.AddDefaulted_GetRef();
	TypefaceEntry.Font = FFontData(FontFace);
	for (UWidget* Child : ItemPanel->GetAllChildren()) {
		UTapMarqueeItem* Item = Cast<UTapMarqueeItem>(Child);
		if (Item) {
			Item->UpdateFont(DFont);
		}
	}
}

