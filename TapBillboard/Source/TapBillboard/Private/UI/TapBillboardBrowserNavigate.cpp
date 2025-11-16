// Fill out your copyright notice in the Description page of Project Settings.


#include "TapBillboardBrowserNavigate.h"

#include "SWebBrowserView.h"
#include "TapBillboardModule.h"
#include "TapBillboardTypes.h"
#include "TapPlatformButton.h"
#include "TUHelper.h"
#include "TUType.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/NativeWidgetHost.h"
#include "Components/ScaleBox.h"
#include "IWebBrowserWindow.h"
#include "TapBillboardBrowserPopup.h"
#include "TUSettings.h"
#include "Components/SizeBox.h"
#include "TapBillboardModels.h"
#include "URLParser.h"
#include "Engine/Engine.h"
#include "Misc/Base64.h"


void UTapBillboardBrowserNavigate::LoadUrl(FString Url)
{
	if (BrowserView)
	{
		BrowserView->LoadURL(Url);
		WebTipUI->ShowWait();
	}
}

void UTapBillboardBrowserNavigate::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (auto Billboard = StaticCastSharedPtr<FTapBillboardPC>(FTapBillboardModule::GetTapBillboardInterface()))
	{
		const FString Url = Billboard->GenerateBillboardUrl(FTUConfig::Get()->BillboardConfig->Dimensions);
		
		SAssignNew(BrowserView, SWebBrowserView)
			.InitialURL(Url)
			.BackgroundColor(FColor(0,0,0,0))
			.SupportsTransparency(true)
			.ShowErrorMessage(false)
			.BrowserFrameRate(60.f)
			.OnBeforePopup_UObject(this, &UTapBillboardBrowserNavigate::OnBeforePopup)
			.OnBeforeNavigation_UObject(this, &UTapBillboardBrowserNavigate::OnBeforeNavigate)
			.OnLoadCompleted_UObject(this, &UTapBillboardBrowserNavigate::OnLoadCompleted)
			.OnLoadError_UObject(this, &UTapBillboardBrowserNavigate::OnLoadError);

		NativeWidget->SetContent(BrowserView.ToSharedRef());

		if (ITextInputMethodSystem* InputSys = FSlateApplication::Get().GetTextInputMethodSystem())
		{
			BrowserView->BindInputMethodSystem(InputSys);
		}

		CloseButton->OnClicked.BindUObject(this, &UTapBillboardBrowserNavigate::Close);
		CloseButton->UpdateButtonTexture(Billboard->GetCurrentAllDetailData().navigate.close_btn_img.url);

		WebTipUI->OnRefreshClick.BindUObject(this, &UTapBillboardBrowserNavigate::LoadUrl, Url);
	}
	SizeBox->SetWidthOverride(1504.f);
	SizeBox->SetHeightOverride(864.f);

}

void UTapBillboardBrowserNavigate::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTapBillboardBrowserNavigate::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (TSharedPtr<SWidget> Widget = GetCachedWidget())
	{
		if (TSharedPtr<SWindow> WidgetWindow = FSlateApplicationBase::Get().FindWidgetWindow(Widget.ToSharedRef()))
		{
			const float NativeScale = WidgetWindow->GetNativeWindow()->GetDPIScaleFactor();
			check(NativeScale > 0.f);
			const FVector2D NativeSize = NativeWidget->GetCachedGeometry().GetAbsoluteSize() / NativeScale;
			const float Scale = NativeSize.X / (NativeSize.X > NativeSize.Y ? 584.f : 336.f);
			UpdateCloseButtonDPIScale(Scale * NativeScale);
		}
	}
}

bool UTapBillboardBrowserNavigate::OnBeforePopup(FString Url, FString FrameName)
{
	if (BrowserView->GetAddressBarUrlText().ToString() == Url)
	{
		return false;
	}
	TWeakObjectPtr<UTapBillboardBrowserNavigate> WeakThis(this);
	
	ETapBillboardUrlType Type = GetBillboardUrlType(Url);

	switch (Type)
	{
	case ETapBillboardUrlType::Custom:
		AsyncTask(ENamedThreads::GameThread, [WeakThis, Url]() { if(WeakThis.IsValid()) { WeakThis->OnCustomLinkClicked.ExecuteIfBound(Url); }});
		break;
	case ETapBillboardUrlType::AudioOn:
		AsyncTask(ENamedThreads::GameThread, [WeakThis]() { if(WeakThis.IsValid()) { WeakThis->OnAudioOutputStateChanged.ExecuteIfBound(true); }});
		break;
	case ETapBillboardUrlType::AudioOff:
		AsyncTask(ENamedThreads::GameThread, [WeakThis]() { if(WeakThis.IsValid()) { WeakThis->OnAudioOutputStateChanged.ExecuteIfBound(false); }});
		break;
	case ETapBillboardUrlType::LaunchSystemBrowser:
		AsyncTask(ENamedThreads::GameThread, [Url]() { TUHelper::LaunchURL(*Url, nullptr, nullptr); });
		break;
	case ETapBillboardUrlType::LaunchInnerBrowser: {
		TUCommon::FURL_RFC3986 Parser = TUCommon::FURL_RFC3986();
		if (!Parser.Parse(Url)) {
			return true;
		}
		FString UrlHost = Parser.GetHost();
		if (!Parser.Parse(BrowserView->GetUrl())) {
			return true;
		}
		FString WebUrlHost = Parser.GetHost();
		if (!UrlHost.IsEmpty() && UrlHost == WebUrlHost) {
			return false;
		}
		else {
			AsyncTask(ENamedThreads::GameThread, [Url]() { UTapBillboardBrowserPopup::OpenUrl(Url); });
		}
	}
		break;
	default:
		return false;
	}
	return true;
}

