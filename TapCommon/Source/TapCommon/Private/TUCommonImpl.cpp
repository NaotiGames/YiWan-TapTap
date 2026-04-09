#include "TUCommonImpl.h"

#include "TUDebuger.h"
#if PLATFORM_MAC || PLATFORM_WINDOWS
#include "Desktop/TUCommonPCImpl.h"
#elif PLATFORM_IOS || PLATFORM_ANDROID
#include "Mobile//TUCommonMobileImpl.h"
#endif

#define UnsupportedPlatformsLog TUDebuger::ErrorLog(FString::Printf(TEXT("Unsupported Platforms Call %s"), ANSI_TO_TCHAR(__FUNCTION__)));

TSharedPtr<TUCommonImpl> TUCommonImpl::Instance = nullptr;

TSharedPtr<TUCommonImpl>& TUCommonImpl::Get() {
	if (!Instance.IsValid()) {
#if PLATFORM_MAC || PLATFORM_WINDOWS
		Instance = MakeShareable(new TUCommonPCImpl);
#elif PLATFORM_IOS || PLATFORM_ANDROID
		Instance = MakeShareable(new TUCommonMobileImpl);
#else
		Instance = MakeShareable(new TUCommonImpl);
#endif
	}
	return Instance;
}

void TUCommonImpl::SetLanguage(ELanguageType LanguageType) {
	UnsupportedPlatformsLog
}

void TUCommonImpl::setDurationStatisticsEnabled(bool bEnable) {
	UnsupportedPlatformsLog
}

void TUCommonImpl::IsLaunchedFromTapTapPC(TFunction<void(const bool Pass)> CallBack)
{
	UnsupportedPlatformsLog
}

void TUCommonImpl::RegisterTapTapPCStateChangeListener(TapTapPCStateChangeListener Listener)
{
	UnsupportedPlatformsLog
}

void TUCommonImpl::UnRegisterTapTapPCStateChangeListener(TapTapPCStateChangeListener Listener)
{
	UnsupportedPlatformsLog
}


bool TUCommonImpl::TapControllerStartup(ETapControllerType InControllerType)
{
	UnsupportedPlatformsLog
	return false;
}

void TUCommonImpl::TapControllerUpdateConfig(const TSharedPtr<FTapControllerConfig>& InConfig)
{
	UnsupportedPlatformsLog
}

void TUCommonImpl::TapControllerShutdown()
{
	UnsupportedPlatformsLog
}

bool TUCommonImpl::IsTapControllerAlreadyStarted() const
{
	UnsupportedPlatformsLog
	return false;
}

bool TUCommonImpl::IsTapControllerInputAwake() const
{
	UnsupportedPlatformsLog
	return false;
}

void TUCommonImpl::SetXUA() {
	UnsupportedPlatformsLog
}

// 是否支持 Tap 客户端授权
bool TUCommonImpl::IsSupportLoginByTapClient()
{
	UnsupportedPlatformsLog
	return false;
}

// 通过 PC 客户端获取授权信息
void TUCommonImpl::TapLoginByTapPCClient(TArray<FString> Permissions, FString ResponseType,
		FString RedirectUri, FString CodeChallenge, FString State, FString CodeChallengeMethod,
		FString Version, FString SdkUa, FString Info, TFunction<void(const bool IsCancel,const bool IsFail, FString ErrMsg,   FString RedirectUrl)> CallBack)
{
	UnsupportedPlatformsLog
}

bool TUCommonImpl::QueryTapDLC(FString dlcId)
{
	UnsupportedPlatformsLog
	return false;
}
bool TUCommonImpl::ShowStoreWithDLC(FString dlcId)
{
	UnsupportedPlatformsLog
	return false;
}
bool TUCommonImpl::HasTapLicense()
{
	UnsupportedPlatformsLog
	return false;
}

void TUCommonImpl::RegisterDLCOwnedCallback(TFunction<void(FString DLCId, const bool IsOwned)> Callback)
{
	UnsupportedPlatformsLog
}
void TUCommonImpl::RegisterLicenseCallback(TFunction<void(const bool IsOwned)> Callback)
{
	UnsupportedPlatformsLog
}


