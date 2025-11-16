#pragma once
#include "TUError.h"
#include "TUHttpRequest.h"
#include "Model/AAUPayableModel.h"
#include "Model/AAUPlayableModel.h"
#include "Model/AAURealNameResultModel.h"
#include "Model/AAUServerTimeModel.h"
#include "Model/AAUSettingsModel.h"
#include "Model/AAUUser.h"
#include "Model/China/AAURealNameConfigModel.h"
#include "Model/China/AAUserConfigModel.h"


struct FAAUserConfigModel;
struct FTUAccessToken;
struct FAntiAddictionError;

DECLARE_DELEGATE_TwoParams(FAntiAddictionRealNameResult, TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error);
class AAUNet: public TUHttpRequest
{
public:

	AAUNet();

	static void GetServerTime(TFunction<void(TSharedPtr<FAAUServerTimeModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);
	
	static void GetSDKConfig(const FString& UserId,TFunction<void(TSharedPtr<FAAURealNameConfigModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);

	/// 提交已付费金额
	static void SetPayment(int Amount, const FAAUUser& User, TFunction<void(TSharedPtr<FAAUPaymentModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);
	/// 检查付费限制
	static void CheckPayment(int Amount, const FAAUUser& User, TFunction<void(TSharedPtr<FAAUPayableModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);

	

	// "is_login":0,   // 1:登录时调用  0：游戏中调用
	static void CheckPlayable(const FString& UserID, const FString& Token, const FString& Session,
		TFunction<void(TSharedPtr<FAAUPlayableModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);

	// MARK: - 实名认证部分
	static void ChinaManualVerify(const FString& UserID, const FString& Name, const FString& CardID,
		TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);
	
	static void CheckRealNameStateByUserId(const FString& UserID, TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);

	static void CheckRealNameStateByTapToken(const FString& UserID,  TSharedRef<FTUAccessToken> TapToken, FString Timestamp,TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error, const int64 Now)> CallBack);

	static void CheckRealNameStateByOldToken(const FString& UserID, const FString& OldToken, TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);

	static void FetchUserConfig(const FString& UserID, const FString& Token, TFunction<void(TSharedPtr<FAAUserConfigModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);

	static FString GenerateTapAuthorization(FString FinalUrl, TSharedRef<FTUAccessToken> TapToken, FString Timestamp);
	
	

private:
	virtual TMap<FString, FString> CommonHeaders() override;
	virtual TSharedPtr<FJsonObject> CommonParameters() override;
	virtual bool ResetHeadersBeforeRequest() override;
	
	
};
