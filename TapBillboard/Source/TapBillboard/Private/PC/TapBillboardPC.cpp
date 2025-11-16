#include "TapBillboardPC.h"

#include "UI/TapBillboardBrowserSplash.h"
#include "UI/TapBillboardMarqueeUI.h"
#include "UI/TapBillboardBrowserNavigate.h"
#include "UI/TapBillboardBrowserPopup.h"
#include "TapBillboardModels.h"

#include "JsonObjectConverter.h"
#include "JsonWrapper.h"

#include "TUDeviceInfo.h"
#include "TULanguage.h"
#include "TapCommon.h"
#include "TapUrlResourceLoader.h"
#include "TUDebuger.h"
#include "TUError.h"
#include "TUHelper.h"
#include "TUSettings.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"
#include "TapBillboardStorage.h"


template<typename CharType, typename PrintPolicy, typename FValue>
void WriteValue(const TSharedRef<TJsonWriter<CharType, PrintPolicy>>& Writer, const TMap<FString, FValue>& ValueMap)
{
	Writer->WriteArrayStart();
	for (const TTuple<FString, FValue>& T : ValueMap)
	{
		Writer->WriteObjectStart();
		Writer->WriteValue(T.Key, T.Value);
		Writer->WriteObjectEnd();
	}
	Writer->WriteArrayEnd();
}

template<typename CharType, typename PrintPolicy, typename FValue>
void WriteValue(const TSharedRef<TJsonWriter<CharType, PrintPolicy>>& Writer, const FString& Identifier, const TMap<FString, FValue>& ValueMap)
{
	Writer->WriteArrayStart(Identifier);
	for (const TTuple<FString, FValue>& T : ValueMap)
	{
		Writer->WriteObjectStart();
		Writer->WriteValue(T.Key, T.Value);
		Writer->WriteObjectEnd();
	}
	Writer->WriteArrayEnd();
}

#define BASE_SERVER_URL_CN "https://%s.billboard.tds1.tapapis.cn"
#define BASE_SERVER_URL_IO "https://%s.billboard.ap-sg.tapapis.com"

#define BASE_WEB_URL_IO "https://%s.billboard.ap-sg.tapapis.com"

FTapBillboardPC::FTapBillboardPC()
	: NavigateBrowser(nullptr)
	, SplashBrowser(nullptr)
	, MarqueeWidget(nullptr)
	, NavigateBrowserClass(FString(TEXT("WidgetBlueprint'/TapBillboard/BillboardWebBrowserNavigate.BillboardWebBrowserNavigate_C'")))
	, SplashBrowserClass(FString(TEXT("WidgetBlueprint'/TapBillboard/BillboardWebBrowserSplash.BillboardWebBrowserSplash_C'")))
	, MarqueeClass(FString(TEXT("WidgetBlueprint'/TapBillboard/BillboardMarquee.BillboardMarquee_C'")))
	, PopupBrowserClass(FString(TEXT("WidgetBlueprint'/TapBillboard/BillboardWebBrowserPopup.BillboardWebBrowserPopup_C'")))
	, CanGetBadgeSecond(0.f)
	, bSplashDataCallback(false)
	, bCachedBadge(false)
{
}

void FTapBillboardPC::Init(const FTUConfig& InConfig)
{
	if (!InConfig.BillboardConfig)
	{
		ensure(false);
		return;
	}
	if (!InConfig.ClientID.IsEmpty())
	{
		FTUConfig::Get()->ClientID = InConfig.ClientID;
	}
	if (!InConfig.ClientToken.IsEmpty())
	{
		FTUConfig::Get()->ClientToken = InConfig.ClientToken;
	}
	FTUConfig::Get()->RegionType = InConfig.RegionType;
	FTUConfig::Get()->BillboardConfig = InConfig.BillboardConfig;

	if (InConfig.RegionType == ERegionType::Global)
	{
		FTUConfig::Get()->BillboardConfig->BillboardUrl = FString::Printf(TEXT(BASE_WEB_URL_IO), *FTUConfig::Get()->ClientID.Left(8));
	}

	ServerUrl = FTUConfig::Get()->RegionType == ERegionType::CN ? FString::Printf(TEXT(BASE_SERVER_URL_CN), *FTUConfig::Get()->ClientID.Left(8)) : FString::Printf(TEXT(BASE_SERVER_URL_IO), *FTUConfig::Get()->ClientID.Left(8));
	ServerUrl = TUDebuger::GetReplacedHost(ServerUrl);

	PrepareData();

}

