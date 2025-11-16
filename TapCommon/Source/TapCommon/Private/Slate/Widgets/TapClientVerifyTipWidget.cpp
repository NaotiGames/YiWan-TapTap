// Fill out your copyright notice in the Description page of Project Settings.


#include "TapClientVerifyTipWidget.h"

#include "SlateOptMacros.h"
#include "TapCommon.h"
#include "TUSettings.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Controller/TapControllerManager.h"
#include "Controller/TapControllerTip.h"
#include "Desktop/TapClientBridge.hpp"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/SRichTextBlock.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "TapTap"

STapClientVerifyTipWidget::~STapClientVerifyTipWidget()
{
	FTapControllerManager::Get().UnregisterWidget(this);
}


void STapClientVerifyTipWidget::Construct(const FArguments& InArgs)
{
	SavedStyleSet = FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style;
	
	const FSlateBrush* Brush =FTapCommonModule::Get().Style->GetBrush("/Controller/tap_client_connection_tip");

	const float DPIScale = FSlateApplication::Get().GetApplicationScale();
	FSlateBrush *UnderlineBrush =  new FSlateBrush();
	UnderlineBrush->TintColor = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#52575D")));  // 颜色
	UnderlineBrush->DrawAs = ESlateBrushDrawType::Image;  // 作为图片渲染
	UnderlineBrush->ImageSize = FVector2D(200, DPIScale > 1 ? DPIScale :1)  ;  // 宽度 128px，高度 2px（下划线）

	FSlateFontInfo FontInfo = FCoreStyle::Get().GetFontStyle("Monospaced");
	FTextBlockStyle BlockStyle =  FTextBlockStyle();
	BlockStyle.SetFont(FontInfo);
	BlockStyle.SetColorAndOpacity(FSlateColor(FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#52575D")))));
	BlockStyle.SetFontSize(10);
	
	FTextBlockStyle ContentBlockStyle =  FTextBlockStyle();
	ContentBlockStyle.SetFont(FontInfo);
	ContentBlockStyle.SetColorAndOpacity(FSlateColor(FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#1D2127")))));
	ContentBlockStyle.SetFontSize(12);
	
	
	ChildSlot
	[
		SAssignNew(Wrapper, STapBoxWrapper, 600.f, 347.f)
		.TitleText(FText::FromString(TEXT("游戏启动异常提示")))
		.bShowCloseButton(false)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot() /// Content Name
			  .Padding(0,30,0,11)
			  .AutoHeight()
			  .HAlign(HAlign_Center)
			  .VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(50)
				.HeightOverride(50)
				[
					SNew(SImage)
					.Image(Brush)
				]
			]
			+ SVerticalBox::Slot() /// Content 0
				  .Padding(0,11,0,0)
				  .AutoHeight()
				  .HAlign(HAlign_Center)
				  .VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(256)
					.HeightOverride(48)
					[
						SAssignNew(TipTextBlock, STextBlock)
						.TextStyle(&ContentBlockStyle)
						.AutoWrapText(true)
						.LineHeightPercentage(1.5)
						.Justification(ETextJustify::Center)
						.Text(FText::FromString(TEXT("发生未知错误，请从 TapTap 客户端重新启动游戏")))
					]
				]
				+ SVerticalBox::Slot() /// Content 0
					  .Padding(0,25,0,0)
					  .AutoHeight()
					  .HAlign(HAlign_Center)
					  .VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(220)
					.HeightOverride(36)
					[
						SNew(STapButton)
						.StyleType(ETapButtonStyleType::Blue)
						.Text(FText::FromString(TEXT("我知道了")))
						.OnClicked(this,&STapClientVerifyTipWidget::OnQuitClicked)
					]
				]
				+ SVerticalBox::Slot() /// Content 0
					  .Padding(0,20,0,0)
					  .AutoHeight()
					  .HAlign(HAlign_Center)
					  .VAlign(VAlign_Center)
				[
					
						SNew(SButton)
						.ButtonStyle(FCoreStyle::Get(), "NoBorder") // 移除按钮默认边框
						.OnClicked(this, &STapClientVerifyTipWidget::OnDownLoadClicked)   // 按钮点击事件
						[ 
							SNew(STextBlock)
							.AutoWrapText(false)
							.Text(FText::FromString(TEXT("我还没有安装 TapTap，点击前往下载安装")))
							.TextStyle(&BlockStyle)// 选择 UE 内置的文本样式
						]
				]
				+ SVerticalBox::Slot() 
					  .Padding(0,4,0,0)
					  .AutoHeight()
					  .HAlign(HAlign_Center)
					  .VAlign(VAlign_Center)
					[
						SNew(SBox)
						.WidthOverride(260)
						.HeightOverride(1.5)
						[
							SNew(SImage)
							.Image(UnderlineBrush)
							.Clipping(EWidgetClipping::ClipToBoundsAlways)
							
						]
					]
				
		]
	];


	

	
	// TSharedRef<FTapControllerWidgetData> NewData = MakeShared<FTapControllerWidgetData>(AsShared());
	// NewData->ControllerTip = Wrapper->GetControllerTip();
	// NewData->ControllerTip->SetForceLanguageType(ELanguageType::ZH);
	// FTapControllerManager::Get().RegisterWidget(NewData);
}


FReply STapClientVerifyTipWidget::OnQuitClicked()
{
	
	if (UGameInstance* GI = TUSettings::GetGameInstance().Get())
	{
		FGenericPlatformMisc::RequestExit(false);
		// UKismetSystemLibrary::QuitGame(GI, GI->GetFirstLocalPlayerController(), EQuitPreference::Quit, true);
		// UTapSubsystem::RemoveWidget(Box);
	}
	else
	{
		UE_LOG(LogTap, Warning, TEXT("Quit game, game instance is null."));
	}
	return FReply::Handled();
}



FReply STapClientVerifyTipWidget::OnDownLoadClicked()
{
	FString URL = TEXT("https://www.taptap.cn/mobile?utm_medium=coop&utm_source=pc_toStart");  // 设置你想要打开的链接
	FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	return FReply::Handled();
}

void STapClientVerifyTipWidget::SetInitErrorType(int ErrorType)
{
	FString ErrorTip = TEXT("发生未知错误，请从 TapTap 客户端重新启动游戏");
	if (ErrorType == TapClientBridge::TapSDKInitResult::NoPlatform) // 未找到 TapTap 客户端
	{
		ErrorTip = TEXT("获取游戏信息失败，请下载 TapTap 客户端后重新启动游戏");
	}else if(ErrorType == TapClientBridge::TapSDKInitResult::NotLaunchedByPlatform) // 未通过 TapTap 客户端启动
	{
		ErrorTip = TEXT("获取游戏信息失败，请从 TapTap 客户端重新启动游戏");
	}else if(ErrorType == TapClientBridge::TapSDKInitResult::Unknown)
	{
		ErrorTip = TEXT("本地发生未知错误，请从 TapTap 客户端重新启动游戏");
	}else if(ErrorType == TapClientBridge::TapSDKInitResult::Timeout)
	{
		ErrorTip = TEXT("获取游戏信息超时，请从 TapTap 客户端重新启动游戏");
	}
	TipTextBlock->SetText(FText::FromString(ErrorTip));
}

FNavigationReply STapClientVerifyTipWidget::OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent)
{
	return FNavigationReply::Stop();
}


#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
