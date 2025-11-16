#pragma once
#include "TUError.h"
#include "Model/AAURealNameResultModel.h"
#include "TUAuthResult.h"
#include "Model/China/AntiAddictionError.h"

class AAUChinaRealName {
public:

	// MARK: - 实名认证部分
	static void ManualVerify(const FString& UserID, const FString& Name, const FString& CardID,
		TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);

	static void CheckRealNameState(const FString& UserID, bool useAgeRange, TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);

	static void CheckRealNameStateByTapToken(const FString& UserID, const TSharedRef<FTUAccessToken> TapToken,  const FString& Timestamp, TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);
	
	static void CheckRealNameStateByUserId(const FString& UserID, TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);

};