void FTapBillboardPC::OpenPanel(const FSimpleDelegate& OnSuccess, const FTapFailed& OnFailed, const FSimpleDelegate& OnClose)
{
	if (NavigateBrowser.IsValid() && NavigateBrowser->IsInViewport())
	{
		OnFailed.ExecuteIfBound(FTUError(-1, TEXT("Already exists one.")));
		return;
	}
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnFailed.ExecuteIfBound(*Error);
		return;
	}
	if (!TUSettings::GetGameInstance().IsValid())
	{
		OnFailed.ExecuteIfBound(FTUError(-1, TEXT("Failed display billboard widget. no game instance.")));
		return;
	}

	NavigateBrowser = CreateWidget<UTapBillboardBrowserNavigate>(TUSettings::GetGameInstance().Get(),
	                                                             NavigateBrowserClass.Get());
	NavigateBrowser->OnAudioOutputStateChanged.BindRaw(this, &FTapBillboardPC::InternalAudioOutputStateChangedCallback,
	                                                   ETapBillboardTemplate::Navigate);
	NavigateBrowser->OnCustomLinkClicked.BindRaw(this, &FTapBillboardPC::InternalCustomUrlClickedCallback,
	                                             ETapBillboardTemplate::Navigate);
	NavigateBrowser->OnLoadComplete = OnSuccess;
	NavigateBrowser->OnLoadFailed = OnFailed;
	NavigateBrowser->OnBillboardBrowserClosed = OnClose;

	const FString Url = GenerateBillboardUrl(FTUConfig::Get()->BillboardConfig->Dimensions);
	NavigateBrowser->LoadUrl(Url);
	NavigateBrowser->AddToViewport(TUSettings::GetUILevel());
}

void FTapBillboardPC::ClosePanel() {
	if (NavigateBrowser.IsValid() && NavigateBrowser->IsInViewport())
	{
		NavigateBrowser->Close();
	}
}

void FTapBillboardPC::GetBadgeDetails(const FTapBadgeDetailsResult& OnSuccess, const FTapFailed& OnFailed)
{
	if (const TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnFailed.ExecuteIfBound(*Error);
		return;
	}

	if (bCachedBadge)
	{
		if (const UGameInstance* GI = TUSettings::GetGameInstance().Get())
		{
			if (const UWorld* World = GI->GetWorld())
			{
				if (World->GetTimeSeconds() < CanGetBadgeSecond)
				{
					OnSuccess.ExecuteIfBound(CachedBadgeDetails);
					return;
				}
			}
		}
	}
	
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
	const FString Url = FString::Printf(TEXT("%s/billboard/rest-api/v1/dot/read?client_id=%s"),
		*ServerUrl,
		*FTUConfig::Get()->ClientID);
	HttpRequest->SetURL(Url);

	HttpRequest->SetVerb(TEXT("POST"));
	
	HttpRequest->SetHeader(TEXT("X-LC-Id"), FTUConfig::Get()->ClientID);
	HttpRequest->SetHeader(TEXT("X-LC-Sign"), TUHelper::GetLCSign(FTUConfig::Get()->ClientToken));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FString JsonString;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("uuid"), TUDeviceInfo::GetLoginId());
	Writer->WriteValue(TEXT("template"), ETapBillboardTemplate::Navigate);
	WriteValue(Writer, TEXT("dimension_list"), FTUConfig::Get()->BillboardConfig->Dimensions);
	Writer->WriteObjectEnd();
	Writer->Close();
	HttpRequest->SetContentAsString(JsonString);
	
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FTapBillboardPC::NetGetBadgeDetailsCallback, OnSuccess, OnFailed);

	if (HttpRequest->ProcessRequest())
	{
		CanGetBadgeSecond += 5.f * 60.f;
	}
	else
	{
		OnFailed.ExecuteIfBound(FTUError(-1, TEXT("Net error")));
	}
}

void FTapBillboardPC::OpenSplashPanel(const FSimpleDelegate& OnSuccess, const FTapFailed& OnFailed, const FSimpleDelegate& OnClose)
{
	if (IsValid(SplashBrowser) && SplashBrowser->IsInViewport()) {
		OnFailed.Execute(FTUError(-1, TEXT("Already exists one.")));
		return;
	}
	if (TSharedPtr<FTUError> Error = CheckConfig()) {
		OnFailed.ExecuteIfBound(*Error);
		return;
	}
	if (!TUSettings::GetGameInstance().IsValid()) {
		OnFailed.ExecuteIfBound(FTUError(-1, TEXT("Failed display splash billboard widget. no game instance")));
		return;
	}


	SplashBrowser = CreateWidget<UTapBillboardBrowserSplash>(TUSettings::GetGameInstance().Get(),
	                                                         SplashBrowserClass.Get());
	SplashBrowser->OnAudioOutputStateChanged.BindRaw(this, &FTapBillboardPC::InternalAudioOutputStateChangedCallback,
	                                                 ETapBillboardTemplate::Splash);
	SplashBrowser->OnCustomLinkClicked.BindRaw(this, &FTapBillboardPC::InternalCustomUrlClickedCallback,
	                                           ETapBillboardTemplate::Splash);

	SplashBrowser->OnLoadComplete = OnSuccess;
	SplashBrowser->OnLoadFailed = OnFailed;
	SplashBrowser->OnBillboardBrowserClosed = OnClose;

	SplashBrowser->LoadSplash();

}

void FTapBillboardPC::CloseSplashPanel()
{
	if (IsValid(SplashBrowser))
	{
		TMap<FString, FString> TraceParams;
		TraceParams.Add(TEXT("action"), TEXT("click"));
		TraceParams.Add(TEXT("type"), TEXT("close"));
		Rest_SendTraceEvent(ETapBillboardTemplate::Splash, TraceParams);
		
		FSimpleDelegate Temp = SplashBrowser->OnBillboardBrowserClosed;
		SplashBrowser->RemoveFromParent();
		SplashBrowser = nullptr;
		Temp.ExecuteIfBound();
	}
}

