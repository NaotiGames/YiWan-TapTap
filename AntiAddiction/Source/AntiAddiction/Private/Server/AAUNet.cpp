#include "AAUNet.h"

#include "AAUImpl.h"
#include "AAURegionConfig.h"
#include "TapUELogin.h"
#include "TUCrypto.h"
#include "TUDebuger.h"
#include "TUDeviceInfo.h"
#include "TUHttpManager.h"
#include "TUJsonHelper.h"
#include "TUOpenSSL.h"
#include "Model/AAUUser.h"
#include "TUAccessToken.h"
#include "TUHelper.h"
#include "URLParser.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Model/China/AntiAddictionError.h"


FAntiAddictionError GenerateErrorInfo(const TSharedPtr<TUHttpResponse>& Response)
{
	FAntiAddictionError Error = FAntiAddictionError();
	Error.httpState = Response->state;
	Error.msg = TEXT("网络异常，请稍后重试");
	return Error;
}


template <typename StructType>
void PerfromResponseCallBack(const TSharedPtr<TUHttpResponse>& Response,
							 TFunction<void(TSharedPtr<StructType> ModelPtr, const FAntiAddictionError& Error)> Callback) {
	if (Callback == nullptr) {
		return;
	}
	FAntiAddictionError Error = GenerateErrorInfo(Response);

	auto JsonObject = TUJsonHelper::GetUStruct<StructType>(Response->contentString);

	Callback(JsonObject, Error);
}

template <typename StructType>
int64 GenerateWrapperResponse(const TSharedPtr<TUHttpResponse>& Response, TSharedPtr<StructType>& ModelPtr, FAntiAddictionError& Error)
{
	Error = GenerateErrorInfo(Response);

	if (Response->state == TUHttpResponse::serverError) {
		return 0;
	}

	const auto JsonObject = TUJsonHelper::GetJsonObject(Response->contentString);
	bool Success = false;
	const TSharedPtr<FJsonObject>* DataJsonObject = nullptr;
	if (JsonObject.IsValid() && JsonObject->TryGetBoolField("success", Success) && JsonObject->TryGetObjectField("data", DataJsonObject))
	{
		if (Success && Response->state == TUHttpResponse::success)
		{
			ModelPtr = TUJsonHelper::GetUStruct<StructType>(*DataJsonObject);
		} else
		{
			const auto ErrorPtr = TUJsonHelper::GetUStruct<FAntiAddictionError>(*DataJsonObject);
			if (ErrorPtr.IsValid())
			{
				Error = *ErrorPtr.Get();
				Error.httpState = Response->state;
			}
		}
		int64 timestamp = 0;
		JsonObject->TryGetNumberField("now", timestamp);
		return timestamp;
	}
	return 0;
}

// template <typename StructType>
// void GenerateAntiAddictionWrapperResponse(const TSharedPtr<TUHttpResponse>& Response, TSharedPtr<StructType>& ModelPtr, FAntiAddictionError& Error)
// {
// 	if (Response->state == TUHttpResponse::serverError) {
// 		return;
// 	}
//
// 	const auto JsonObject = TUJsonHelper::GetJsonObject(Response->contentString);
// 	bool Success = false;
// 	const TSharedPtr<FJsonObject>* DataJsonObject = nullptr;
// 	if (JsonObject.IsValid() && JsonObject->TryGetBoolField("success", Success) && JsonObject->TryGetObjectField("data", DataJsonObject))
// 	{
// 		if (Success)
// 		{
// 			ModelPtr = TUJsonHelper::GetUStruct<StructType>(*DataJsonObject);
// 		} else
// 		{
// 			const auto ErrorPtr = TUJsonHelper::GetUStruct<FAntiAddictionError>(*DataJsonObject);
// 			if (ErrorPtr.IsValid())
// 			{
// 				Error = *ErrorPtr.Get();
// 				Error.httpState = Response->state;
// 			}
// 		}
// 	}
// }

template <typename StructType>
void PerfromWrapperResponseCallBack(const TSharedPtr<TUHttpResponse>& Response, TFunction<void(TSharedPtr<StructType> ModelPtr, const FAntiAddictionError& Error)> Callback)
{
	if (Callback == nullptr)
	{
		return;
	}
	FAntiAddictionError Error;
	TSharedPtr<StructType> ModelPtr = nullptr;
	GenerateWrapperResponse(Response, ModelPtr, Error);
	Callback(ModelPtr, Error);
}


AAUNet::AAUNet() {
	TimeoutSecs = 10;
	Form = Json;
}

