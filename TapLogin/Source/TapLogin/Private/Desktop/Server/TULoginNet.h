#pragma once
#include "TUAccessToken.h"
#include "TUAntiAddictionModel.h"
#include "TULoginError.h"
#include "TULoginProfileModel.h"
#include "Desktop/TUQrCodeModel.h"
#include "TUHttpRequest.h"


class TULoginNet: public TUHttpRequest
{
public:

	TULoginNet();
	

	static void RequestLoginQrCode(const TArray<FString> Permissions, TFunction<void(TSharedPtr<FTUQrCodeModel> Model, FTULoginError Error)> callback);
	static void RequestAccessToken(const FString& DeviceCode, TFunction<void(TSharedPtr<FTUAccessToken> Model, FTULoginError Error)> callback);
	static void RequestProfile(const FTUAccessToken& AccessToken, TFunction<void(TSharedPtr<FTULoginProfileModel> Model, FTULoginError Error)> callback);
	static void RequestAccessTokenFromWeb(const TSharedPtr<FJsonObject>& Paras, TFunction<void(TSharedPtr<FTUAccessToken> Model, FTULoginError Error)> callback);
	static void RequestRealNameCode(const FTUAccessToken& AccessToken, TFunction<void(TSharedPtr<FTUAntiAddictionModel> Model, FTULoginError Error)> Callback);
	static void RefreshToken(const FString& AccessToken, TFunction<void(TSharedPtr<FTUAccessToken> Model, FTULoginError Error)> callback);

	static FTULoginError GenerateErrorInfo(const TSharedPtr<TUHttpResponse>& Response);
private:
	virtual TMap<FString, FString> CommonHeaders() override;
	virtual TSharedPtr<FJsonObject> CommonParameters() override;
	virtual bool ResetHeadersBeforeRequest() override;

	TSharedPtr<FTUAccessToken> AccessToken = nullptr;
	// 上次服务端返回的时间戳，用于 profile 接口无效时间戳适配
	static  FString LastProfileSeverTimestamp ;
	FString GetMacToken();

	
};
