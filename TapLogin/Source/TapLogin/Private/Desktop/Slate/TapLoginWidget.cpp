// Fill out your copyright notice in the Description page of Project Settings.


#include "TapLoginWidget.h"

#include "SlateOptMacros.h"
#include "TapCommon.h"
#include "Slate/TapStyle.h"
#include "Slate/Components/TapRoundCornersBorder.h"
#include "Slate/Components/TapRoundCornersImage.h"
#include "Styles/TapLoginWidgetStyle.h"
#include "Styling/SlateStyle.h"

#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Text/SRichTextBlock.h"

#include "Desktop/TULoginLanguage.h"
#include "TapLogin.h"
#include "TapSubsystem.h"
#include "TUHelper.h"
#include "TULoginImpl.h"
#include "TUType.h"
#include "Components/QrCodeScanTipButton.h"
#include "Desktop/Server/TULoginNet.h"
#include "Slate/Widgets/TapToast.h"
#include "Widgets/Layout/SScaleBox.h"
#include "TUAuthResult.h"
#include "Controller/TapControllerManager.h"
#include "Controller/TapControllerTip.h"
#include "Desktop/TUWebAuthHelper.h"
#include "Slate/Components/STapButton.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "TapTap"

#define SCAN_TIP_URL_iOS_CN "https://sdk-assets.tds1.tapfiles.cn/scan-guide/ios.png"
#define SCAN_TIP_URL_iOS_IO "https://sdk-assets.ap-sg.tdsfiles.com/scan-guide/ios.png"
#define SCAN_TIP_URL_Android_CN "https://sdk-assets.tds1.tapfiles.cn/scan-guide/android.png"
#define SCAN_TIP_URL_Android_IO "https://sdk-assets.ap-sg.tdsfiles.com/scan-guide/android.png"

STapLoginWidget::STapLoginWidget()
{
	ScanTipBrush.SetImageSize(FVector2D(160.f, 160.f));
	
	QrCodeBrush.SetImageSize(FVector2D(160.f, 160.f));
	QrCodeBrush.SetResourceObject(QrCodeTexture);

	bCanSupportFocus = true;

	PermissionDisplaySettings = {
		{TUType::PermissionScope::Profile, LOCTEXT("Profile", "头像、昵称")},
		{TUType::PermissionScope::Friend, LOCTEXT("Friend", "好友关系")},
		{TUType::PermissionScope::Email, LOCTEXT("Email", "邮件")},
		{TEXT("phone"), LOCTEXT("Phone", "手机号")},
	};
}

STapLoginWidget::~STapLoginWidget()
{
	FTapControllerManager::Get().UnregisterWidget(this);
}