bool UTapBillboardBrowserNavigate::OnBeforeNavigate(const FString& Url, const FWebNavigationRequest& Request)
{
	if (BrowserView->GetAddressBarUrlText().ToString() == Url)
	{
		return false;
	}
	TWeakObjectPtr<UTapBillboardBrowserNavigate> WeakThis(this);
	
	ETapBillboardUrlType Type = GetBillboardUrlType(Url);

	switch (Type)
	{
	case ETapBillboardUrlType::Custom:
		AsyncTask(ENamedThreads::GameThread, [WeakThis, Url]() { if(WeakThis.IsValid()) { WeakThis->OnCustomLinkClicked.ExecuteIfBound(Url); }});
		break;
	case ETapBillboardUrlType::AudioOn:
		AsyncTask(ENamedThreads::GameThread, [WeakThis]() { if(WeakThis.IsValid()) { WeakThis->OnAudioOutputStateChanged.ExecuteIfBound(true); }});
		break;
	case ETapBillboardUrlType::AudioOff:
		AsyncTask(ENamedThreads::GameThread, [WeakThis]() { if(WeakThis.IsValid()) { WeakThis->OnAudioOutputStateChanged.ExecuteIfBound(false); }});
		break;
	case ETapBillboardUrlType::LaunchSystemBrowser:
		AsyncTask(ENamedThreads::GameThread, [Url]() { TUHelper::LaunchURL(*Url, nullptr, nullptr); });
		break;
	case ETapBillboardUrlType::LaunchInnerBrowser: {
		TUCommon::FURL_RFC3986 Parser = TUCommon::FURL_RFC3986();
		if (!Parser.Parse(Url)) {
			return true;
		}
		FString UrlHost = Parser.GetHost();
		if (!Parser.Parse(BrowserView->GetUrl())) {
			return true;
		}
		FString WebUrlHost = Parser.GetHost();
		if (!UrlHost.IsEmpty() && UrlHost == WebUrlHost) {
			return false;
		}
		else {
			AsyncTask(ENamedThreads::GameThread, [Url]() { UTapBillboardBrowserPopup::OpenUrl(Url); });
		}
	}
		break;
	default:
		return false;
	}
	return true;
}

void UTapBillboardBrowserNavigate::OnLoadCompleted()
{
	TWeakObjectPtr<UTapBillboardBrowserNavigate> WeakThis(this);
	AsyncTask(ENamedThreads::GameThread, [WeakThis]()
	{
		if(WeakThis.IsValid())
		{
			WeakThis->WebTipUI->ShowLoadSuccess();
			WeakThis->OnLoadComplete.ExecuteIfBound();
			WeakThis->OnLoadComplete.Unbind();
			WeakThis->OnLoadFailed.Unbind();
		}
	});
}

void UTapBillboardBrowserNavigate::OnLoadError()
{
	TWeakObjectPtr<UTapBillboardBrowserNavigate> WeakThis(this);
	AsyncTask(ENamedThreads::GameThread, [WeakThis]()
	{
		if(WeakThis.IsValid())
		{
			WeakThis->WebTipUI->ShowLoadFail();
			WeakThis->OnLoadFailed.ExecuteIfBound(FTUError(-1, TEXT("Net error.")));
			WeakThis->OnLoadComplete.Unbind();
			WeakThis->OnLoadFailed.Unbind();
		}
	});
}

void UTapBillboardBrowserNavigate::Close()
{
	SetVisibility(ESlateVisibility::Hidden);//接收小红点 暂时不关闭
	
	BrowserView->BindUObject(TEXT("browser"), this, false);
	const TCHAR* JSScript = TEXT("if (window.localStorage==null){window.ue.browser.jsnotifyreddot('submit')}else{window.ue.browser.jsnotifyreddot(window.localStorage.getItem('local_red_dot'))}");
	BrowserView->ExecuteJavascript(JSScript);
	if (auto Billboard = StaticCastSharedPtr<FTapBillboardPC>(FTapBillboardModule::GetTapBillboardInterface()))
	{
		TMap<FString, FString> TraceParams;
		TraceParams.Add(TEXT("action"), TEXT("click"));
		TraceParams.Add(TEXT("type"), TEXT("close"));
		Billboard->Rest_SendTraceEvent(ETapBillboardTemplate::Navigate, TraceParams);
		Billboard->NavigateBrowser = nullptr;//@TODO
	}
	OnBillboardBrowserClosed.ExecuteIfBound();
}

void UTapBillboardBrowserNavigate::JSNotifyRedDot(const FString& ResultString)
{
	FString JsonString;
	if (ResultString == TEXT("submit"))
	{
	if (auto Billboard = StaticCastSharedPtr<FTapBillboardPC>(FTapBillboardModule::GetTapBillboardInterface()))
		{
			Billboard->Rest_SubmitReadingRecord();
		}
	}
	else if (!ResultString.IsEmpty() && FBase64::Decode(ResultString, JsonString))
	{
		JsonString = FGenericPlatformHttp::UrlDecode(JsonString);
		TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			bool bResult = true;
			if (JsonObject->TryGetBoolField(FTUConfig::Get()->ClientID, bResult) && !bResult)
			{
	if (auto Billboard = StaticCastSharedPtr<FTapBillboardPC>(FTapBillboardModule::GetTapBillboardInterface()))
				{
					Billboard->Rest_SubmitReadingRecord();
				}
			}
		}
	}

	RemoveFromParent();
}

void UTapBillboardBrowserNavigate::UpdateCloseButtonDPIScale(float NewScale)
{
	if (DPIBox && GEngine && GEngine->GameViewport)
	{
		const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(GEngine->GameViewport);
		DPIBox->SetUserSpecifiedScale(NewScale / DPIScale);
	}
}
