// Copyright Epic Games, Inc. All Rights Reserved.

#include "TapCommon.h"

#if PLATFORM_MAC
#include "WebBrowserModule.h"
#endif
#include "Slate/Widgets/STapThrobber.h"
#include "Http.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "TapSubsystem.h"
#include "TUType.h"
#include "Desktop/TapClientBridge.hpp"

#include "Styling/SlateStyleRegistry.h"
#include "TapUECommon.h"


#define TAP_STYLE_NAME_COMMON "Tap.Common"

void FTapCommonModule::StartupModule()
{
#if PLATFORM_MAC
	IWebBrowserModule::Get().GetSingleton();
#endif
	Style = FSlateGameResources::New(TAP_STYLE_NAME_COMMON, TEXT("/TapCommon/Styles"), TEXT("/TapCommon/Styles"));
	FSlateStyleRegistry::RegisterSlateStyle(*Style);

#if PLATFORM_ANDROID || PLATFORM_IOS
	TapUECommon::SetXUA();
#endif

#if PLATFORM_WINDOWS
	if (!FModuleManager::Get().IsModuleLoaded(TEXT("Messaging")))
	{
		FModuleManager::Get().LoadModule(TEXT("Messaging"));
	}
	TapClientBridge::LoadSDK();
#endif
	
}

void FTapCommonModule::ShutdownModule()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(TAP_STYLE_NAME_COMMON);
	
}


void HandleImageRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FAsyncDownloadImage Callback)
{
	if ( bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0 )
	{
		UTexture2D* Tex = FImageUtils::ImportBufferAsTexture2D(HttpResponse->GetContent());
		Callback.ExecuteIfBound(Tex);		
	}
	else
	{
		Callback.ExecuteIfBound(nullptr);
		UE_LOG(LogTap, Error, TEXT("Net error, Http success:%s."), bSucceeded ? TEXT("True") : TEXT("False"));
	}
}

void HandleImageBrushRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FAsyncDownloadBrush Callback)
{
	check(HttpRequest);
	if (!HttpResponse.IsValid())
	{
		UE_LOG(LogTap, Error, TEXT("Image Download: Connection Failed. url=%s"), *HttpRequest->GetURL());
		Callback.ExecuteIfBound(nullptr);
		return;
	}

	FString ETag = HttpResponse->GetHeader(TEXT("ETag"));

	// check status code
	int32 StatusCode = HttpResponse->GetResponseCode();
	if (StatusCode / 100 != 2)
	{
		UE_LOG(LogTap, Error, TEXT("Image Download: HTTP response %d. url=%s"), StatusCode, *HttpRequest->GetURL());
		Callback.ExecuteIfBound(nullptr);
		return;
	}

	// build an image wrapper for this type
	static const FName MODULE_IMAGE_WRAPPER(TEXT("ImageWrapper"));
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(MODULE_IMAGE_WRAPPER);

	// Look at the signature of the downloaded image to detect image type. (and ignore the content type header except for error reporting)
	const TArray<uint8>& Content = HttpResponse->GetContent();
	EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(Content.GetData(), Content.Num());

	if (ImageFormat == EImageFormat::Invalid)
	{
		FString ContentType = HttpResponse->GetContentType();
		UE_LOG(LogTap, Error, TEXT("Image Download: Could not recognize file type of image downloaded from url %s, server-reported content type: %s"), *HttpRequest->GetURL(), *ContentType);
		Callback.ExecuteIfBound(nullptr);
		return;
	}

	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
	if (!ImageWrapper.IsValid())
	{
		UE_LOG(LogTap, Error, TEXT("Image Download: Unable to make image wrapper for image format %d"), (int32)ImageFormat);
		Callback.ExecuteIfBound(nullptr);
		return;
	}

	// parse the content
	if (!ImageWrapper->SetCompressed(Content.GetData(), Content.Num()))
	{
		UE_LOG(LogTap, Error, TEXT("Image Download: Unable to parse image format %d from %s"), (int32)ImageFormat, *HttpRequest->GetURL());
		Callback.ExecuteIfBound(nullptr);
		return;
	}

	// get the raw image data
	TArray<uint8> RawImageData;
	if (!ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, RawImageData))
	{
		UE_LOG(LogTap, Error, TEXT("Image Download: Unable to convert image format %d to BGRA 8"), (int32)ImageFormat);
		Callback.ExecuteIfBound(nullptr);
		return;
	}

	// make a dynamic image
	FName ResourceName(*HttpRequest->GetURL());
	TSharedPtr<FSlateDynamicImageBrush> DownloadedBrush = FSlateDynamicImageBrush::CreateWithImageData(ResourceName, FVector2D(ImageWrapper->GetWidth(), ImageWrapper->GetHeight()), RawImageData);
	Callback.ExecuteIfBound(DownloadedBrush);
}