void STapLoginWidget::Construct(const FArguments& InArgs, const TArray<FString>& InPermissions, ERegionType Region)
{
	Permissions = InPermissions;
	bRequestProfile = InArgs._bRequestProfile;

	OnTapAuthComplete = InArgs._OnAuthComplete;
	
	TSharedPtr<FSlateStyleSet> StyleSet = FModuleManager::GetModuleChecked<FTapLoginModule>("TapLogin").Style;
	const FTapLoginStyle& WidgetStyle = StyleSet->GetWidgetStyle<FTapLoginStyle>(Region == ERegionType::CN ? "TapLoginStyleCN" : "TapLoginStyleGlobal");

	TSharedRef<STapControllerTip> Tip = SNew(STapControllerTip).RenderTransform(FSlateRenderTransform(FVector2D(0.f, 42.f)));
	
	ChildSlot
	[
		SNew(SConstraintCanvas)
		+SConstraintCanvas::Slot()
		.Alignment(0.5f)
		.Anchors(0.5f)
		.Offset(FMargin(0.f, 0.f, 800.f, 480.f))
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			[
				SNew(STapRoundCornersImage)
			]
			+SOverlay::Slot()
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(WidgetStyle.TitlePadding)
				[
					SNew(SRichTextBlock)//title
					.Justification(ETextJustify::Center)
					.Decorators({SRichTextBlock::ImageDecorator()})
					.DecoratorStyleSet(StyleSet.Get())
					.TextStyle(&WidgetStyle.TitleTextBlockStyle)
					.Text(FText::Format(LOCTEXT("TapLoginTitleFormat", "{0} TapTap {1}"),
						FText::FromString(TULoginLanguage::GetCurrentLang()->TitleUse()),
						FText::FromString(TULoginLanguage::GetCurrentLang()->TitleLogin())))
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()//left
					.FillWidth(1.f)
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.TextStyle(&WidgetStyle.SubtitleTextBlockStyle)
							.Text(FText::FromString(TULoginLanguage::GetCurrentLang()->QrTitleLogin()))
							.Visibility(Region == ERegionType::CN ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed)
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.Padding(WidgetStyle.QrCodePadding)
						[
							SNew(SOverlay)
							+SOverlay::Slot()
							[
								SNew(STapRoundCornersBorder)
								.CornerRadius(16.f)
								.Padding(10.f)
								.BorderBackgroundColor(FColor::FromHex(TEXT("#F4F4F4")))
								[
									SNew(STapRoundCornersBorder)
									.CornerRadius(10.f)
									.Padding(10.f)
									.BorderBackgroundColor(FColor::White)
									[
										SAssignNew(QrCodeImage, SImage)
										.Image(&QrCodeBrush)
									]
								]
							]
							+SOverlay::Slot()
							.Expose(RetrySlot)
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							SNew(SRichTextBlock)
							.TextStyle(&WidgetStyle.QrCodeDescriptionStyle)
							.DecoratorStyleSet(StyleSet.Get())
							.Justification(ETextJustify::Center)
							.Text(FText::FromString(TULoginLanguage::GetCurrentLang()->QrNoticeScanToLogin()))
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.Padding(WidgetStyle.QrCodeDescription2Padding)
						[
							SNew(SRichTextBlock)
							.TextStyle(&WidgetStyle.QrCodeDescriptionStyle2)
							.DecoratorStyleSet(StyleSet.Get())
							.Justification(ETextJustify::Center)
							.Text(FText::FromString(TULoginLanguage::GetCurrentLang()->QrNoticeScanToLogin2()))
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.Padding(0.f)
						[
							SNew(SHorizontalBox)
							+SHorizontalBox::Slot()
							.AutoWidth()
							[
								SAssignNew(IOSButton, SQrCodeScanTipButton, LOCTEXT("iOS", "iOS"), Region == ERegionType::CN ? TEXT(SCAN_TIP_URL_iOS_CN) : TEXT(SCAN_TIP_URL_iOS_IO))
								.OnRequestTip(this, &STapLoginWidget::OnRequestScanTipPanel)
								.OnRequestClose(this, &STapLoginWidget::OnRequestCloseScanTipPanel)
								.Style(&WidgetStyle.QrCodeScanTipButtonStyle)
								.bShowIcon(Region == ERegionType::CN)
							]
							+SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(12.f, 0.f, 0.f, 0.f)
							[
								SAssignNew(AndroidButton, SQrCodeScanTipButton, LOCTEXT("Android", "Android"), Region == ERegionType::CN ? TEXT(SCAN_TIP_URL_Android_CN) : TEXT(SCAN_TIP_URL_Android_IO))
								.OnRequestTip(this, &STapLoginWidget::OnRequestScanTipPanel)
								.OnRequestClose(this, &STapLoginWidget::OnRequestCloseScanTipPanel)
								.Style(&WidgetStyle.QrCodeScanTipButtonStyle)
								.bShowIcon(Region == ERegionType::CN)
							]
						]
					]
					+SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(0.f, 0.f, 0.f, 18.f)
					[
						SNew(SImage)
						.ColorAndOpacity(FColor::FromHex(TEXT("#F5F5F5")))
						.Image(FTapStyle::GetUnitBrush())
					]
					+SHorizontalBox::Slot()//right
					.FillWidth(1.f)
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.TextStyle(&WidgetStyle.SubtitleTextBlockStyle)
							.Text(FText::FromString(TULoginLanguage::GetCurrentLang()->WebLogin()))
							.Visibility(Region == ERegionType::CN ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed)
						]
						+SVerticalBox::Slot()
						.HAlign(HAlign_Center)
						.AutoHeight()
						.Padding(WidgetStyle.NoPermissionPadding)
						[
							SNew(SImage)
							.Image(&WidgetStyle.NoPermissionBrush)
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.Padding(WidgetStyle.WebAuthButtonPadding)
						.HAlign(HAlign_Center)
						[
							SNew(SBox)
							.WidthOverride(230.f)
							.HeightOverride(40.f)
							[
								SAssignNew(WebButton, STapButton)
								.StyleType(Region == ERegionType::CN ? ETapButtonStyleType::Blue : ETapButtonStyleType::Black)
								.OnClicked(this, &STapLoginWidget::OnWebAuthButtonClicked)
								.ContentPadding(FMargin(22.f, 0.f))
								.HAlign(WidgetStyle.WebAuthHAlignment)
								.VAlign(VAlign_Center)
								[
									SNew(SHorizontalBox)
									+SHorizontalBox::Slot()
									.FillWidth(1.f)
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Left)
									[
										SNew(STextBlock)
										.TextStyle(&WidgetStyle.WebAuthTextBlockStyle)
										.Text(FText::FromString(TULoginLanguage::GetCurrentLang()->WebButtonJumpToWeb()))
									]
									+SHorizontalBox::Slot()
									.AutoWidth()
									.Padding(8.f, 0.f, 0.f, 0.f)
									.VAlign(VAlign_Center)
									[
										SNew(SImage)
										.Image(&WidgetStyle.WebAuthArrow)
									]
								]
							]
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							Region == ERegionType::CN
							? SNullWidget::NullWidget
							: SNew(STextBlock)
							.TextStyle(&WidgetStyle.WebAuthTipTextBlockStyle)
							.Text(FText::FromString(TULoginLanguage::GetCurrentLang()->WebNotice()))
							.Justification(ETextJustify::Center)
						]
						+SVerticalBox::Slot()
						.Padding(WidgetStyle.WebAuthTip2Padding)
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.TextStyle(&WidgetStyle.WebAuthTipTextBlockStyle2)
							.Text(FText::FromString(TULoginLanguage::GetCurrentLang()->WebNotice2()))
							.Justification(ETextJustify::Center)
						]
						+SVerticalBox::Slot()
						.Expose(PermissionSlot)
					]
				]
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Top)
			.Padding(14.f, 12.f)
			[
				SAssignNew(CloseButton, STapButton)
				.StyleType(ETapButtonStyleType::SmallClose)
				.OnClicked(this, &STapLoginWidget::OnCloseButtonClicked)
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)
			[
				Tip
			]
		]
		+SConstraintCanvas::Slot()
		.Alignment(FVector2D(1.f, 0.82f))
		.Anchors(0.5f)
		.Offset(FMargin(-290.f, 180.f, 444.f, 519.f))
		[
			SAssignNew(TipBorder, SBorder)
			.Visibility(EVisibility::Collapsed)
			.Padding(22.f + 25.f)
			.BorderImage(&WidgetStyle.TipContainerBrush)
			[
				SNew(SScaleBox)
				.Stretch(EStretch::ScaleToFit)
				[
					SAssignNew(TipImage, SImage)
					.Image(&ScanTipBrush)
				]
			]
		]
	];

	// Handle retry button
	SAssignNew(RetryOverlay, SOverlay)
	+ SOverlay::Slot()
	.Padding(20.f)
	[
		SNew(SImage)
		.Image(&WidgetStyle.RetryBrush)
	]
	+ SOverlay::Slot()
	  .VAlign(VAlign_Center)
	  .HAlign(HAlign_Center)
	[
		SAssignNew(RefreshButton, STapButton)
		.StyleType(Region == ERegionType::CN ? ETapButtonStyleType::RefreshBlue : ETapButtonStyleType::RefreshBlack)
		.ContentPadding(WidgetStyle.RetryContentPadding)
		.OnClicked(this, &STapLoginWidget::OnQrCodeRefreshClicked)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Center)
			  .AutoWidth()
			[
				SNew(SImage)
				.Image(&WidgetStyle.QrCodeRetryIconBrush)
			]
			+ SHorizontalBox::Slot()
			  .Padding(6.f, 0.f, 0.f, 0.f)
			  .VAlign(VAlign_Center)
			  .AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TULoginLanguage::GetCurrentLang()->QrRefresh()))
				.TextStyle(&WidgetStyle.QrCodeRetryTextBlockStyle)
			]
		]
	];

	RefreshQrCode();

	TSharedRef<FNavigationMetaData> NewMetaData = MakeShared<FNavigationMetaData>();
	WebButton->AddMetadata(NewMetaData);
	NewMetaData->SetNavigationCustom(EUINavigation::Left, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(this, &STapLoginWidget::OnWebButtonNavigation));
	FTapUINavigationBuilder(WebButton.ToSharedRef())
		.Up(CloseButton);
	FTapUINavigationBuilder(CloseButton.ToSharedRef())
		.Down(WebButton);
	FTapUINavigationBuilder(AndroidButton.ToSharedRef())
		.Right(WebButton);
	FTapUINavigationBuilder(RefreshButton.ToSharedRef())
		.Right(WebButton);
	TSharedRef<FTapControllerWidgetData> NewData = MakeShared<FTapControllerWidgetData>(AsShared());
	NewData->StartupFocusWidget = WebButton;
	NewData->OnBackButtonClicked.BindSP(this, &STapLoginWidget::OnBackButtonClicked);
	NewData->ControllerTip = Tip;
	FTapControllerManager::Get().RegisterWidget(NewData);
}

