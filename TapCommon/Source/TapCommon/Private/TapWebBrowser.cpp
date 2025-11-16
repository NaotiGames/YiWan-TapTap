// Fill out your copyright notice in the Description page of Project Settings.


#include "TapWebBrowser.h"

#include "SWebBrowser.h"
#include "TapCommon.h"
#include "TUHelper.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/NativeWidgetHost.h"
#include "UMG/Components/TapButton.h"

void UTapWebBrowser::LoadURL(const FString& InURL)
{
	if (const TSharedPtr<SWebBrowser> InnerBrowser = GetInnerWebBrowser())
	{
		InnerBrowser->LoadURL(InURL);
	}
}

void UTapWebBrowser::UpdateRetryPanelVisibility(ESlateVisibility NewVisibility)
{
	if (RetryPanel)
	{
		RetryPanel->SetVisibility(NewVisibility);
	}
}

TSharedPtr<SWebBrowser> UTapWebBrowser::GetInnerWebBrowser() const
{
	return StaticCastSharedPtr<SWebBrowser>(WebBrowser->GetContent());
}

void UTapWebBrowser::ExecuteJavascript(const FString& ScriptText) {
	GetInnerWebBrowser()->ExecuteJavascript(ScriptText);
}

bool UTapWebBrowser::CanGoBack() const
{
	if (const TSharedPtr<SWebBrowser> InnerBrowser = GetInnerWebBrowser())
	{
		return InnerBrowser->CanGoBack();
	}
	return false;
}

bool UTapWebBrowser::IsLoaded() const
{
	if (const TSharedPtr<SWebBrowser> InnerBrowser = GetInnerWebBrowser())
	{
		return InnerBrowser->IsLoaded();
	}
	return false;
}

void UTapWebBrowser::GoBack()
{
	if (const TSharedPtr<SWebBrowser> InnerBrowser = GetInnerWebBrowser())
	{
		InnerBrowser->GoBack();
	}
}

void UTapWebBrowser::Close()
{
	if (OnCloseWebBrowserClicked.IsBound())
	{
		OnCloseWebBrowserClicked.Execute();
	}
	RemoveFromParent();
}

void UTapWebBrowser::Reload()
{
	if (const TSharedPtr<SWebBrowser> InnerBrowser = GetInnerWebBrowser())
	{
		InnerBrowser->Reload();
	}
}

void UTapWebBrowser::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	const TSharedRef<SWebBrowser> Browser = SNew(SWebBrowser)
		.InitialURL(TEXT(""))
		.ShowInitialThrobber(false)
		.BackgroundColor(FColor(0,0,0,0))
		.SupportsTransparency(true)
		.ShowControls(false)
		.ShowErrorMessage(false)
		.BrowserFrameRate(60.f)
		.OnUrlChanged(FOnTextChanged::CreateUObject(this, &UTapWebBrowser::HandleOnURLChanged))
		.OnTitleChanged(FOnTextChanged::CreateUObject(this, &UTapWebBrowser::HandleOnTitleChanged))
		.OnLoadStarted(FSimpleDelegate::CreateUObject(this, &UTapWebBrowser::HandleOnLoadStarted))
		.OnLoadCompleted(FSimpleDelegate::CreateUObject(this, &UTapWebBrowser::HandleOnLoadCompleted))
		.OnLoadError(FSimpleDelegate::CreateUObject(this, &UTapWebBrowser::HandleOnLoadError))
		.OnBeforeNavigation(SWebBrowser::FOnBeforeBrowse::CreateUObject(this, &UTapWebBrowser::OnBeforeNavigation))
		.OnBeforePopup(FOnBeforePopupDelegate::CreateUObject(this, &UTapWebBrowser::OnBeforePopup));
		
	WebBrowser->SetContent(Browser);

	if (ITextInputMethodSystem* InputSys = FSlateApplication::Get().GetTextInputMethodSystem())
	{
		Browser->BindInputMethodSystem(InputSys);
	}

	if (BTN_GoBack)
	{
		BTN_GoBack->OnClicked.AddDynamic(this, &UTapWebBrowser::GoBack);
	}

	if (BTN_Close)
	{
		BTN_Close->OnClicked.AddDynamic(this, &UTapWebBrowser::Close);
	}

	if (BTN_Retry)
	{
		BTN_Retry->OnClicked.AddDynamic(this, &UTapWebBrowser::Reload);
	}
}