void FTapBillboardPC::StartFetchMarqueeData()
{
	if (FetchMarqueeTimer.IsValid())
	{
		return;
	}
	
	if (!TUSettings::GetGameInstance().IsValid())
	{
		return;
	}
	if (UWorld* World = TUSettings::GetGameInstance()->GetWorld())
	{
		FTimerDelegate Delegate;
		Delegate.BindRaw(this, &FTapBillboardPC::TimerFetchMarqueeData);
		World->GetTimerManager().SetTimer(FetchMarqueeTimer, Delegate, 300.f, true, 0.f);
		TMap<FString, FString> TraceParams;
		TraceParams.Add(TEXT("action"), TEXT("app_init"));
		TraceParams.Add(TEXT("init_from"), TEXT("sdk"));
		Rest_SendTraceEvent(ETapBillboardTemplate::Marquee, TraceParams);
	}
}

void FTapBillboardPC::StopFetchMarqueeData(bool bCloseNow)
{
	if (TUSettings::GetGameInstance().IsValid())
	{
		if (UWorld* World = TUSettings::GetGameInstance()->GetWorld())
		{
			World->GetTimerManager().ClearTimer(FetchMarqueeTimer);
			FetchMarqueeTimer.Invalidate();
		}
	}
	if (bCloseNow && IsValid(MarqueeWidget))
	{
		if (MarqueeWidget->IsInViewport())
		{
			MarqueeWidget->RemoveFromParent();
		}
		MarqueeWidget = nullptr;
	}
}



void FTapBillboardPC::Rest_SubmitReadingRecord()
{
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
	const FString Url = FString::Printf(TEXT("%s/billboard/rest-api/v1/dot/submit?client_id=%s"),
		*ServerUrl,
		*FTUConfig::Get()->ClientID);
	HttpRequest->SetURL(Url);

	HttpRequest->SetVerb(TEXT("POST"));
	
	HttpRequest->SetHeader(TEXT("X-LC-Id"), FTUConfig::Get()->ClientID);
	HttpRequest->SetHeader(TEXT("X-LC-Sign"), TUHelper::GetLCSign(FTUConfig::Get()->ClientToken));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FString JsonString;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("uuid"), TUDeviceInfo::GetLoginId());
	Writer->WriteValue(TEXT("client_id"), FTUConfig::Get()->ClientID);
	Writer->WriteValue(TEXT("read_all"), 1);
	Writer->WriteObjectEnd();
	Writer->Close();
	HttpRequest->SetContentAsString(JsonString);

	if (HttpRequest->ProcessRequest())
	{
		CachedBadgeDetails.show_red_dot = false;
	}
}

TSharedPtr<FTUError> FTapBillboardPC::CheckConfig() const
{
	const TSharedPtr<FTUConfig> Config = FTUConfig::Get();
	check(Config);
	if (Config->ClientID.IsEmpty())
		return MakeShared<FTUError>(-1, TEXT("Need ClientID"));
	
	if (Config->ClientToken.IsEmpty())
		return MakeShared<FTUError>(-1, TEXT("Need ClientToken"));
	
	if (Config->RegionType == ERegionType::CN && (!Config->BillboardConfig.IsValid() || Config->BillboardConfig->BillboardUrl.IsEmpty()))
		return MakeShared<FTUError>(-1, TEXT("ERegionType::CN Need BillboardConfig.BillboardUrl"));
	ensure(NavigateBrowserClass);
	ensure(SplashBrowserClass);
	ensure(MarqueeClass);
	return nullptr;
}

FString FTapBillboardPC::GenerateBillboardUrl(const TMap<FString, FString>& DimensionsMap) const
{
	return FString::Printf(TEXT("%s/webapp/%s?business=%s&track=%s"),
					*TUDebuger::GetReplacedHost(FTUConfig::Get()->BillboardConfig->BillboardUrl),
					*FTUConfig::Get()->ClientID,
					*GenerateBusinessParams(DimensionsMap),
					*GenerateTrackParams());
}

FString FTapBillboardPC::GenerateSplashUrl(const TMap<FString, FString>& DimensionsMap, int64 SplashID)
{
	return FString::Printf(TEXT("%s/webapp/%s?business=%s&track=%s"),
					*TUDebuger::GetReplacedHost(FTUConfig::Get()->BillboardConfig->BillboardUrl),
					*FTUConfig::Get()->ClientID,
					*GenerateBusinessParamsSplash(DimensionsMap, SplashID),
					*GenerateTrackParams());
}

const FAnnouncementStyleData& FTapBillboardPC::GetMarqueeStyle() const
{
	return CachedMarqueeStyle;
}

TSubclassOf<UTapBillboardBrowserPopup> FTapBillboardPC::GetPopupBrowserClass() const
{
	return PopupBrowserClass.LoadSynchronous();
}

const FAnnouncementDetailData* FTapBillboardPC::FindNextMarqueeData(const TArray<int64>& IgnoreIds) const
{
	for (int32 i = PendingDetailsReverse.Num() - 1; i >= 0; --i)
	{
		if (IgnoreIds.Contains(PendingDetailsReverse[i].id))
		{
			continue;
		}

		return &PendingDetailsReverse[i];
	}

	return nullptr;
}