void STapLoginWidget::RefreshQrCode()
{
	TWeakPtr<STapLoginWidget> WeakSelf(StaticCastSharedRef<STapLoginWidget>(AsShared()));
	TULoginNet::RequestLoginQrCode(
		Permissions,
		[=](TSharedPtr<FTUQrCodeModel> Model, FTULoginError Error)
		{
			if (TSharedPtr<STapLoginWidget> UI = WeakSelf.Pin())
			{
				if (Model.IsValid())
				{
					QrCodeModel = Model;
					UI->StartCheckQrCode();
					UI->RefreshQrCodeBrush();
				}
				else
				{
					UI->ShowQrCodeRetryButton(true);
				}
			}
		});
}

void STapLoginWidget::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(ScanTipTextureIOS);
	Collector.AddReferencedObject(ScanTipTextureAndroid);
	Collector.AddReferencedObject(QrCodeTexture);
}

FReply STapLoginWidget::OnCloseButtonClicked()
{
	Close(TUAuthResult::CancelInit());
	return FReply::Handled();
}

FReply STapLoginWidget::OnWebAuthButtonClicked()
{
	if (!WebAuthHelper.IsValid())
	{
		WebAuthHelper = MakeShared<TUWebAuthHelper>();
	}
	if (WebAuthHelper->ProcessWebAuth(
		Permissions,
		[=](FString WebCode)
		{
			GetTokenFromWebCode(WebCode);
		}))
	{
		STapToast::Show(ETapToastType::Info, FText::FromString(TULoginLanguage::GetCurrentLang()->WebNoticeLogin()));
	}
	return FReply::Handled();
}

