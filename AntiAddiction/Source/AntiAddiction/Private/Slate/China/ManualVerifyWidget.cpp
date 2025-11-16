// Fill out your copyright notice in the Description page of Project Settings.


#include "ManualVerifyWidget.h"

#include "AntiAddiction.h"
#include "SlateOptMacros.h"
#include "TapCommon.h"
#include "TapSubsystem.h"
#include "TUDataStorage.h"
#include "Server/AAUHelper.h"
#include "Server/AAUImpl.h"
#include "Server/AAUStorage.h"
#if PLATFORM_DESKTOP
#include "Server/China/Desktop/AAUChinaImpl.h"
#include "Server/China/Desktop/AAUChinaRealName.h"
#endif
#include "Controller/TapControllerManager.h"
#include "Controller/TapControllerTip.h"
#include "Slate/Widgets/TapToast.h"
#include "Styles/ManualVerifyWidgetStyle.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Slate/Components/STapEditableTextBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "TapTap"

class SFocusableScrollBox : public SScrollBox
{
public:
	SLATE_BEGIN_ARGS(SFocusableScrollBox)
	{
	}
	SLATE_STYLE_ARGUMENT( FScrollBoxStyle, Style )
	SLATE_ARGUMENT( EOrientation, Orientation )
	SLATE_ARGUMENT( EVisibility, ScrollBarVisibility )
	SLATE_ARGUMENT(EAllowOverscroll, AllowOverscroll);
	SLATE_SLOT_ARGUMENT( FSlot, Slots )
	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs )
	{
		SScrollBox::Construct(SScrollBox::FArguments()
			.Style(InArgs._Style)
			.Orientation(InArgs._Orientation)
			.ScrollBarVisibility(InArgs._ScrollBarVisibility)
			.AllowOverscroll(InArgs._AllowOverscroll));

		for (int32 SlotIndex = 0; SlotIndex < InArgs._Slots.Num(); ++SlotIndex)
		{
			ScrollPanel->Children.AddSlot(MoveTemp(const_cast<SScrollBox::FSlot::FSlotArguments&>(InArgs._Slots[SlotIndex])));
		}
	}

	virtual bool SupportsKeyboardFocus() const override
	{
		return true;
	}
};

SManualVerifyWidget::~SManualVerifyWidget()
{
	FTapControllerManager::Get().UnregisterWidget(this);
}

void SManualVerifyWidget::Construct(const FArguments& InArgs, const TSharedRef<AAUImpl>& InImpl, const FAAUAuthIdentifyWord& Word)
{
	Impl = InImpl;
	OnClose = InArgs._OnClose;
	const FManualVerifyStyle& Style = FModuleManager::GetModuleChecked<FAntiAddictionModule>("AntiAddiction").Style->GetWidgetStyle<FManualVerifyStyle>("ManualVerifyStyle");

	ChildSlot
	[
		SAssignNew(Wrapper, STapBoxWrapper, 600.f, 347.f)
		.TitleText(FText::FromString(Word.title))
		.BlueButtonText(FText::FromString(Word.positive_button))
		.OnBlueButtonClicked(this, &SManualVerifyWidget::OnCommitButtonClicked)
		.OnCloseButtonClicked(this, &SManualVerifyWidget::OnCloseButtonClicked)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot() /// Content
			  .Padding(Style.ContentMargin)
			  .FillHeight(1.f)
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Fill)
			[
				SAssignNew(FocusableScrollBox, SFocusableScrollBox)
				.Style(&Style.ContentScrollBoxStyle)
				.Orientation(Orient_Vertical)
				.ScrollBarVisibility(EVisibility::Collapsed)
				.AllowOverscroll(EAllowOverscroll::No)
				+ SScrollBox::Slot()
				.Padding(2.f)
				[
					SNew(STextBlock)
							.TextStyle(&Style.ContentStyle)
							.Justification(ETextJustify::Left)
							.AutoWrapText(true)
							.Text(FText::FromString(AAUHelper::ReplaceHtmlTag(Word.description_plain)))
				]
			]
			+ SVerticalBox::Slot() /// Content Name
			  .Padding(Style.ContentMargin)
			  .AutoHeight()
			  .HAlign(HAlign_Center)
			  .VAlign(VAlign_Fill)
			[
				SNew(SBox)
				.HeightOverride(38.f)
				.WidthOverride(294.f)
				[
					SAssignNew(NameBox, STapEditableTextBox)
							.Style(&Style.InputBoxStyle)
							.HintText(LOCTEXT("VerifyRealName", "真实姓名"))
				]
			]
			+ SVerticalBox::Slot() /// Content Card id
			  .Padding(Style.ContentMargin)
			  .AutoHeight()
			  .HAlign(HAlign_Center)
			  .VAlign(VAlign_Fill)
			[
				SNew(SBox)
				.HeightOverride(38.f)
				.WidthOverride(294.f)
				[
					SAssignNew(CardIdBox, STapEditableTextBox)
							.Style(&Style.InputBoxStyle)
							.HintText(LOCTEXT("VerifyCardId", "请输入正确的身份证号码"))
				]
			]
		]
	];

	FSlateApplication::Get().SetKeyboardFocus(NameBox.ToSharedRef());

	TSharedPtr<STapButton> BlueButton = Wrapper->GetBlueButton();
	TSharedPtr<STapButton> CloseButton = Wrapper->GetCloseButton();

	FTapUINavigationBuilder(CloseButton.ToSharedRef()).Down(FocusableScrollBox);
	FTapUINavigationBuilder(FocusableScrollBox.ToSharedRef()).Up(CloseButton).Down(NameBox);
	FTapUINavigationBuilder(NameBox.ToSharedRef()).Up(FocusableScrollBox).Down(CardIdBox);
	FTapUINavigationBuilder(CardIdBox.ToSharedRef()).Up(NameBox).Down(BlueButton);
	
	TSharedRef<FTapControllerWidgetData> NewData = MakeShared<FTapControllerWidgetData>(AsShared());
	NewData->StartupFocusWidget = NameBox;
	NewData->ControllerTip = Wrapper->GetControllerTip();
	NewData->ControllerTip->SetForceLanguageType(ELanguageType::ZH);
	if (CloseButton)
	{
		NewData->OnBackButtonClicked.BindSP(this, &SManualVerifyWidget::OnBackButtonClicked);
	}
	FTapControllerManager::Get().RegisterWidget(NewData);
}

