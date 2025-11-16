#include "AAUMobileTipWidget.h"

#include "TUSettings.h"
#include "Components/NativeWidgetHost.h"
#include "Controller/TapControllerManager.h"
#include "Controller/TapControllerTip.h"
#include "UMG/Components/TapButton.h"




UAAUMobileTipWidget* UAAUMobileTipWidget::ShowUI() {
#if PLATFORM_WINDOWS || PLATFORM_MAC
	if (UClass* MyWidgetClass = LoadClass<UAAUMobileTipWidget>(nullptr, TEXT("WidgetBlueprint'/AntiAddiction/AAUTipViewBPPC.AAUTipViewBPPC_C'")))
#else
	if (UClass* MyWidgetClass = LoadClass<UAAUMobileTipWidget>(nullptr, TEXT("WidgetBlueprint'/AntiAddiction/AAUTipViewBP.AAUTipViewBP_C'")))
#endif
	{
		if (TUSettings::GetGameInstance().IsValid()) {
			auto widget = CreateWidget<UAAUMobileTipWidget>(TUSettings::GetGameInstance().Get(), MyWidgetClass);
			widget->AddToViewport(TUSettings::GetUILevel());
			return widget;
		}
	}
	return nullptr;
}

void UAAUMobileTipWidget::SetContent(const FString& Title, const FString& Content, const FString& ComfirmStr) {
	if (Title.IsEmpty()) {
		TitleLabel->SetVisibility(ESlateVisibility::Collapsed);
	} else {
		TitleLabel->SetVisibility(ESlateVisibility::Visible);
		TitleLabel->SetText(FText::FromString(Title));
	}
	if (Content.IsEmpty()) {
		ContentBox->SetVisibility(ESlateVisibility::Collapsed);
	} else {
		ContentBox->SetVisibility(ESlateVisibility::Visible);
		DetailLabel->SetText(FText::FromString(Content));
	}
	SubButtonLabel->SetText(FText::FromString(ComfirmStr));
}

void UAAUMobileTipWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ControllerTip = SNew(STapControllerTip);
	ControllerTipHost->SetContent(ControllerTip.ToSharedRef());
}

void UAAUMobileTipWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SubmitBtn->OnClicked.AddUniqueDynamic(this, &UAAUMobileTipWidget::OnSubmitBtnClick);

	TSharedRef<FTapControllerWidgetData> NewData = MakeShared<FTapControllerWidgetData>(this);
	NewData->StartupFocusWidget = SubmitBtn->GetCachedWidget();
	NewData->ControllerTip = ControllerTip;
	NewData->ControllerTip->SetForceLanguageType(ELanguageType::VI);
	FTapControllerManager::Get().RegisterWidget(NewData);
}

void UAAUMobileTipWidget::NativeDestruct()
{
	Super::NativeDestruct();
	FTapControllerManager::Get().UnregisterWidget(this);
}

FNavigationReply UAAUMobileTipWidget::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
	return FNavigationReply::Stop();
}


void UAAUMobileTipWidget::OnSubmitBtnClick() {
	if (ComformBlock) {
		ComformBlock();
	}
	RemoveFromParent();
}