FReply STapLoginWidget::OnQrCodeRefreshClicked()
{
	RefreshQrCode();
	return FReply::Handled();
}

FString STapLoginWidget::GetReferencerName() const
{
	return TEXT("STapLoginWidget");
}

void STapLoginWidget::Close(const TUAuthResult& Result)
{
	if (WebAuthHelper.IsValid()) {
		WebAuthHelper->StopProcess();
		WebAuthHelper = nullptr;
	}
	
	StopCheckQrCode();
	UTapSubsystem::RemoveWidget(AsShared());
	OnTapAuthComplete.ExecuteIfBound(Result);
}

void STapLoginWidget::GetProfile(const TSharedPtr<FTUAccessToken>& AccessToken)
{
	TWeakPtr<STapLoginWidget> WeakSelf(StaticCastSharedRef<STapLoginWidget>(AsShared()));
	ensure(bRequestProfile);
	TULoginNet::RequestProfile(
		*AccessToken.Get(),
		[AccessToken, WeakSelf](TSharedPtr<FTULoginProfileModel> Model, FTULoginError Error)
		{
			if (TSharedPtr<STapLoginWidget> UI = WeakSelf.Pin())
			{
				if (Model.IsValid())
				{
					AccessToken->SaveToLocal();
					Model->SaveToLocal();
					// STapToast::Show(ETapToastType::TapTap, FText::FromString(FString::Format(*TULoginLanguage::GetCurrentLang()->login_account_logged_tip(), {Model->name})), Model->avatar);
					UI->Close(TUAuthResult::SuccessInit(AccessToken));
				}
				else
				{
					FTUError TapError;
					TapError.code = Error.code;
					TapError.error_description = Error.error_description + ".\t" + "Get profile error";
					UI->Close(TUAuthResult::FailInit(TapError));
				}
			}
		});
}