FReply SManualVerifyWidget::OnCommitButtonClicked()
{
	if (!Impl)
	{
		return FReply::Handled();
	}
#if PLATFORM_DESKTOP
	TSharedRef<AAUChinaImpl> CImpl = StaticCastSharedRef<AAUChinaImpl>(Impl.ToSharedRef());
	FString Name = NameBox->GetText().ToString();
	FString CardId = CardIdBox->GetText().ToString();
	FString UserId = CImpl->CurrentUserID;
	if (Name.IsEmpty())
	{
		STapToast::Show(ETapToastType::Error, LOCTEXT("Name is empty", "姓名不能为空"));
		FSlateApplication::Get().SetKeyboardFocus(NameBox, FTapControllerManager::Get().IsTapControllerRunning() ? EFocusCause::Navigation : EFocusCause::SetDirectly);
	}
	else if (CardId.IsEmpty())
	{
		STapToast::Show(ETapToastType::Error, LOCTEXT("Card id is empty", "身份信息不能为空"));
		FSlateApplication::Get().SetKeyboardFocus(CardIdBox, FTapControllerManager::Get().IsTapControllerRunning() ? EFocusCause::Navigation : EFocusCause::SetDirectly);
	}
	else
	{
		FTapCommonModule::TapThrobberShowWait();
		AAUChinaRealName::ManualVerify(UserId, Name, CardId, [CImpl, this, Name, CardId, UserId](TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)
		{
			FTapCommonModule::TapThrobberDismiss();
			if (ModelPtr.IsValid())
			{
				switch (ModelPtr->GetAuthState())
				{
				case FAAURealNameResultModel::Success:
					{
						CImpl->Login(ModelPtr->anti_addiction_token, true);
						STapToast::Show(ETapToastType::Success, LOCTEXT("CommitSuccess", "提交成功"));
						UTapSubsystem::RemoveWidget(AsShared());
					}
					break;
				case FAAURealNameResultModel::Verifying:
					CImpl->ShowRealNameUI(AAURealNameWordTypeVerifying);
					break;
				case FAAURealNameResultModel::Fail:
					CImpl->ShowRealNameUI(AAURealNameWordTypeVerifyFail);
					break;
				}
			}
			else
			{
				STapToast::Show(ETapToastType::Error, FText::FromString(Error.msg));
			}
		});
	}
#endif
	return FReply::Handled();
}

FReply SManualVerifyWidget::OnCloseButtonClicked()
{
	UTapSubsystem::RemoveWidget(AsShared());
	OnClose.ExecuteIfBound();
	return FReply::Handled();
}

FNavigationReply SManualVerifyWidget::OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent)
{
	return FNavigationReply::Stop();
}

void SManualVerifyWidget::OnBackButtonClicked()
{
	OnCloseButtonClicked();
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
