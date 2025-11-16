#pragma once
#include "TapCommon.h"
#include "TULoginImpl.h"
#include "MessageEndpoint.h"

class IMessageContext;

class TULoginPCImpl: public TULoginImpl {
private:
	TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint;
public:
	static bool IsCacheUserSameWithTapClient;
	virtual void Init(FTULoginConfig _Config) override;
	virtual TSharedPtr<FTULoginProfileModel> GetProfile() override;
	virtual void FetchProfile(
		TFunction<void(TSharedPtr<FTULoginProfileModel> ModelPtr, const FTUError& Error)> CallBack) override;
	virtual TSharedPtr<FTUAccessToken> GetAccessToken() override;
	virtual void Login(TArray<FString> Permissions, TFunction<void(const TUAuthResult& Result)> CallBack) override;
	virtual void Logout() override;
	virtual void GetAntiAddictionCode(const FTUAccessToken& Token,
		TFunction<void(TSharedPtr<FString> codePtr, const FTUError& Error)> CallBack) override;
	virtual void CheckAndRefreshToken();
	static void FetchUserByCode(const FString& Code, const FString& CodeVerifier, TFunction<void(const TUAuthResult& Result)> CallBack);

	void RegisterSyncTapUserListener();
	void HandleSyncTapUserMessage(const FTapCustomMessage& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context);
};
