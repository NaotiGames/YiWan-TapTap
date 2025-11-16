// Fill out your copyright notice in the Description page of Project Settings.


#include "TapBillboard.h"

#include "IOSHelper.h"
#include "TUHelper.h"

@interface TapBillboardDelegateImpl : NSObject <TapBillboardOutputStateDelegate, TapBillboardMarqueeDelegate>

@property (nonatomic, strong) dispatch_block_t playVoiceBlock;
@property (nonatomic, strong) dispatch_block_t stopVoiceBlock;
@property (nonatomic, strong) dispatch_block_t marqueeCloseBlock;
@property (nonatomic, strong) void(^marqueeShowBlock)(TapBillboardMarquee *marquee, TapBillboardMarqueeConfig *config);
@property (nonatomic, strong) TTBillboardMessageListener billboardMessageListener;
@end

@implementation TapBillboardDelegateImpl

- (void)onPlayVoice {
	if (self.playVoiceBlock) {
		self.playVoiceBlock();
	}
}

- (void)onStopVoice {
	if (self.stopVoiceBlock) {
		self.stopVoiceBlock();
	}
}

- (void)onClose { 
	if (self.marqueeCloseBlock) {
		self.marqueeCloseBlock();
	}
}

- (void)onShow:(nonnull TapBillboardMarquee *)marquee config:(nonnull TapBillboardMarqueeConfig *)config { 
	if (self.marqueeShowBlock) {
		self.marqueeShowBlock(marquee, config);
	}
}

@end

static TapBillboardDelegateImpl *BillboardDelegateImpl = nil;

FTapBillboardMarquee ConverMarquee(TapBillboardMarquee *marquee) {
	FTapBillboardMarquee MarqueeUE;
	MarqueeUE.Id = marquee._id;
	MarqueeUE.Content = IOSHelper::Convert(marquee.content);
	MarqueeUE.Type = IOSHelper::Convert(marquee.type);
	MarqueeUE.Template = IOSHelper::Convert(marquee._template);
	MarqueeUE.ShortTitle = IOSHelper::Convert(marquee.shortTitle);
	MarqueeUE.LongTitle = IOSHelper::Convert(marquee.longTitle);
	MarqueeUE.JumpLocation = IOSHelper::Convert(marquee.jumpLocation);
	MarqueeUE.JumpLink = IOSHelper::Convert(marquee.jumpLink);
	MarqueeUE.PublishTime = marquee.publishTime;
	MarqueeUE.ExpireTime = marquee.expireTime;
	return MoveTemp(MarqueeUE);
}

FTapBillboardMarqueeOrientationConfig ConverMarqueeOrientationConfig(TapBillboardMarqueeOrientationConfig *marqueeOrientationConfig) {
	FTapBillboardMarqueeOrientationConfig MarqueeOrientationConfigUE;
	MarqueeOrientationConfigUE.ScrollWidth = marqueeOrientationConfig.scrollWidth;
	MarqueeOrientationConfigUE.HorizontalOffset = marqueeOrientationConfig.horizontalOffset;
	MarqueeOrientationConfigUE.VerticalOffset = marqueeOrientationConfig.verticalOffset;
	MarqueeOrientationConfigUE.HorizontalScrollMargin = marqueeOrientationConfig.horizontalScrollMargin;
	MarqueeOrientationConfigUE.VerticalScrollMargin = marqueeOrientationConfig.verticalScrollMargin;
	switch (marqueeOrientationConfig.position) {
	case TapBillboardMarqueeOrientationPositionTop:
		MarqueeOrientationConfigUE.Position = ETapBillboardMarqueeOrientationPosition::Top;
		break;
	case TapBillboardMarqueeOrientationPositionTopLeft:
		MarqueeOrientationConfigUE.Position = ETapBillboardMarqueeOrientationPosition::TopLeft;
		break;
	case TapBillboardMarqueeOrientationPositionTopRight:
		MarqueeOrientationConfigUE.Position = ETapBillboardMarqueeOrientationPosition::TopRight;
		break;
	case TapBillboardMarqueeOrientationPositionBottom:
		MarqueeOrientationConfigUE.Position = ETapBillboardMarqueeOrientationPosition::Bottom;
		break;
	case TapBillboardMarqueeOrientationPositionBottomLeft:
		MarqueeOrientationConfigUE.Position = ETapBillboardMarqueeOrientationPosition::BottomLeft;
		break;
	case TapBillboardMarqueeOrientationPositionBottomRight:
		MarqueeOrientationConfigUE.Position = ETapBillboardMarqueeOrientationPosition::BottomRight;
		break;
	}
	switch (marqueeOrientationConfig.scrollWidthType) {
	case TapBillboardMarqueeOrientationScrollWidthTypeFill:
		MarqueeOrientationConfigUE.ScrollWidthType = ETapBillboardMarqueeOrientationScrollWidthType::Fill;
		break;
	case TapBillboardMarqueeOrientationScrollWidthTypeFixed:
		MarqueeOrientationConfigUE.ScrollWidthType = ETapBillboardMarqueeOrientationScrollWidthType::Fixed;
		break;
	}
	return MoveTemp(MarqueeOrientationConfigUE);
}

