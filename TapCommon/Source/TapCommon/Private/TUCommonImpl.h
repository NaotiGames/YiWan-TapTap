#pragma once
#include "TUType.h"
#include "Controller/TapControllerTypes.h"

class TUCommonImpl {
public:

	virtual ~TUCommonImpl() = default;

	static TSharedPtr<TUCommonImpl>& Get();
	
	virtual void SetLanguage(ELanguageType LanguageType);

	virtual void setDurationStatisticsEnabled(bool bEnable);

	virtual void IsLaunchedFromTapTapPC(TFunction<void(const bool Pass)> CallBack);
	
	virtual bool TapControllerStartup(ETapControllerType InControllerType);

	virtual void TapControllerUpdateConfig( const TSharedPtr<FTapControllerConfig>& InConfig);
	
	virtual void TapControllerShutdown();

	virtual bool IsTapControllerAlreadyStarted() const;

	virtual bool IsTapControllerInputAwake() const;
	
	virtual void SetXUA();

	// 是否支持 Tap 客户端授权
	virtual bool IsSupportLoginByTapClient();

	// 通过 PC 客户端获取授权信息
	virtual void TapLoginByTapPCClient(TArray<FString> Permissions, FString ResponseType,
		FString RedirectUri, FString CodeChallenge, FString State, FString CodeChallengeMethod,
		FString Version, FString SdkUa, FString Info, TFunction<void(const bool IsCancel,const bool IsFail, FString ErrMsg,   FString RedirectUrl)> CallBack);

private:
	static TSharedPtr<TUCommonImpl> Instance;
	
};