void FTapBillboardPC::PopMarqueeData(int64 MarqueeId)
{
	if (PendingDetailsReverse.Num() > 0)
	{
		if (PendingDetailsReverse.Top().id == MarqueeId)
		{
			PendingDetailsReverse.Pop();
		}
	}
}

void FTapBillboardPC::UpdateMarqueeScrollSpeed(float NewSpeed)
{
	MarqueeSpeed = NewSpeed;
	if (MarqueeWidget)
	{
		MarqueeWidget->UpdateScrollSpeed(NewSpeed);
	}
}

float FTapBillboardPC::GetMarqueeSpeed() const
{
	return MarqueeSpeed;
}

const FBillboardAllDetailData& FTapBillboardPC::GetCurrentAllDetailData() {
	return CurrentAllDetailData;
}

void FTapBillboardPC::Rest_FetchMarqueeStyle(const FAnnouncementStyleDataResult& OnSuccess, const FTapFailed& OnFailed)
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnFailed.ExecuteIfBound(*Error);
		return;
	}
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
	const FString Url = FString::Printf(TEXT("%s/billboard/rest-api/v1/pattern/detail?template=%s&client_id=%s"),
		*ServerUrl,
		ETapBillboardTemplate::Marquee,
		*FTUConfig::Get()->ClientID);
	HttpRequest->SetURL(Url);

	HttpRequest->SetVerb(TEXT("GET"));

	HttpRequest->SetHeader(TEXT("X-LC-Id"), FTUConfig::Get()->ClientID);
	HttpRequest->SetHeader(TEXT("X-LC-Sign"), TUHelper::GetLCSign(FTUConfig::Get()->ClientToken));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FTapBillboardPC::NetFetchMarqueeStyleCallback, OnSuccess, OnFailed);

	if (!HttpRequest->ProcessRequest())
	{
		OnFailed.ExecuteIfBound(FTUError(-1, TEXT("Net error")));
	}
}

void FTapBillboardPC::Rest_FetchAllDetail(const FAllDetailDataResult& OnSuccess, const FTapFailed& OnFailed)
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnFailed.ExecuteIfBound(*Error);
		return;
	}
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
	const FString Url = FString::Printf(TEXT("%s/billboard/rest-api/v1/pattern/detail-all?client_id=%s"),
		*ServerUrl, *FTUConfig::Get()->ClientID);
	HttpRequest->SetURL(Url);

	HttpRequest->SetVerb(TEXT("GET"));

	HttpRequest->SetHeader(TEXT("X-LC-Id"), FTUConfig::Get()->ClientID);
	HttpRequest->SetHeader(TEXT("X-LC-Sign"), TUHelper::GetLCSign(FTUConfig::Get()->ClientToken));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FTapBillboardPC::NetFetchAllDetailCallback, OnSuccess, OnFailed);

	if (!HttpRequest->ProcessRequest())
	{
		OnFailed.ExecuteIfBound(FTUError(-1, TEXT("Net error")));
	}
}

void FTapBillboardPC::Rest_FetchUnreadAnnouncementsGeneralData(const TCHAR* Template, const FAnnouncementGeneralDataResult& OnSuccess, const FTapFailed& OnFailed)
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnFailed.ExecuteIfBound(*Error);
		return;
	}
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
	const FString Url = FString::Printf(TEXT("%s/billboard/rest-api/v1/announcement/unread?client_id=%s"),
		*ServerUrl,
		*FTUConfig::Get()->ClientID);
	HttpRequest->SetURL(Url);

	HttpRequest->SetVerb(TEXT("POST"));

	HttpRequest->SetHeader(TEXT("X-LC-Id"), FTUConfig::Get()->ClientID);
	HttpRequest->SetHeader(TEXT("X-LC-Sign"), TUHelper::GetLCSign(FTUConfig::Get()->ClientToken));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FString JsonString;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	Writer->WriteObjectStart();
	WriteValue(Writer, TEXT("dimension_list"), FTUConfig::Get()->BillboardConfig->Dimensions);
	Writer->WriteValue(TEXT("template"), Template);
	Writer->WriteValue(TEXT("uuid"), TUDeviceInfo::GetLoginId());
	Writer->WriteObjectEnd();
	Writer->Close();
	HttpRequest->SetContentAsString(JsonString);
	
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FTapBillboardPC::NetFetchGeneralDataCallback, OnSuccess, OnFailed);

	if (!HttpRequest->ProcessRequest())
	{
		OnFailed.ExecuteIfBound(FTUError(-1, TEXT("Net error")));
	}
}

