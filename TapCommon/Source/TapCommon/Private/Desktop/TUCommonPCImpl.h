#pragma once
#include "TUCommonImpl.h"


class TUCommonPCImpl: public TUCommonImpl {
	
public:

	static bool HasCheckLaunchStateByTapClient;
	static int LastIsLaunchByTapPCResult ;
	static FDelegateHandle TickDelegateHandle;

	
	static TFunction<void(const bool IsCancel, const bool IsFail, FString ErrMsg,   FString RedirectUrl)> CurrentLoginCallBack;
	static TFunction<void(FString DLCId, const bool IsOwned)> CurrentDLCCallBack ;
	static TFunction<void(const bool IsOwned)> CurrentLicenseCallBack ;

	virtual void SetLanguage(ELanguageType LanguageType) override;

	virtual void IsLaunchedFromTapTapPC(TFunction<void(const bool Pass)> CallBack) override;
	
	virtual bool TapControllerStartup(ETapControllerType InControllerType) override;

	virtual void TapControllerUpdateConfig(const TSharedPtr<FTapControllerConfig>& InConfig) override;
	
	virtual void TapControllerShutdown() override;

	virtual bool IsTapControllerAlreadyStarted() const override;

	virtual bool IsTapControllerInputAwake() const override;

	// 是否支持 Tap 客户端授权
	virtual bool IsSupportLoginByTapClient() override;

	// 通过 PC 客户端获取授权信息
	virtual void TapLoginByTapPCClient(TArray<FString> Permissions, FString ResponseType,
		FString RedirectUri, FString CodeChallenge, FString State, FString CodeChallengeMethod,
		FString Version, FString SdkUa, FString Info, TFunction<void(const bool IsCancel, const bool IsFail, FString ErrMsg,  FString RedirectUrl)> CallBack) override;

	virtual  bool QueryTapDLC(FString dlcId) override;
	virtual  bool ShowStoreWithDLC(FString dlcId) override;
	virtual  bool HasTapLicense() override;

	virtual  void RegisterDLCOwnedCallback(TFunction<void(FString DLCId, const bool IsOwned)> Callback) override;
	virtual  void RegisterLicenseCallback(TFunction<void(const bool IsOwned)> Callback) override;

	
	static void TapLoginDelegate(int CallbackId, void *UserData);

	static void TapLicenseDelegate(int CallbackId, void *UserData);

	static void TapDLCDelegate(int CallbackId, void *UserData);

	// 用于接收 PC 客户端 回调
	static void OnWorldTickListener(UWorld* World, ELevelTick TickType, float DeltaTime);

};
