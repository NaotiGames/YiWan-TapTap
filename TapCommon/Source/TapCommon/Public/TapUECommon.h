#pragma once
#include "TUType.h"
#include "Controller/TapControllerTypes.h"

class TAPCOMMON_API TapUECommon {
public:
	static void SetLanguage(ELanguageType LanguageType);

	// UE5 如果开启时长统计有兼容性问题，可能导致偶发闪退
	static void setDurationStatisticsEnabled(bool bEnable);
	
	static bool TapControllerStartup(ETapControllerType InControllerType);

	static void TapControllerUpdateConfig(const TSharedPtr<FTapControllerConfig>& InConfig);
	
	static void TapControllerShutdown();

	static bool IsTapControllerRunning();

	static void IsLaunchedFromTapTapPC(TFunction<void(const bool Pass)> CallBack);

	// 是否支持 Tap 客户端授权
	static bool IsSupportLoginByTapClient();

	// 通过 PC 客户端获取授权信息
	static void TapLoginByTapPCClient(TArray<FString> Permissions, FString ResponseType,
		FString RedirectUri, FString CodeChallenge, FString State, FString CodeChallengeMethod,
		FString Version, FString SdkUa, FString Info, TFunction<void(const bool IsCancel,  const bool IsFail, FString ErrMsg, FString RedirectUrl)> CallBack);

	static bool QueryTapDLC(FString dlcId);
	static bool ShowStoreWithDLC(FString dlcId);
	static bool HasTapLicense();

	static void RegisterDLCOwnedCallback(TFunction<void(FString DLCId, const bool IsOwned)> Callback);
	static void RegisterLicenseCallback(TFunction<void(const bool IsOwned)> Callback);
	static FOnTapControllerRunningStateChanged PreTapControllerRunningStateChange;

	static FOnTapControllerRunningStateChanged PostTapControllerRunningStateChanged;

	//设置引擎信息到移动端
	static void SetXUA();
};