void FTapBillboardPC::Rest_FetchAnnouncementsDetailData(const TArray<int64>& Ids, const FAnnouncementDetailsDataResult& OnSuccess, const FTapFailed& OnFailed)
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnFailed.ExecuteIfBound(*Error);
		return;
	}
	
	if (Ids.Num() == 0)
	{
		OnFailed.ExecuteIfBound(FTUError(-1, TEXT("Need Ids")));
		return;
	}

	FString IDStr;
	for (int32 i = 0; i < Ids.Num(); ++i)
	{
		if (i == Ids.Num() - 1)
		{
			IDStr += FString::Printf(TEXT("%lld"), Ids[i]);
		}
		else
		{
			IDStr += FString::Printf(TEXT("%lld,"), Ids[i]);
		}
	}

	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
	const FString Url = FString::Printf(TEXT("%s/billboard/rest-api/v1/announcement/detail/multi?client_id=%s&ids=%s&uuid=%s&lang=%s"),
		*ServerUrl,
		*FTUConfig::Get()->ClientID,
		*IDStr,
		*TUDeviceInfo::GetLoginId(),
		*TULanguage::GetLanguageString());
	HttpRequest->SetURL(Url);

	HttpRequest->SetVerb(TEXT("GET"));

	HttpRequest->SetHeader(TEXT("X-LC-Id"), FTUConfig::Get()->ClientID);
	HttpRequest->SetHeader(TEXT("X-LC-Sign"), TUHelper::GetLCSign(FTUConfig::Get()->ClientToken));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FTapBillboardPC::NetFetchDetailDataCallback, OnSuccess, OnFailed);

	if (!HttpRequest->ProcessRequest())
	{
		OnFailed.ExecuteIfBound(FTUError(-1, TEXT("Net error")));
	}
}

void FTapBillboardPC::Rest_AnnouncementsMarkRead(const TArray<int64>& Ids, const FSimpleDelegate& OnSuccess, const FTapFailed& OnFailed)
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnFailed.ExecuteIfBound(*Error);
		return;
	}
	
	if (Ids.Num() == 0)
	{
		OnFailed.ExecuteIfBound(FTUError(-1, TEXT("Need param id")));
		return;
	}
	
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
	const FString Url = FString::Printf(TEXT("%s/billboard/rest-api/v1/announcement/mark?client_id=%s"),
		*ServerUrl,
		*FTUConfig::Get()->ClientID);
	HttpRequest->SetURL(Url);

	HttpRequest->SetVerb(TEXT("POST"));

	HttpRequest->SetHeader(TEXT("X-LC-Id"), FTUConfig::Get()->ClientID);
	HttpRequest->SetHeader(TEXT("X-LC-Sign"), TUHelper::GetLCSign(FTUConfig::Get()->ClientToken));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	FString JsonString;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("uuid"), *TUDeviceInfo::GetLoginId());
	Writer->WriteValue(TEXT("ids"), Ids);
	Writer->WriteValue(TEXT("template"), ETapBillboardTemplate::Marquee);
	Writer->WriteObjectEnd();
	Writer->Close();
	HttpRequest->SetContentAsString(JsonString);
	
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FTapBillboardPC::NetMarkReadCallback, OnSuccess, OnFailed);

	if (!HttpRequest->ProcessRequest())
	{
		OnFailed.ExecuteIfBound(FTUError(-1, TEXT("Net error")));
	}
}

void FTapBillboardPC::Rest_SendTraceEvent(const TCHAR* Template, const TMap<FString, FString>& TraceParams)
{
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	FString Url = FTUConfig::Get()->RegionType == ERegionType::CN
			? TEXT("https://tap-tds-data-bj.cn-beijing.log.aliyuncs.com")
			: TEXT("https://tap-tds-data-hk.log-global.aliyuncs.com");
	Url = TUDebuger::GetReplacedHost(Url);
	Url += TEXT("/logstores/web_tds_logs/track");
	
	TMap<FString, FString> Params;
	Params.Add(TEXT("APIVersion"), TEXT("0.6.0"));
	Params.Add(TEXT("XUA"), GenerateXUA());
	Params.Add(TEXT("client_id"), FTUConfig::Get()->ClientID);
	Params.Add(TEXT("os"), TUDeviceInfo::GetPlatform());
	Params.Add(TEXT("sv"), TUDeviceInfo::GetOSVersion());
	Params.Add(TEXT("dv"), FPlatformMisc::GetCPUVendor());
	Params.Add(TEXT("md"), FPlatformMisc::GetCPUBrand());
	Params.Add(TEXT("time"), FString::Printf(TEXT("%lld"), FDateTime::Now().ToUnixTimestamp() * 1000));
	Params.Add(TEXT("t_log_id"), FGuid::NewGuid().ToString());
	Params.Add(TEXT("sdk_vc"), TapBillboard_UE_VERSION_NUMBER);
#if PLATFORM_IOS || PLATFORM_ANDROID
	EDeviceScreenOrientation Orientation = FPlatformMisc::GetDeviceOrientation();
	FString OrientationStr = Orientation == EDeviceScreenOrientation::Portrait || Orientation == EDeviceScreenOrientation::PortraitUpsideDown ? TEXT("1") : TEXT("2");
	Params.Add(TEXT("orientation"), OrientationStr);
#else
	Params.Add(TEXT("orientation"), TEXT("1"));
#endif
	Params.Add(TEXT("template"), Template);
	Params.Add(TEXT("area"), FTUConfig::Get()->RegionType == ERegionType::CN ? TEXT("cn") : TEXT("intl"));
	Params.Append(TraceParams);
	
	int32 i = 0;
	FString Result = TEXT("?");
	for (const TTuple<FString, FString>& T : Params)
	{
		Result += FPlatformHttp::UrlEncode(T.Key);
		Result += TEXT("=");
		Result += FPlatformHttp::UrlEncode(T.Value);
		if (i++ < Params.Num() - 1)
		{
			Result += TEXT("&");
		}
	}
	Url += Result;
	HttpRequest->SetURL(Url);

	HttpRequest->SetVerb(TEXT("GET"));
	
	HttpRequest->SetHeader(TEXT("X-LC-Id"), FTUConfig::Get()->ClientID);
	HttpRequest->SetHeader(TEXT("X-LC-Sign"), TUHelper::GetLCSign(FTUConfig::Get()->ClientToken));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	HttpRequest->ProcessRequest();
}