void STapLoginWidget::GetTokenFromWebCode(const FString& WebCode)
{
	if (!WebAuthHelper.IsValid())
	{
		UE_LOG(LogTap, Warning, TEXT("WebAuthHelper is invalid, please check code"));
		return;
	}
	TSharedPtr<FJsonObject> Paras = MakeShareable(new FJsonObject);
	Paras->SetStringField("client_id", TULoginImpl::Get()->Config.ClientID);
	Paras->SetStringField("grant_type", "authorization_code");
	Paras->SetStringField("secret_type", "hmac-sha-1");
	Paras->SetStringField("code", WebCode);
	Paras->SetStringField("redirect_uri", WebAuthHelper->RedirectUri);
	Paras->SetStringField("code_verifier", WebAuthHelper->GetCodeVerifier());
	
	TWeakPtr<STapLoginWidget> WeakSelf(StaticCastSharedRef<STapLoginWidget>(AsShared()));
	TULoginNet::RequestAccessTokenFromWeb(
		Paras,
		[=](TSharedPtr<FTUAccessToken> Model, FTULoginError Error) {
		if (TSharedPtr<STapLoginWidget> UI = WeakSelf.Pin())
		{
			if (Model.IsValid())
			{
				if (UI->bRequestProfile)
				{
					UI->GetProfile(Model);
				}
				else
				{
					UI->Close(TUAuthResult::SuccessInit(Model));
				}
			}
			else {
				if (UI->bRequestProfile)
				{
					UE_LOG(LogTap, Warning, TEXT("web login fail"));
					STapToast::Show(ETapToastType::Success, FText::Format(LOCTEXT("ScanFailFormat", "{0} {1}"),
						FText::FromString(TULoginLanguage::GetCurrentLang()->WebNoticeFail()),
						FText::FromString(TULoginLanguage::GetCurrentLang()->WebNoticeFail2())));
				}
				else
				{
					FTUError TapError;
					TapError.code = Error.code;
					TapError.error_description = Error.error_description;
					UI->Close(TUAuthResult::FailInit(TapError));
				}
			}
		}
	});
}

void STapLoginWidget::OnRequestScanTipPanel(const FString& Url, TSharedRef<SQrCodeScanTipButton> FireButton)
{
	bool bAndroid = AndroidButton == FireButton;
	UTexture2D*& Tex = bAndroid ? ScanTipTextureAndroid : ScanTipTextureIOS;
	if (Tex)
	{
		ScanTipBrush.SetImageSize(FVector2D(Tex->GetSizeX(), Tex->GetSizeY()));
		ScanTipBrush.SetResourceObject(Tex);
	}
	else
	{
		FTapCommonModule::AsyncDownloadImage(Url, FAsyncDownloadImage::CreateSP(this, &STapLoginWidget::UpdateScanTipBrush, bAndroid));
	}
	TipBorder->SetVisibility(EVisibility::SelfHitTestInvisible);
}

void STapLoginWidget::OnRequestCloseScanTipPanel()
{
	TipBorder->SetVisibility(EVisibility::Collapsed);
	ScanTipBrush.SetResourceObject(nullptr);
}

void STapLoginWidget::UpdateScanTipBrush(UTexture2D* ImageTexture, bool bAndroid)
{
	if (ImageTexture)
	{
		UTexture2D*& Tex = bAndroid ? ScanTipTextureAndroid : ScanTipTextureIOS;
		Tex = ImageTexture;
		ScanTipBrush.SetImageSize(FVector2D(Tex->GetSizeX(), Tex->GetSizeY()));
		ScanTipBrush.SetResourceObject(Tex);
	}
}

void STapLoginWidget::ShowQrCodeRetryButton(bool bShow)
{
	if (bShow)
	{
		RetrySlot->AttachWidget(RetryOverlay.ToSharedRef());
		StopCheckQrCode();
	}
	else
	{
		RetrySlot->DetachWidget();
	}
	RetryOverlay->SetVisibility(bShow ? EVisibility::Visible : EVisibility::Hidden);
}

