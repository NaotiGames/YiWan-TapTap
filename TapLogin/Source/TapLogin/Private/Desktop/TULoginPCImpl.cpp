#include "TULoginPCImpl.h"

#include "TapSubsystem.h"
#include "TapUECommon.h"
#include "TULoginLanguage.h"
#include "Server/TULoginNet.h"
#include "Slate/TapLoginWidget.h"
#include "TUDebuger.h"
#include "TUWebAuthHelper.h"
#include "TUOpenSSL.h"
#include "TUCrypto.h"
#include "TUHelper.h"
#include "MessageEndpointBuilder.h"
#include "TUDeviceInfo.h"
#include "Server/TULoginRegionConfig.h"

bool TULoginPCImpl::IsCacheUserSameWithTapClient = true;

void TULoginPCImpl::Init(FTULoginConfig _Config) {
	FTUConfig::Get()->ClientID = _Config.ClientID;
	FTUConfig::Get()->RegionType = _Config.RegionType;
	this->Config = _Config;
	CheckAndRefreshToken();
	RegisterSyncTapUserListener();
}

TSharedPtr<FTULoginProfileModel> TULoginPCImpl::GetProfile() {
	return FTULoginProfileModel::GetLocalModel();
}

void TULoginPCImpl::FetchProfile(
	TFunction<void(TSharedPtr<FTULoginProfileModel> ModelPtr, const FTUError& Error)> CallBack) {

	auto AccessTokenPtr = GetAccessToken();
	if (!AccessTokenPtr.IsValid() && CallBack != nullptr) {
		CallBack(nullptr, FTUError(-1, "no login"));
		return;
	}
	TULoginNet::RequestProfile(*AccessTokenPtr.Get(), [=](TSharedPtr<FTULoginProfileModel> Model, FTULoginError Error) {
		if (CallBack) {
			CallBack(Model, FTUError(Error.code, Error.msg));
		}
	});
}

TSharedPtr<FTUAccessToken> TULoginPCImpl::GetAccessToken() {
	return FTUAccessToken::GetLocalModel();
}

void TULoginPCImpl::Login(TArray<FString> Permissions, TFunction<void(const TUAuthResult& Result)> CallBack) {
	Permissions.RemoveSingle(TUType::PermissionScope::BasicInfo);
	Permissions.AddUnique(TUType::PermissionScope::Profile);
	Permissions.Append(AdditionalPermissions);
	if (Config.RegionType == ERegionType::Global)
	{
		Permissions.Remove(TEXT("compliance"));
	}
	if (TapUECommon::IsSupportLoginByTapClient())
	{
		FString State = FGuid::NewGuid().ToString();
		FString CodeVerifier = TUHelper::GetRandomStr(16);
		FString Version = TapCommon_UE_VERSION;
		FString Info = FString::Printf(TEXT("{\"device_id\":\"%s\"}"), *FGenericPlatformMisc::GetDeviceMakeAndModel());
		FString SdkUA = "client_id=" + TULoginImpl::Get()->Config.ClientID + "&uuid=" + *TUDeviceInfo::GetLoginId();
		TapUECommon::TapLoginByTapPCClient(Permissions, "code", "tapoauth://authorize",
			TUCrypto::UrlBase64Encode(TUOpenSSL::Sha256(TUCrypto::UTF8Encode(CodeVerifier))),
				State, "S256", Version, SdkUA, Info, [State, CodeVerifier,CallBack](bool isCancel,  bool IsFail, FString ErrMsg, FString uri)  {
			if(isCancel)
			{
				CallBack(TUAuthResult::CancelInit());
			}else if(IsFail)
			{
				FTUError TapError;
				TapError.code = -1;
				TapError.error_description = ErrMsg.IsEmpty() ? "发起授权失败" : ErrMsg;
				CallBack(TUAuthResult::FailInit(TapError));
			}
			else
			{
				FString QueryString;
				if (uri.Split(TEXT("?"), nullptr, &QueryString))
				{
					TArray<FString> Pairs;
					QueryString.ParseIntoArray(Pairs, TEXT("&"), true);
					TMap<FString, FString> QueryParams;
					for (const FString& Pair : Pairs)
					{
						FString Key, Value;
						if (Pair.Split(TEXT("="), &Key, &Value))
						{
							QueryParams.Add(Key, Value);
						}
					}
					FString Code = QueryParams.Contains("code") ? QueryParams["code"] : "";
					FString UriState =  QueryParams.Contains("state") ? QueryParams["state"] : "";
					FString Error = QueryParams.Contains("error") ?  QueryParams["error"] :  "";
					// FString Name = QueryParams.Contains("name") ? QueryParams["name"] : "";
					if(Error.IsEmpty() && UriState == State && !Code.IsEmpty())
					{
						FetchUserByCode(Code, CodeVerifier, CallBack);
					}else
					{
						FTUError TapError;
						TapError.code = -1;
						TapError.error_description = Error + ".\t" + "Get profile error";
						CallBack(TUAuthResult::FailInit(TapError));
					}
				}else
				{
					FTUError TapError;
					TapError.code = -1;
					TapError.error_description = "fail to parse login url.\t Get profile error";
					CallBack(TUAuthResult::FailInit(TapError));
				}
			}
		});
	}else
	{
		UTapSubsystem::AddWidget(SNew(STapLoginWidget, Permissions, Config.RegionType)
			.OnAuthComplete(FTapAuthComplete::CreateLambda(CallBack)));
	}
}