FTapBillboardMarqueeConfig ConverMarqueeConfig(TapBillboardMarqueeConfig *marqueeConfig) {
	FTapBillboardMarqueeConfig MarqueeConfigUE;
	MarqueeConfigUE.DefaultTextColor = IOSHelper::Convert(marqueeConfig.defaultTextColor);
	MarqueeConfigUE.ContentBackgroundColor = IOSHelper::Convert(marqueeConfig.contentBackgroundColor);
	MarqueeConfigUE.CustomFont = IOSHelper::Convert(marqueeConfig.customFont);
	MarqueeConfigUE.bUseSystemFont = marqueeConfig.useSystemFont;
	MarqueeConfigUE.RepeatCount = marqueeConfig.repeatCount;
	MarqueeConfigUE.Icon.Url = IOSHelper::Convert(marqueeConfig.icon.url);
	MarqueeConfigUE.Horizontal = ConverMarqueeOrientationConfig(marqueeConfig.horizontal);
	MarqueeConfigUE.Vertical = ConverMarqueeOrientationConfig(marqueeConfig.vertical);
	return MoveTemp(MarqueeConfigUE);
}


FTapBillboard::FTapBillboard()
{
	BillboardDelegateImpl = [TapBillboardDelegateImpl new];
    
	BillboardDelegateImpl.playVoiceBlock = ^ () {
		TUHelper::PerformOnGameThread([this](){ OnAudioOutputStateChanged.ExecuteIfBound(true); });
	};
	BillboardDelegateImpl.stopVoiceBlock = ^ () {
		TUHelper::PerformOnGameThread([this](){ OnAudioOutputStateChanged.ExecuteIfBound(false); });
	};
    
	BillboardDelegateImpl.marqueeCloseBlock = ^ () {
		TUHelper::PerformOnGameThread([this](){ OnBillboardMarqueeClose.Broadcast(); });
	};
	BillboardDelegateImpl.marqueeShowBlock = ^(TapBillboardMarquee *marquee, TapBillboardMarqueeConfig *config) {
		FTapBillboardMarquee MarqueeUE = ConverMarquee(marquee);
		FTapBillboardMarqueeConfig MarqueeConfigUE = ConverMarqueeConfig(config);
		TUHelper::PerformOnGameThread([=](){ OnBillboardMarqueeShow.Broadcast(MarqueeUE, MarqueeConfigUE); });
	};
	BillboardDelegateImpl.billboardMessageListener = ^(NSString * _Nullable customUrl) {
		FString Url = FString(customUrl);
		TUHelper::PerformOnGameThread([this, Url](){ OnCustomLinkClicked.Broadcast(Url); });
	};
	[TapBillboard regsiterOutputStateListener:BillboardDelegateImpl];
	[TapBillboard registerMarqueeDelegate:BillboardDelegateImpl];
	[TapBillboard registerCustomLinkListener:BillboardDelegateImpl.billboardMessageListener];
}

FTapBillboard::~FTapBillboard() {
	[TapBillboard unRegisterMarqueeDelegate:BillboardDelegateImpl];
	[TapBillboard unRegisterOutputStateListener];
	[TapBillboard unRegisterCustomLinkListener:BillboardDelegateImpl.billboardMessageListener];
	BillboardDelegateImpl = nil;
}

void FTapBillboard::Init(const FTUConfig& InConfig)
{
	if (!InConfig.BillboardConfig.IsValid()) {
		return;
	}

	NSMutableSet <NSArray *> *dimensionSet = [[NSMutableSet alloc] init];
	if (InConfig.BillboardConfig->Dimensions.Num() > 0)
	{
		for (TTuple<FString, FString>& T : InConfig.BillboardConfig->Dimensions)
		{
			[dimensionSet addObject:[NSArray arrayWithObjects:T.Key.GetNSString(), T.Value.GetNSString(), nil]];
		}
	}

	TapBillboardConfig *billboardCnConfig = [TapBillboardConfig new];
	billboardCnConfig.diemensionSet = dimensionSet; // 可选项
	billboardCnConfig.serverUrl = InConfig.BillboardConfig->BillboardUrl.GetNSString();

	TapConfig *config = [TapConfig new];
	config.clientId = InConfig.ClientID.GetNSString();
	config.clientToken = InConfig.ClientToken.GetNSString();
	config.region = InConfig.RegionType == ERegionType::CN ? TapSDKRegionTypeCN : TapSDKRegionTypeIO; 
	config.serverURL = FTUConfig::Get()->ServerURL.GetNSString();
	config.tapBillboardConfig = billboardCnConfig;
	[TapBillboard initWithConfig:config];
}

