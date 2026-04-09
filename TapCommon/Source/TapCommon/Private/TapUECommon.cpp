#include "TapUECommon.h"

#include "TUCommonImpl.h"

FOnTapControllerRunningStateChanged TapUECommon::PreTapControllerRunningStateChange;
FOnTapControllerRunningStateChanged TapUECommon::PostTapControllerRunningStateChanged;

void TapUECommon::SetLanguage(ELanguageType LanguageType) {
	TUCommonImpl::Get()->SetLanguage(LanguageType);
}

void TapUECommon::setDurationStatisticsEnabled(bool bEnable) {
	TUCommonImpl::Get()->setDurationStatisticsEnabled(bEnable);
}

void TapUECommon::IsLaunchedFromTapTapPC(TFunction<void(const bool Pass)> CallBack)
{
	TUCommonImpl::Get()->IsLaunchedFromTapTapPC(CallBack);
}

void TapUECommon:: RegisterTapTapPCStateChangeListener(TapTapPCStateChangeListener Listener)
{
	TUCommonImpl::Get()->RegisterTapTapPCStateChangeListener(Listener);
}

void TapUECommon:: UnRegisterTapTapPCStateChangeListener(TapTapPCStateChangeListener Listener)
{
	TUCommonImpl::Get()->UnRegisterTapTapPCStateChangeListener(Listener);
}



bool TapUECommon::TapControllerStartup(ETapControllerType InControllerType)
{
	return TUCommonImpl::Get()->TapControllerStartup(InControllerType);
}

void TapUECommon::TapControllerUpdateConfig(const TSharedPtr<FTapControllerConfig>& InConfig)
{
	TUCommonImpl::Get()->TapControllerUpdateConfig(InConfig);
}

void TapUECommon::TapControllerShutdown()
{
	TUCommonImpl::Get()->TapControllerShutdown();
}

bool TapUECommon::IsTapControllerRunning()
{
	return TUCommonImpl::Get()->IsTapControllerInputAwake();
}

void TapUECommon::SetXUA() {
	TUCommonImpl::Get()->SetXUA();
}

// 是否支持 Tap 客户端授权
bool TapUECommon::IsSupportLoginByTapClient() {
	return TUCommonImpl::Get()->IsSupportLoginByTapClient();
}

// 通过 PC 客户端获取授权信息
void TapUECommon::TapLoginByTapPCClient(TArray<FString> Permissions, FString ResponseType,
		FString RedirectUri, FString CodeChallenge, FString State, FString CodeChallengeMethod,
		FString Version, FString SdkUa, FString Info, TFunction<void(const bool IsCancel,const bool IsFail, FString ErrMsg,   FString RedirectUrl)> CallBack) {
	TUCommonImpl::Get()->TapLoginByTapPCClient(Permissions, ResponseType,
	RedirectUri, CodeChallenge, State, CodeChallengeMethod,Version, SdkUa, Info, CallBack);
}

bool TapUECommon::QueryTapDLC(FString dlcId)
{
	return TUCommonImpl::Get()->QueryTapDLC(dlcId);
}
bool TapUECommon::ShowStoreWithDLC(FString dlcId)
{
	return TUCommonImpl::Get()->ShowStoreWithDLC(dlcId);
}
bool TapUECommon::HasTapLicense()
{
	return TUCommonImpl::Get()->HasTapLicense();
}

void TapUECommon::RegisterDLCOwnedCallback(TFunction<void(FString DLCId, const bool IsOwned)> Callback)
{
	TUCommonImpl::Get()->RegisterDLCOwnedCallback(Callback);
}

void TapUECommon::RegisterLicenseCallback(TFunction<void(const bool IsOwned)> Callback)
{
	TUCommonImpl::Get()->RegisterLicenseCallback(Callback);
}
