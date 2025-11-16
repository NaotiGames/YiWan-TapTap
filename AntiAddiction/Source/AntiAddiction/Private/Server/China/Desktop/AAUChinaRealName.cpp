#include "AAUChinaRealName.h"

#include <string>

#include "AAUChinaImpl.h"
#include "TapUELogin.h"
#include "TUDataStorage.h"
#include "Server/AAUNet.h"
#include "Server/AAUStorage.h"


void AAUChinaRealName::ManualVerify(const FString& UserID, const FString& Name, const FString& CardID,
                                    TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)>
                                    CallBack) {
	AAUNet::ChinaManualVerify(UserID, Name, CardID, [=](TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error) {
		if (!CallBack) {
			return;
		}
		CallBack(ModelPtr, Error);
	});
}

void AAUChinaRealName::CheckRealNameState(const FString& UserID, bool useAgeRange,
	TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	TSharedPtr<FAAUUser> LoginUser = TUDataStorage<FAAUStorage>::LoadStruct<FAAUUser>(FAAUStorage::HasLoginedUser + UserID);
	FString tokenV2 = "", token = "";
	if(LoginUser.IsValid())
	{
		token = LoginUser->AccessToken;
		tokenV2 = LoginUser->AccessTokenV2;
	}
	if(!tokenV2.IsEmpty())
	{
		FAAURealNameResultModel* model = new FAAURealNameResultModel();
		model->anti_addiction_token = tokenV2;
		model->status = "pass";
		TSharedPtr<FAAURealNameResultModel> ModelPtr = MakeShareable(model);
		CallBack(ModelPtr, FAntiAddictionError());
		return;
	}
	if(!token.IsEmpty())
	{
		CheckRealNameStateByOldToken(UserID, token, [=](TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error){
			if(ModelPtr.IsValid())
			{
				CallBack(ModelPtr,Error);
			}else
			{
				TSharedPtr<FTUAccessToken> AccessToken = TapUELogin::GetAccessToken();
				if(AAUChinaImpl::HasComplianceInTapToken(AccessToken, useAgeRange))
				{
					CheckRealNameStateByTapToken(UserID, AccessToken.ToSharedRef(),TEXT(""), CallBack);
				}else
				{
					CheckRealNameStateByUserId(UserID,CallBack);
				}
			}
		});
	}else
	{
		TSharedPtr<FTUAccessToken> AccessToken = TapUELogin::GetAccessToken();
		if( AAUChinaImpl::HasComplianceInTapToken(AccessToken, useAgeRange))
		{
			CheckRealNameStateByTapToken(UserID, AccessToken.ToSharedRef(),TEXT(""),CallBack);
		}else
		{
			CheckRealNameStateByUserId(UserID,CallBack);
		}
	}
}

void AAUChinaRealName::CheckRealNameStateByTapToken(const FString& UserID, const TSharedRef<FTUAccessToken> TapToken, const FString& Timestamp,
	TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	AAUNet::CheckRealNameStateByTapToken(UserID, TapToken,Timestamp, [=](TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error, const int64 Now)
	{
		if(!ModelPtr.IsValid())
		{
			if(Error.error.Equals("invalid_time"))
			{
				FString time = Now > 0 ? FString::FromInt(Now) : TEXT("");
				CheckRealNameStateByTapToken(UserID, TapToken,time,CallBack);
				return;
			}
		}
		CallBack(ModelPtr,Error);
	});
}

void AAUChinaRealName::CheckRealNameStateByOldToken(const FString& UserID,  const FString& OldToken,
	TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	AAUNet::CheckRealNameStateByOldToken(UserID, OldToken,[=](TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)->void
	{
		if(Error.httpState != TUHttpResponse::networkError && Error.httpState != TUHttpResponse::serverError)
		{
			TSharedPtr<FAAUUser> LoginUser = TUDataStorage<FAAUStorage>::LoadStruct<FAAUUser>(FAAUStorage::HasLoginedUser + UserID);
			if(LoginUser.IsValid())
			{
				LoginUser->AccessToken = "";
				TUDataStorage<FAAUStorage>::SaveStruct(FAAUStorage::HasLoginedUser + UserID, LoginUser);
			}
			CallBack(ModelPtr,Error);
		}
	});
}

void AAUChinaRealName::CheckRealNameStateByUserId(const FString& UserID,
	TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	AAUNet::CheckRealNameStateByUserId(UserID, CallBack);
}