void AAUNet::GetServerTime(TFunction<void(TSharedPtr<FAAUServerTimeModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	const TSharedPtr<AAUNet> request = MakeShareable(new AAUNet());
	request->URL = AAURegionConfig::Get()->AntiAddictionUrl() / "server-time";
	request->onCompleted.BindLambda([=](TSharedPtr<TUHttpResponse> response) {
		PerfromWrapperResponseCallBack(response, CallBack);
	});
	TUHttpManager::Get().request(request);
}

template <typename ConfigType>
void CommonGetSDKConfig(const FString& UserId,TFunction<void(TSharedPtr<ConfigType> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	const TSharedPtr<AAUNet> request = MakeShareable(new AAUNet());
	request->URL = AAURegionConfig::Get()->RealNameUrl() / "get-global-config?client_id="  + AAUImpl::Config.ClientID+ "&user_identifier=" + FGenericPlatformHttp::UrlEncode(UserId);
	// AAUNet::AddUriParas(request->URL, "", request->PathParameters);
	request->RepeatCount = 3;
	request->onCompleted.BindLambda([=](TSharedPtr<TUHttpResponse> response) {
		PerfromWrapperResponseCallBack(response, CallBack);
	});
	TUHttpManager::Get().request(request);
}

void AAUNet::GetSDKConfig(const FString& UserId,TFunction<void(TSharedPtr<FAAURealNameConfigModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	CommonGetSDKConfig(UserId,CallBack);
}

void AAUNet::SetPayment(int Amount, const FAAUUser& User,
                        TFunction<void(TSharedPtr<FAAUPaymentModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	const TSharedPtr<AAUNet> request = MakeShareable(new AAUNet());
	request->URL = AAURegionConfig::Get()->AntiAddictionUrl() / "payment-submit?client_id="  + AAUImpl::Config.ClientID + "&user_identifier=" + FGenericPlatformHttp::UrlEncode(User.UserID);
	if (AAUImpl::bTestEnvEnable)
	{
		request->URL = request->URL + "&test_mode=1";
	}
	request->Type = Post;
	request->RepeatCount = 3;
	request->Headers.Add("X-TAP-Anti-Addiction-Token", User.AccessTokenV2);
	request->Parameters->SetNumberField("amount", Amount);
	request->onCompleted.BindLambda([=](TSharedPtr<TUHttpResponse> response) {
		PerfromWrapperResponseCallBack(response, CallBack);
	});
	TUHttpManager::Get().request(request);
}

void AAUNet::CheckPayment(int Amount, const FAAUUser& User,
	TFunction<void(TSharedPtr<FAAUPayableModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	const TSharedPtr<AAUNet> request = MakeShareable(new AAUNet());
	request->URL = AAURegionConfig::Get()->AntiAddictionUrl() / "payable?client_id="  + AAUImpl::Config.ClientID + "&user_identifier=" + FGenericPlatformHttp::UrlEncode(User.UserID) + "&amount=" + FString::FromInt(Amount);
	if (AAUImpl::bTestEnvEnable)
	{
		request->URL = request->URL + "&test_mode=1";
	}
	request->Type = Get;
	request->RepeatCount = 3;
	request->Headers.Add("X-TAP-Anti-Addiction-Token", User.AccessTokenV2);
	request->onCompleted.BindLambda([=](TSharedPtr<TUHttpResponse> response) {
		PerfromWrapperResponseCallBack(response, CallBack);
	});
	TUHttpManager::Get().request(request);
}

void AAUNet::CheckPlayable(const FString& UserID, const FString& Token, const FString& Session,
                           TFunction<void(TSharedPtr<FAAUPlayableModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	const TSharedPtr<AAUNet> request = MakeShareable(new AAUNet());
	request->URL = AAURegionConfig::Get()->AntiAddictionUrl() / "heartbeat?client_id=" + AAUImpl::Config.ClientID + "&user_identifier=" + FGenericPlatformHttp::UrlEncode(UserID) ;
	if (AAUImpl::bTestEnvEnable)
	{
		request->URL = request->URL + "&test_mode=1";
	}
	request->Type = Post;
	request->RepeatCount = 3;
	request->Headers.Add("X-TAP-Anti-Addiction-Token", Token);

	FString JsonStr;
	TSharedRef <TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonStr);
	JsonWriter->WriteObjectStart();
	JsonWriter->WriteValue("session_id", Session);
	JsonWriter->WriteObjectEnd(); 
	JsonWriter->Close();

	request->PostBodyString = JsonStr;
	auto object = TUJsonHelper::GetJsonObject(JsonStr);
	JsonStr = TUJsonHelper::GetJsonString(object);
	
	// TSharedPtr<FJsonObject> PlayLogsJsonObject = MakeShareable(new FJsonObject);
	// PlayLogsJsonObject->SetArrayField("server_times", ServerTimes);
	
	request->onCompleted.BindLambda([=](TSharedPtr<TUHttpResponse> response) {
		PerfromWrapperResponseCallBack(response, CallBack);
	});
	TUHttpManager::Get().request(request);
}



void AAUNet::ChinaManualVerify(const FString& UserID, const FString& Name, const FString& CardID,
                          TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	const TSharedPtr<AAUNet> request = MakeShareable(new AAUNet());
	request->URL = AAURegionConfig::Get()->RealNameUrl() / "anti-addiction-token-manual?client_id=" + AAUImpl::Config.ClientID + "&user_identifier=" + FGenericPlatformHttp::UrlEncode(UserID) ;
	
	request->Type = Post;
	request->RepeatCount = 3;
	request->Parameters->SetStringField("name", Name);
	request->Parameters->SetStringField("id_card", CardID);
	
	request->onCompleted.BindLambda([=](TSharedPtr<TUHttpResponse> response) {
		FAntiAddictionError Error;
		TSharedPtr<FAAURealNameResultModel> ModelPtr = nullptr;
		GenerateWrapperResponse(response, ModelPtr, Error);
		if (CallBack) {
			CallBack(ModelPtr, Error);
		}
	});
	TUHttpManager::Get().request(request);
}

void AAUNet::CheckRealNameStateByUserId(const FString& UserID,
                                TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	const TSharedPtr<AAUNet> request = MakeShareable(new AAUNet());
	request->URL = AAURegionConfig::Get()->RealNameUrl() / "anti-addiction-token?client_id=" + AAUImpl::Config.ClientID + "&user_identifier=" + FGenericPlatformHttp::UrlEncode(UserID) ;
	request->Type = Get;
	request->RepeatCount = 3;
	request->onCompleted.BindLambda([=](TSharedPtr<TUHttpResponse> response) {
		FAntiAddictionError Error;
		TSharedPtr<FAAURealNameResultModel> ModelPtr = nullptr;
		GenerateWrapperResponse(response, ModelPtr, Error);
		if (!ModelPtr.IsValid()) {
			if (response->state == TUHttpResponse::networkError) {
				Error.msg = TEXT("网络异常，请稍后重试");
			}
			else {
				Error.msg = TEXT("未查询到实名状态，防沉迷启动失败");
			}
		}
		if (CallBack) {
			CallBack(ModelPtr, Error);
		}
	});
	TUHttpManager::Get().request(request);
}

void AAUNet::CheckRealNameStateByTapToken(const FString& UserID, TSharedRef<FTUAccessToken> TapToken, FString Timestamp,
								TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error, const int64 Now)> CallBack) {
	const TSharedPtr<AAUNet> request = MakeShareable(new AAUNet());
	FString finalUrl = AAURegionConfig::Get()->RealNameUrl() / "anti-addiction-token-taptap?client_id="+ AAUImpl::Config.ClientID + "&user_identifier=" + FGenericPlatformHttp::UrlEncode(UserID);
	request->URL = finalUrl;
	request->Type = Get;
	request->RepeatCount = 3;
	request->Headers.Add("Authorization", GenerateTapAuthorization(finalUrl,TapToken,Timestamp));
	
	request->onCompleted.BindLambda([=](TSharedPtr<TUHttpResponse> response) {
		FAntiAddictionError Error;
		TSharedPtr<FAAURealNameResultModel> ModelPtr = nullptr;
		const int64 Now = GenerateWrapperResponse(response, ModelPtr, Error);
		if (CallBack) {
			CallBack(ModelPtr, Error, Now);
		}
	});
	TUHttpManager::Get().request(request);
}


void AAUNet::CheckRealNameStateByOldToken(const FString& UserID, const FString& OldToken,
								TFunction<void(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)> CallBack) {
	const TSharedPtr<AAUNet> request = MakeShareable(new AAUNet());
	request->URL = AAURegionConfig::Get()->RealNameUrl() / "anti-addiction-token-upgrade?client_id="+ AAUImpl::Config.ClientID + "&user_identifier=" + FGenericPlatformHttp::UrlEncode(UserID);
	request->Type = Post;
	request->RepeatCount = 3;
	request->Parameters->SetStringField("anti_addiction_token_v1", OldToken);
	
	request->onCompleted.BindLambda([=](TSharedPtr<TUHttpResponse> response) {
		FAntiAddictionError Error;
		TSharedPtr<FAAURealNameResultModel> ModelPtr = nullptr;
		GenerateWrapperResponse(response, ModelPtr, Error);
		if (!ModelPtr.IsValid()) {
			if (response->state == TUHttpResponse::networkError) {
				Error.msg = TEXT("网络异常，请稍后重试");
			}
			else {
				Error.msg = TEXT("未查询到实名状态，防沉迷启动失败");
			}
		}
		if (CallBack) {
			CallBack(ModelPtr, Error);
		}
	});
	TUHttpManager::Get().request(request);
}

void AAUNet::FetchUserConfig(const FString& UserID, const FString& Token, TFunction<void(TSharedPtr<FAAUserConfigModel> ModelPtr, const FAntiAddictionError& Error)> CallBack)
{
	const TSharedPtr<AAUNet> request = MakeShareable(new AAUNet());
	request->URL = AAURegionConfig::Get()->AntiAddictionUrl() / "get-config-by-token?client_id="+ AAUImpl::Config.ClientID + "&user_identifier=" + FGenericPlatformHttp::UrlEncode(UserID)
		 + "&platform=pc";
	if (AAUImpl::bTestEnvEnable)
	{
		request->URL = request->URL + "&test_mode=1";
	}
	request->Type = Get;
	request->RepeatCount = 3;
	request->Headers.Add("X-TAP-Anti-Addiction-Token", Token);
	
	request->onCompleted.BindLambda([=](TSharedPtr<TUHttpResponse> response) {
		FAntiAddictionError Error;
		TSharedPtr<FAAUserConfigModel> ModelPtr = nullptr;
		GenerateWrapperResponse(response, ModelPtr, Error);
		if (CallBack) {
			CallBack(ModelPtr, Error);
		}
	});
	TUHttpManager::Get().request(request);
}



// "/v3/fcm/authorizations"  // POST



TMap<FString, FString> AAUNet::CommonHeaders() {
	TMap<FString, FString> HeadMap = TUHttpRequest::CommonHeaders();
	FString UAStr = FString::Printf(TEXT("Version=%s&Platform=%s&Unreal-SDK-Version=%s"), AntiAddiction_UE_VERSION, *TUDeviceInfo::GetPlatform(), *TUDeviceInfo::GetEngineVersion());
	HeadMap.Add("UA", UAStr);
	HeadMap.Add("Accept-Language", "zh-CN");
	
	return HeadMap;
	
	// [request setValue:[requestParams objectForKey:ANTI_AUTH_KEY] forHTTPHeaderField:ANTI_AUTH_KEY]; Authorization
}

TSharedPtr<FJsonObject> AAUNet::CommonParameters() {
	auto _Parameters = TUHttpRequest::CommonParameters();
	// _Parameters->SetStringField("game", AAUImpl::Get()->Config.ClientID);
	// _Parameters->SetStringField("sdkVersion", AntiAddiction_UE_VERSION);
	return _Parameters;
}

bool AAUNet::ResetHeadersBeforeRequest() {
	return TUHttpRequest::ResetHeadersBeforeRequest();
}

FString AAUNet::GenerateTapAuthorization(FString originUrl, TSharedRef<FTUAccessToken> TapToken, FString Timestamp)
{
	FString FinalUrl = TUDebuger::GetReplacedUrl(originUrl);
	FString authToken;
	auto Parse = TUCommon::FURL_RFC3986();
	Parse.Parse(FinalUrl);
	FString timeStr ;
	if(Timestamp.IsEmpty())
	{
		timeStr = FString::Printf(TEXT("%lld"), FDateTime::UtcNow().ToUnixTimestamp());
	}else
	{
		timeStr = Timestamp;
	}
	FString nonce = TUHelper::GetRandomStr(5);
	// TUDebuger::DisplayLog(FString::Printf(TEXT("random: %s"), *nonce));
	FString md = "GET" ;

	FString pathAndQuery = Parse.GetPath();
	if (Parse.GetQuery().Len() > 0)
	{
		pathAndQuery += "?" + Parse.GetQuery();
	}
	FString domain = Parse.GetHost().ToLower();
	FString port = Parse.GetPort();

	FString dataStr = timeStr + "\n" + nonce + "\n" + md + "\n" + pathAndQuery + "\n" + domain + "\n" + port + "\n\n";
	auto sha1 = TUCrypto::HmacSHA1(TUCrypto::UTF8Encode(dataStr), TUCrypto::UTF8Encode(TapToken.Get().mac_key));
	FString mac = TUCrypto::Base64Encode(sha1);
	FString kid = TapToken.Get().kid;
	authToken = FString::Printf(TEXT("MAC id=\"%s\",ts=\"%s\",nonce=\"%s\",mac=\"%s\""), ToCStr(kid), *timeStr, *nonce, *mac);
	return authToken;
}