void FTapBillboardPC::Test_UnmarkCurrentDevice()
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		return;
	}
	
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
	const FString Url = FString::Printf(TEXT("%s/billboard/rest-api/v1/announcement/unmark?client_id=%s"),
		*ServerUrl,
		*FTUConfig::Get()->ClientID);
	HttpRequest->SetURL(Url);

	HttpRequest->SetVerb(TEXT("POST"));

	HttpRequest->SetHeader(TEXT("X-LC-Id"), FTUConfig::Get()->ClientID);
	HttpRequest->SetHeader(TEXT("X-LC-Sign"), TUHelper::GetLCSign(FTUConfig::Get()->ClientToken));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	FString JsonString;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("uuid"), *TUDeviceInfo::GetLoginId());
	Writer->WriteObjectEnd();
	Writer->Close();
	HttpRequest->SetContentAsString(JsonString);
	
	HttpRequest->ProcessRequest();
}

void FTapBillboardPC::LoadBrowserClass()
{
	NavigateBrowserClass.LoadSynchronous();
	SplashBrowserClass.LoadSynchronous();
	MarqueeClass.LoadSynchronous();
	PopupBrowserClass.LoadSynchronous();
}

FString FTapBillboardPC::GenerateBusinessParams(const TMap<FString, FString>& DimensionsMap) const
{
	TMap<FString, FString> Params;
	Params.Add(TEXT("lang"), TULanguage::GetLanguageString());
	
	FString JsonString;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	WriteValue(Writer, DimensionsMap);
	Writer->Close();
	Params.Add(TEXT("dimension_list"), JsonString);
	Params.Add(TEXT("template"), ETapBillboardTemplate::Navigate);
	Params.Add(TEXT("query_nameless"), FTUConfig::Get()->ClientToken);
	
	int32 i = 0;
	FString Result;
	for (const TTuple<FString, FString>& T : Params)
	{
		Result += T.Key;
		Result += TEXT("=");
		Result += T.Value;
		if (i++ < Params.Num() - 1)
		{
			Result += TEXT("&");
		}
	}
	return FBase64::Encode(Result);
}

FString FTapBillboardPC::GenerateBusinessParamsSplash(const TMap<FString, FString>& DimensionsMap, int64 SplashId) const
{
	TMap<FString, FString> Params;
	Params.Add(TEXT("lang"), TULanguage::GetLanguageString());
	
	FString JsonString;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	WriteValue(Writer, DimensionsMap);
	Writer->Close();
	Params.Add(TEXT("dimension_list"), JsonString);
	Params.Add(TEXT("template"), ETapBillboardTemplate::Splash);
	Params.Add(TEXT("query_nameless"), FTUConfig::Get()->ClientToken);
	Params.Add(TEXT("splash_id"), FString::Printf(TEXT("%lld"), SplashId));
	
	int32 i = 0;
	FString Result;
	for (const TTuple<FString, FString>& T : Params)
	{
		Result += T.Key;
		Result += TEXT("=");
		Result += T.Value;
		if (i++ < Params.Num() - 1)
		{
			Result += TEXT("&");
		}
	}
	return FBase64::Encode(Result);
}

FString FTapBillboardPC::GenerateTrackParams() const
{
	TMap<FString, FString> Params;
	Params.Add(TEXT("OS"), TUDeviceInfo::GetPlatform());
	Params.Add(TEXT("SV"), TUDeviceInfo::GetOSVersion());
	// Params.Add(TEXT("DEB"),TEXT("Google"));
	// Params.Add(TEXT("DEM"), TEXT("Sdk-Google-platform"));
	Params.Add(TEXT("SDKVC"), TapBillboard_UE_VERSION_NUMBER);
	Params.Add(TEXT("DEVICEID"), TUDeviceInfo::GetDeviceId());
	
	int32 i = 0;
	FString Result;
	for (const TTuple<FString, FString>& T : Params)
	{
		Result += T.Key;
		Result += TEXT("=");
		Result += T.Value;
		if (i++ < Params.Num() - 1)
		{
			Result += TEXT("&");
		}
	}
	return FBase64::Encode(Result);
}