void UTapWebBrowser::NativeDestruct()
{
	Super::NativeDestruct();
	if (TSharedPtr<ICursor> PlatformCursor = FSlateApplication::Get().GetPlatformCursor())
	{
		PlatformCursor->SetTypeShape(EMouseCursor::Custom, nullptr);
	}
}

void UTapWebBrowser::OnURLChanged(const FText& NewURL)
{

}

void UTapWebBrowser::OnTitleChanged(const FText& NewTitle)
{
}

void UTapWebBrowser::OnLoadStarted()
{
	FTapCommonModule::TapThrobberShowWait();
}

void UTapWebBrowser::OnLoadCompleted()
{
	FTapCommonModule::TapThrobberDismiss();
}

void UTapWebBrowser::OnLoadError()
{
	FTapCommonModule::TapThrobberDismiss();
}

bool UTapWebBrowser::OnBeforeNavigation(const FString& URL, const FWebNavigationRequest& Request)
{
	return false;
}

bool UTapWebBrowser::OnBeforePopup(FString URL, FString FrameName)
{
	return false;
}

FNavigationReply UTapWebBrowser::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
	switch (InNavigationEvent.GetNavigationType())
	{
	case EUINavigation::Down:
		if (RetryPanel->GetVisibility() == ESlateVisibility::Hidden || RetryPanel->GetVisibility() == ESlateVisibility::Collapsed)
		{
			return FNavigationReply::Explicit(GetInnerWebBrowser());
		}
		else
		{
			if (BTN_Retry)
			{
				return FNavigationReply::Explicit(BTN_Retry->GetCachedWidget());
			}
		}
	case EUINavigation::Up:
		if (BTN_Close)
		{
			return FNavigationReply::Explicit(BTN_Close->GetCachedWidget());
		}
	default:
		break;
	}
	return FNavigationReply::Stop();
}

void UTapWebBrowser::HandleOnURLChanged(const FText& NewURL) {
	TWeakObjectPtr<UTapWebBrowser> WeakThis = this;
	TUHelper::PerformOnGameThread([=]() {
		if (!WeakThis.IsValid()) {
			return;
		}
		const TCHAR* JSCode = TEXT(
			"window.oncontextmenu = function(event){	event.preventDefault();	event.stopPropagation();	return false; };");
		GetInnerWebBrowser()->ExecuteJavascript(JSCode);
		WeakThis->OnURLChanged(NewURL);
	});
}

void UTapWebBrowser::HandleOnTitleChanged(const FText& NewTitle) {
	TWeakObjectPtr<UTapWebBrowser> WeakThis = this;
	TUHelper::PerformOnGameThread([=]() {
		if (WeakThis.IsValid()) {
			WeakThis->OnTitleChanged(NewTitle);
		}
	});
}

void UTapWebBrowser::HandleOnLoadStarted() {
	TWeakObjectPtr<UTapWebBrowser> WeakThis = this;
	TUHelper::PerformOnGameThread([=]() {
		if (WeakThis.IsValid()) {
			WeakThis->OnLoadStarted();
		}
	});
}

void UTapWebBrowser::HandleOnLoadCompleted() {
	TWeakObjectPtr<UTapWebBrowser> WeakThis = this;
	TUHelper::PerformOnGameThread([=]() {
		if (WeakThis.IsValid()) {
			WeakThis->OnLoadCompleted();
		}
	});
}

void UTapWebBrowser::HandleOnLoadError() {
	TWeakObjectPtr<UTapWebBrowser> WeakThis = this;
	TUHelper::PerformOnGameThread([=]() {
		if (WeakThis.IsValid()) {
			WeakThis->OnLoadError();
		}
	});
}