bool STapLoginWidget::IsRetryButtonShow() const
{
	return RetrySlot->GetWidget() != SNullWidget::NullWidget;
}

void STapLoginWidget::RefreshQrCodeBrush()
{
	ShowQrCodeRetryButton(false);
	QrCodeTexture = TUHelper::GenerateQrCode(QrCodeModel->qrcode_url);
	QrCodeBrush.SetResourceObject(QrCodeTexture);
}

void STapLoginWidget::StartCheckQrCode()
{
	if (ensure(QrCodeModel))
	{
		if (CheckQcCodeTimer.IsValid())
		{
			StopCheckQrCode();
		}
#if (ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 0)
		CheckQcCodeTimer = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(this, &STapLoginWidget::TimerCheckQrCode), QrCodeModel->interval);
#else
		CheckQcCodeTimer = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(this, &STapLoginWidget::TimerCheckQrCode), QrCodeModel->interval);
#endif
	}
}

void STapLoginWidget::StopCheckQrCode()
{
#if (ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 0)
	FTSTicker::GetCoreTicker().RemoveTicker(CheckQcCodeTimer);
#else
	FTicker::GetCoreTicker().RemoveTicker(CheckQcCodeTimer);
#endif
	CheckQcCodeTimer.Reset();
}

bool STapLoginWidget::TimerCheckQrCode(float DeltaSecond)
{
	int64 ExpireAt = FDateTime::UtcNow().ToUnixTimestamp() + QrCodeModel->expires_in;
	int64 Now = FDateTime::UtcNow().ToUnixTimestamp();
	if (Now > ExpireAt || !QrCodeModel.IsValid())
	{
		ShowQrCodeRetryButton(true);
		return false;
	}

	TWeakPtr<STapLoginWidget> WeakSelf(StaticCastSharedRef<STapLoginWidget>(AsShared()));
	TULoginNet::RequestAccessToken(
		QrCodeModel->device_code,
		[WeakSelf](TSharedPtr<FTUAccessToken> Model, FTULoginError Error)
		{
			if (TSharedPtr<STapLoginWidget> UI = WeakSelf.Pin())
			{
				if (Model.IsValid())
				{
					if (!UI->bRequestProfile)
					{
						UI->Close(TUAuthResult::SuccessInit(Model));
						return;
					}
					UI->GetProfile(Model);
					UI->StopCheckQrCode();
				}
				else if(Error.error != "slow_down")
				{
					if (UI->ScanState == Error.error)
					{
						return;
					}
					UI->ScanState = Error.error;
					if (Error.error == "authorization_pending")
					{
					}
					else if (Error.error == "authorization_waiting")
					{
						STapToast::Show(ETapToastType::Success, FText::Format(LOCTEXT("ScanSuccessFormat", "{0} {1}"),
							FText::FromString(TULoginLanguage::GetCurrentLang()->QrnNoticeSuccess()),
							FText::FromString(TULoginLanguage::GetCurrentLang()->QrnNoticeSuccess2())));
					}
					else if (Error.error == "access_denied")
					{
						STapToast::Show(ETapToastType::Warning, FText::Format(LOCTEXT("ScanCancelFormat", "{0} {1}"),
							FText::FromString(TULoginLanguage::GetCurrentLang()->QrNoticeCancel()),
							FText::FromString(TULoginLanguage::GetCurrentLang()->QrNoticeCancel2())));
						UI->StopCheckQrCode();
						UI->RefreshQrCode();
					}
					else if (Error.error == "invalid_grant")
					{
						UI->ShowQrCodeRetryButton(true);
					}
					else
					{
						UI->ShowQrCodeRetryButton(true);
					}
				}
			}
		});
	return true;
}

FNavigationReply STapLoginWidget::OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent)
{
	return FNavigationReply::Stop();
}

TSharedPtr<SWidget> STapLoginWidget::OnWebButtonNavigation(EUINavigation Direction)
{
	ensure(Direction == EUINavigation::Left);
	if (RetryOverlay->GetParentWidget().IsValid())
	{
		return RefreshButton;
	}
	else
	{
		return AndroidButton;
	}
}

void STapLoginWidget::OnBackButtonClicked()
{
	OnCloseButtonClicked();
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