FString FTapBillboardPC::GenerateXUA() const
{
	TMap<FString, FString> Params;
	Params.Add(TEXT("V"), TEXT("1"));
	if (FTUConfig::Get()->RegionType == ERegionType::CN)
	{
		Params.Add(TEXT("PN"), TEXT("tdsBillboard"));
	}
	else
	{
		Params.Add(TEXT("PN"), TEXT("tdsBillboardIntl"));
	}
	Params.Add(TEXT("LANG"), TULanguage::GetLanguageString());
	Params.Add(TEXT("UID"), FGuid::NewGuid().ToString());
	Params.Add(TEXT("PLT"), TUDeviceInfo::GetPlatform());
	Params.Add(TEXT("OSV"), TUDeviceInfo::GetOSVersion());
	Params.Add(TEXT("SDKVC"), TapBillboard_UE_VERSION_NUMBER);
	Params.Add(TEXT("CID"), FTUConfig::Get()->ClientID);
	
	int32 i = 0;
	FString Result;
	for (const TTuple<FString, FString>& T : Params)
	{
		
		Result += T.Key;
		Result += TEXT("=");
		Result += T.Value;
		if (i++ < Params.Num() - 1)
		{
			Result += TEXT("&");
		}
	}
	return Result;
}

void FTapBillboardPC::InternalCustomUrlClickedCallback(const FString& Url, const TCHAR* Template)
{
	OnCustomLinkClicked.Broadcast(Url);
}

void FTapBillboardPC::InternalAudioOutputStateChangedCallback(bool bPlaying, const TCHAR* Template)
{
	OnAudioOutputStateChanged.ExecuteIfBound(bPlaying);
}

void FTapBillboardPC::NetGetBadgeDetailsCallback(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FTapBadgeDetailsResult OnSuccess, FTapFailed OnFailed)
{
	if (TSharedPtr<FTUError> Error = ParseTapHttpResponse(HttpResponse, CachedBadgeDetails))
	{
		OnFailed.ExecuteIfBound(*Error);	
	}
	else
	{
		bCachedBadge = true;
		OnSuccess.ExecuteIfBound(CachedBadgeDetails);
	}
}

void FTapBillboardPC::NetFetchMarqueeStyleCallback(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FAnnouncementStyleDataResult OnSuccess, FTapFailed OnFailed)
{
	FAnnouncementStyleData MarqueeStyle;
	if (const TSharedPtr<FTUError> Error = ParseTapHttpResponse(HttpResponse, MarqueeStyle))
	{
		OnFailed.ExecuteIfBound(*Error);	
	}
	else
	{
		OnSuccess.ExecuteIfBound(MarqueeStyle);
	}
}

void FTapBillboardPC::NetFetchAllDetailCallback(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	bool bSucceeded, FAllDetailDataResult OnSuccess, FTapFailed OnFailed) {
	if (const TSharedPtr<FTUError> Error = ParseTapHttpResponse(HttpResponse, CurrentAllDetailData))
	{
		OnFailed.ExecuteIfBound(*Error);	
	}
	else
	{
		OnSuccess.ExecuteIfBound(CurrentAllDetailData);
	}
}

void FTapBillboardPC::NetFetchGeneralDataCallback(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FAnnouncementGeneralDataResult OnSuccess, FTapFailed OnFailed)
{
	FAnnouncementGeneralDataList Wrapper;
	if (TSharedPtr<FTUError> Error = ParseTapHttpResponse(HttpResponse, Wrapper))
	{
		OnFailed.ExecuteIfBound(*Error);	
	}
	else
	{
		OnSuccess.ExecuteIfBound(Wrapper.list);
	}
}

void FTapBillboardPC::NetFetchDetailDataCallback(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FAnnouncementDetailsDataResult OnSuccess, FTapFailed OnFailed)
{
	FAnnouncementDetailDataList Wrapper;
	if (TSharedPtr<FTUError> Error = ParseTapHttpResponse(HttpResponse, Wrapper))
	{
		OnFailed.ExecuteIfBound(*Error);	
	}
	else
	{
		OnSuccess.ExecuteIfBound(Wrapper.list);
	}
}

void FTapBillboardPC::NetMarkReadCallback(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FSimpleDelegate OnSuccess, FTapFailed OnFailed)
{
	FAnnouncementMarkRead Wrapper;
	if (TSharedPtr<FTUError> Error = ParseTapHttpResponse(HttpResponse, Wrapper))
	{
		OnFailed.ExecuteIfBound(*Error);	
	}
	else if(Wrapper.msg == TEXT("ok"))
	{
		OnSuccess.ExecuteIfBound();
	}
	else
	{
		OnFailed.ExecuteIfBound(FTUError(-1, FString::Printf(TEXT("Net error, error content, response content: %s"), *HttpResponse->GetContentAsString())));
	}
}

void FTapBillboardPC::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(SplashBrowser);
	Collector.AddReferencedObject(MarqueeWidget);
}