void TULoginPCImpl::FetchUserByCode(const FString& Code, const FString& CodeVerifier, TFunction<void(const TUAuthResult& Result)> CallBack)
{
	const TSharedPtr<FJsonObject> Paras = MakeShareable(new FJsonObject);
	Paras->SetStringField("client_id", TULoginImpl::Get()->Config.ClientID);
	Paras->SetStringField("grant_type", "authorization_code");
	Paras->SetStringField("secret_type", "hmac-sha-1");
	Paras->SetStringField("code", Code);
	Paras->SetStringField("redirect_uri", "tapoauth://authorize");
	Paras->SetStringField("code_verifier", CodeVerifier);
						
	TULoginNet::RequestAccessTokenFromWeb(
		Paras,
		[CallBack](TSharedPtr<FTUAccessToken> Model, FTULoginError Error) {
							
			if (Model.IsValid())
			{
				TULoginNet::RequestProfile(
				*Model.Get(),
				[Model,CallBack](TSharedPtr<FTULoginProfileModel> ProfileModel, FTULoginError Error)
				{
					if (ProfileModel.IsValid())
					{
						Model->SaveToLocal();
						ProfileModel->SaveToLocal();
						// STapToast::Show(ETapToastType::TapTap, FText::FromString(FString::Format(*TULoginLanguage::GetCurrentLang()->login_account_logged_tip(), {Model->name})), Model->avatar);
						CallBack(TUAuthResult::SuccessInit(Model));
					}
					else
					{
						FTUError TapError;
						TapError.code = Error.code;
						TapError.error_description = Error.error_description + ".\t" + "Get profile error";
						CallBack(TUAuthResult::FailInit(TapError));
					}
				});
				
			}
			else {
					FTUError TapError;
					TapError.code = Error.code;
					TapError.error_description = Error.error_description;
					CallBack(TUAuthResult::FailInit(TapError));
			}
	});
}

void TULoginPCImpl::RegisterSyncTapUserListener()
{
	if(!MessageEndpoint.IsValid())
	{
		MessageEndpoint =FMessageEndpoint::Builder("TapUserSyncBus")
							.ReceivingOnAnyThread()
							.Handling<FTapCustomMessage>(this, &TULoginPCImpl::HandleSyncTapUserMessage);
		if(MessageEndpoint.IsValid())
		{
			MessageEndpoint->Subscribe<FTapCustomMessage>();
		}
	}
}

void TULoginPCImpl::HandleSyncTapUserMessage(const FTapCustomMessage& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	if (Message.MessageName == "syncTapUser")
	{
		FString CacheOpenId = Message.MessageContent;
		TSharedPtr<FTULoginProfileModel> CurrentProfile = GetProfile();
		if(CurrentProfile.IsValid())
		{
			FString OpenId = CurrentProfile.Get()->openid;
			if(!OpenId.IsEmpty() && OpenId != CacheOpenId)
			{
				TUDebuger::DisplayLog(" current openid = " + OpenId + " cacheId = " + CacheOpenId);
				IsCacheUserSameWithTapClient = false;
				Logout();
			}else
			{
				TUDebuger::DisplayLog(" current openid = " + OpenId + " cacheId = " + CacheOpenId
					+ " is same");
			}
		}
	}
}



void TULoginPCImpl::Logout() {
	FTULoginProfileModel::ClearLocalModel();
	FTUAccessToken::ClearLocalModel();
}

void  TULoginPCImpl::CheckAndRefreshToken() {
	TUDebuger::DisplayLog("CheckAndRefreshToken start");
	auto AccessTokenPtr = GetAccessToken();
	if (AccessTokenPtr.IsValid()){
		TUDebuger::DisplayLog("CheckAndRefreshToken has local token");
		TULoginNet::RefreshToken(*AccessTokenPtr.Get()->access_token, [=](TSharedPtr<FTUAccessToken> Model, FTULoginError Error) {
			if (Model.IsValid()) {
				TUDebuger::DisplayLog("CheckAndRefreshToken refresh token success ");
				if(IsCacheUserSameWithTapClient)
				{
					TUDebuger::DisplayLog("CheckAndRefreshToken refresh token And save data");
					Model->SaveToLocal();
					auto FTULoginProfileModelPtr = GetProfile();
					if(!FTULoginProfileModelPtr.IsValid()){
						FetchProfile([=](TSharedPtr<FTULoginProfileModel> ModelPtr, const FTUError& Error)
						{
							if(ModelPtr.IsValid() && IsCacheUserSameWithTapClient)
							{
								ModelPtr->SaveToLocal();
							}
						});
					}
				}
			}else{
				TUDebuger::DisplayLog(FString::Printf(TEXT("CheckAndRefreshToken fail code = %d, msg = %s "), Error.code ,*Error.error));
				if(!Error.error.IsEmpty() && Error.error.Equals("invalid_grant")){
					TUDebuger::DisplayLog(FString::Printf(TEXT("CheckAndRefreshToken fail delete cache")));
					Logout();
				}
			}
		});
	}
}

void TULoginPCImpl::GetAntiAddictionCode(const FTUAccessToken& Token,
	TFunction<void(TSharedPtr<FString> codePtr, const FTUError& Error)> CallBack) {
	TULoginNet::RequestRealNameCode(Token, [=](TSharedPtr<FTUAntiAddictionModel> Model, FTULoginError Error) {
		if (!CallBack) {
			return;
		}
		if (Model.IsValid()) {
			CallBack(MakeShared<FString>(Model->code), FTUError(Error.code, Error.msg));
		} else {
			CallBack(nullptr, FTUError(Error.code, Error.msg));
		}
	});
	
}