void FTapCommonModule::AsyncDownloadImage(const FString& Url, const FAsyncDownloadImage& Callback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindStatic(&HandleImageRequest, Callback);
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("GET"));
	if (!HttpRequest->ProcessRequest())
	{
		Callback.ExecuteIfBound(nullptr);
	}
}

void FTapCommonModule::AsyncDownloadImage(const FString& Url, const FAsyncDownloadBrush& Callback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindStatic(&HandleImageBrushRequest, Callback);
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("GET"));
	if (!HttpRequest->ProcessRequest())
	{
		Callback.ExecuteIfBound(nullptr);
	}
}

void FTapCommonModule::TapThrobberShowWait()
{
	FTapCommonModule& Module = FModuleManager::GetModuleChecked<FTapCommonModule>(TEXT("TapCommon"));
	if (!Module.TapThrobber)
	{
		SAssignNew(Module.TapThrobber, STapThrobber).bBlock(true);
		if (Module.TapThrobber && GEngine && GEngine->GameViewport)
		{
			UTapSubsystem::AddWidget(Module.TapThrobber.ToSharedRef(), MAX_int16);
		}
	}
}

void FTapCommonModule::TapThrobberShowWaitAndToast(const FString& Toast)
{
	FTapCommonModule& Module = FModuleManager::GetModuleChecked<FTapCommonModule>(TEXT("TapCommon"));
	const FText NewContent = FText::FromString(Toast);
	if (Module.TapThrobber)
	{
		Module.TapThrobber->UpdateContent(NewContent);
	}
	else
	{
		SAssignNew(Module.TapThrobber, STapThrobber)
		.bBlock(true)
		.Content(NewContent);
		if (Module.TapThrobber && GEngine && GEngine->GameViewport)
		{
			UTapSubsystem::AddWidget(Module.TapThrobber.ToSharedRef(), MAX_int16);
		}
	}
}

void FTapCommonModule::TapThrobberDismiss()
{
	FTapCommonModule& Module = FModuleManager::GetModuleChecked<FTapCommonModule>(TEXT("TapCommon"));
	if (Module.TapThrobber && GEngine && GEngine->GameViewport)
	{
		UTapSubsystem::RemoveWidget(Module.TapThrobber.ToSharedRef());
		Module.TapThrobber.Reset();
	}
}

void FTapCommonModule::TapThrobberShowToast(const FString& Toast, float TimeInterval)
{
	FTapCommonModule& Module = FModuleManager::GetModuleChecked<FTapCommonModule>(TEXT("TapCommon"));
	const FText NewContent = FText::FromString(Toast);
	if (Module.TapThrobber)
	{
		Module.TapThrobber->ShowThrobber(false);
		Module.TapThrobber->UpdateContent(NewContent);
		Module.TapThrobber->UpdateTimeInterval(TimeInterval);
		Module.TapThrobber->UpdateRemoveSelfDelegate(FOnTapThrobberRemoveSelf::CreateStatic(&FTapCommonModule::OnTapThrobberRemoveSelf));
	}
	else
	{
		SAssignNew(Module.TapThrobber, STapThrobber)
		.HasThrobber(false)
		.Content(NewContent)
		.TimeInterval(TimeInterval)
		.OnRemoveSelf(FOnTapThrobberRemoveSelf::CreateStatic(&FTapCommonModule::OnTapThrobberRemoveSelf));
		if (Module.TapThrobber && GEngine && GEngine->GameViewport)
		{
			UTapSubsystem::AddWidget(Module.TapThrobber.ToSharedRef(), MAX_int16);
		}
	}
}

void FTapCommonModule::OnTapThrobberRemoveSelf(const TSharedRef<STapThrobber>& Throbber)
{
	FTapCommonModule& Module = FModuleManager::GetModuleChecked<FTapCommonModule>(TEXT("TapCommon"));
	Module.TapThrobber.Reset();
}

	
IMPLEMENT_MODULE(FTapCommonModule, TapCommon)

FTapCommonModule::FBridgeCallback FTapCommonModule::OnBridgeCallback;
FTapCommonModule::FGetRegionCode FTapCommonModule::OnRegionCode;
FTapCommonModule::FIsTapTapInstalled FTapCommonModule::OnTapTapInstalled;
FTapCommonModule::FIsTapGlobalInstalled FTapCommonModule::OnTapGlobalInstalled;
FTapCommonModule::FUpdateGameInTapTap FTapCommonModule::OnUpdateGameInTapTap;
FTapCommonModule::FUpdateGameInTapGlobal FTapCommonModule::OnUpdateGameInTapGlobal;
FTapCommonModule::FOpenReviewInTapGlobal FTapCommonModule::OnOpenReviewInTapGlobal;
FTapCommonModule::FOpenReviewInTapTap FTapCommonModule::OnOpenReviewInTapTap;