void FTapBillboardPC::PrepareData() {
	auto PrepareDataBlock = [=](const FBillboardAllDetailData& AllDetailData) {
		CurrentAllDetailData = AllDetailData;
		CachedMarqueeStyle = AllDetailData.marquee;
		FTapUrlResourceLoader::PreLoadFileData(CachedMarqueeStyle.icon.url, 3, FTapUrlResourceLoaderSuccessDelegate());
		if (!CachedMarqueeStyle.use_system_font)
		{
			FTapUrlResourceLoader::PreLoadFileData(CachedMarqueeStyle.custom_font, 60, FTapUrlResourceLoaderSuccessDelegate());
		}
		FTapUrlResourceLoader::PreLoadFileData(AllDetailData.navigate.close_btn_img.url, 3, FTapUrlResourceLoaderSuccessDelegate());
	};
	auto CacheDetailData = TUDataStorage<FTapBillboardStorage>::LoadStruct<FBillboardAllDetailData>(FTapBillboardStorage::CacheDetailData);
	if (CacheDetailData.IsValid()) {
		PrepareDataBlock(*CacheDetailData.Get());
	}
	Rest_FetchAllDetail(FAllDetailDataResult::CreateLambda([=](const FBillboardAllDetailData& AllDetailData) {
		PrepareDataBlock(AllDetailData);
		TUDataStorage<FTapBillboardStorage>::SaveStruct(FTapBillboardStorage::CacheDetailData, AllDetailData);
	}), FTapFailed::CreateLambda([=](const FTUError& Error) {
		TUDebuger::WarningLog(TEXT("公告拉取配置失败"));
	}));

	// Rest_FetchUnreadAnnouncementsGeneralData(ETapBillboardTemplate::Splash,
	// 	FAnnouncementGeneralDataResult::CreateRaw(this, &FTapBillboardPC::InternalFetchUnreadSplashCallback),
	// 	FTapFailed());
	
}

void FTapBillboardPC::InternalFetchUnreadSplashCallback(const TArray<FAnnouncementGeneralData>& GeneralData)
{
	bSplashDataCallback = true;
	if (GeneralData.Num() > 0)
	{
		if (!SplashBrowser)
		{
			SplashBrowser = CreateWidget<UTapBillboardBrowserSplash>(TUSettings::GetGameInstance().Get(), SplashBrowserClass.Get());
			SplashBrowser->OnAudioOutputStateChanged.BindRaw(this, &FTapBillboardPC::InternalAudioOutputStateChangedCallback, ETapBillboardTemplate::Splash);
			SplashBrowser->OnCustomLinkClicked.BindRaw(this, &FTapBillboardPC::InternalCustomUrlClickedCallback, ETapBillboardTemplate::Splash);
			SplashBrowser->LoadSplashWithID(GeneralData[0].id);
		}

		SplashBrowser->SetExpireTime(GeneralData[0].expire_time);
	}
}

void FTapBillboardPC::TimerFetchMarqueeData()
{
	auto Block = [=]() {
		Rest_FetchUnreadAnnouncementsGeneralData(ETapBillboardTemplate::Marquee,
		FAnnouncementGeneralDataResult::CreateRaw(this, &FTapBillboardPC::HandleFetchUnreadMarqueeGeneralData),
		FTapFailed());
	};

	if (CachedMarqueeStyle.default_text_color.IsEmpty() || CachedMarqueeStyle.content_background_color.IsEmpty()) {
		auto PrepareDataBlock = [=](const FBillboardAllDetailData& AllDetailData) {
			CurrentAllDetailData = AllDetailData;
			CachedMarqueeStyle = AllDetailData.marquee;
		};
		Rest_FetchAllDetail(FAllDetailDataResult::CreateLambda([=](const FBillboardAllDetailData& AllDetailData) {
			CurrentAllDetailData = AllDetailData;
			CachedMarqueeStyle = AllDetailData.marquee;
			Block();
			TUDataStorage<FTapBillboardStorage>::SaveStruct(FTapBillboardStorage::CacheDetailData, AllDetailData);
		}), FTapFailed::CreateLambda([=](const FTUError& Error) {
			TUDebuger::ErrorLog(TEXT("跑马灯配置拉取失败：") + Error.GetErrorReason());
		}));
	} else {
		Block();
	}
}

void FTapBillboardPC::HandleFetchUnreadMarqueeGeneralData(const TArray<FAnnouncementGeneralData>& GeneralData)
{
	if (GeneralData.Num() > 0)
	{
		TArray<int64> Ids;
		for (const FAnnouncementGeneralData& Data : GeneralData)
		{
			Ids.Add(Data.id);
		}
		Rest_FetchAnnouncementsDetailData(Ids, FAnnouncementDetailsDataResult::CreateRaw(this, &FTapBillboardPC::HandleFetchMarqueeDetailData), FTapFailed());
	}
	else
	{
		PendingDetailsReverse.Reset();
	}
}

void FTapBillboardPC::HandleFetchMarqueeDetailData(const TArray<FAnnouncementDetailData>& DetailsData)
{
	PendingDetailsReverse.Reset();
	for (int32 i = DetailsData.Num() - 1; i >= 0; --i)
	{
		PendingDetailsReverse.Add_GetRef(DetailsData[i]);
	}
	
	if (PendingDetailsReverse.Num() != 0)
	{
		if (!IsValid(MarqueeWidget))
		{
			MarqueeWidget = CreateWidget<UTapBillboardMarquee>(TUSettings::GetGameInstance().Get(), MarqueeClass.Get());
		}

		if (!MarqueeWidget->IsInViewport())
		{
			MarqueeWidget->AddToViewport(TUSettings::GetUILevel());
		}
	}
}