void FTapBillboard::OpenPanel(const FSimpleDelegate& OnSuccess, const FTapFailed& OnFailed, const FSimpleDelegate& OnClose)
{
	FSimpleDelegate OnSuccessCopy = OnSuccess;
	FTapFailed OnFailedCopy = OnFailed;
	FSimpleDelegate OnCloseCopy = OnClose;
	dispatch_async(dispatch_get_main_queue(), ^
	{
		[TapBillboard openPanel:^(bool _, NSError *_Nullable error)
		{
			if (error)
			{
				FTUError Error = FTUError(error.code, FString(error.localizedDescription));
				TUHelper::PerformOnGameThread([OnFailedCopy, Error](){ OnFailedCopy.ExecuteIfBound(Error); });
			}
			else
			{
				TUHelper::PerformOnGameThread([OnSuccessCopy](){ OnSuccessCopy.ExecuteIfBound(); });
			}
		}
		closeCallback:^(void){
			TUHelper::PerformOnGameThread([OnCloseCopy](){ OnCloseCopy.ExecuteIfBound(); });
		}];
	});
}

void FTapBillboard::OpenSplashPanel(const FSimpleDelegate& OnSuccess, const FTapFailed& OnFailed, const FSimpleDelegate& OnClose)
{
	FSimpleDelegate OnSuccessCopy = OnSuccess;
	FTapFailed OnFailedCopy = OnFailed;
	FSimpleDelegate OnCloseCopy = OnClose;
	dispatch_async(dispatch_get_main_queue(), ^
	{
		[TapBillboard openSplashPanel:^(bool _, NSError *_Nullable error)
		{
			if (error)
			{
				FTUError Error = FTUError(error.code, FString(error.localizedDescription));
				TUHelper::PerformOnGameThread([OnFailedCopy, Error](){ OnFailedCopy.ExecuteIfBound(Error); });
			}
			else
			{
				TUHelper::PerformOnGameThread([OnSuccessCopy](){ OnSuccessCopy.ExecuteIfBound(); });
			}
		}
		closeCallback:^(void){
			TUHelper::PerformOnGameThread([OnCloseCopy](){ OnCloseCopy.ExecuteIfBound(); });
		}];
	});
}

void FTapBillboard::CloseSplashPanel()
{
	dispatch_async(dispatch_get_main_queue(), ^
	{
		[TapBillboard closeSplashPanel];
	});
}

void FTapBillboard::StartFetchMarqueeData()
{
	dispatch_async(dispatch_get_main_queue(), ^
	{
		[TapBillboard startFetchMarqueeData];
	});
}

void FTapBillboard::StopFetchMarqueeData(bool bCloseNow)
{
	dispatch_async(dispatch_get_main_queue(), ^
	{
		[TapBillboard stopFetchMarqueeData:bCloseNow];
	});
}

void FTapBillboard::GetBadgeDetails(const FTapBadgeDetailsResult& OnSuccess, const FTapFailed& OnFailed) {
	auto OnSuccessCopy = OnSuccess;
	auto OnFailedCopy = OnFailed;
	[TapBillboard getBadgeDetails:^(BadgeDetails * _Nullable result, NSError * _Nullable error) {
		if (error) {
			FTUError ErrorUE = IOSHelper::Convert(error);
			TUHelper::PerformOnGameThread([=]() {
				OnFailedCopy.ExecuteIfBound(ErrorUE);
			});
		} else {
			FBadgeDetails BadgeDetailsUE;
			BadgeDetailsUE.close_button_img = IOSHelper::Convert(result.closeButtonImg);
			BadgeDetailsUE.show_red_dot = [result.showRedDot boolValue];
			TUHelper::PerformOnGameThread([=]() {
				OnSuccessCopy.ExecuteIfBound(BadgeDetailsUE);
			});
		}
	}];
}

void FTapBillboard::ClosePanel() {
	dispatch_async(dispatch_get_main_queue(), ^
	{
		[TapBillboard closePanel];
	});
}

