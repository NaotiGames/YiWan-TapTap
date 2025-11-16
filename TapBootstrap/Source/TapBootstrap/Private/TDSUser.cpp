#include "TDSUser.h"

#include "JsonObjectConverter.h"
#include "TUJsonHelper.h"
#include "TapBootstrapImpl.h"
#include "TapUELogin.h"
#include "TUDeviceInfo.h"
#include "TUMobileBridge.h"

#if PLATFORM_IOS
#include "IOSHelper.h"
#import <LeanCloudObjc/LeanCloudObjc.h>
#elif PLATFORM_ANDROID
#include "TapJNI.h"
#else

#endif

#define BOOTSTRAP_SERVICE "TapBootstrapService"
#define LOGIN_RESULT_CODE_KEY "loginCallbackCode"
#define USER_STATUS_CODE_KEY "userStatusCallbackCode"
#define BRIDGE_DATA_KEY "wrapper"

#define CALLBACK_SUCCESS_CODE 0

FString FTDSUser::KeyNickName = "nickname";
FString FTDSUser::KeyAvatar = "avatar";
FString FTDSUser::KeyShortID = "shortId";

TSharedPtr<FJsonObject> GetJsonObject(const TSharedPtr<FLCObject>& ObjectPtr) {
	if (!ObjectPtr.IsValid()) {
		return nullptr;
	}
#if PLATFORM_IOS || PLATFORM_ANDROID
	return ObjectPtr->GetServerData();
#else
	FString ServerDataStr = LC_MapToString(ObjectPtr->GetServerData());
	return TUJsonHelper::GetJsonObject(ServerDataStr);
#endif
}

TSharedPtr<FLCObject> MakeLCObject(const TSharedPtr<FJsonObject>& ObjectPtr) {
	if (!ObjectPtr.IsValid()) {
		return nullptr;
	}
#if PLATFORM_IOS
	LCObject *object = [LCObject object];
	[object objectFromDictionary:IOSHelper::Convert(ObjectPtr)];
	return MakeShared<FLCObject>(object);
#elif PLATFORM_ANDROID
	FString JsonStr = TUJsonHelper::GetJsonString(ObjectPtr);
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass("Lcom/tapsdk/lc/LCObject");
	auto object = JNI.CallStaticObjectMethod(ClassObject, "parseLCObject", "(Ljava/lang/String;)Lcom/tapsdk/lc/LCObject;", *JNI.ToJavaString(JsonStr));
	return MakeShared<FLCObject>(*object);
#else
	FString ClassName;
	if (!ObjectPtr->TryGetStringField("className", ClassName)) {
		return nullptr;
	}
	auto ServerData = LC_StringToMap(TUJsonHelper::GetJsonString(ObjectPtr));
	return MakeShared<FLCObject>(ClassName, ServerData);
#endif
}

TSharedPtr<FLCObject> MakeLCObject(const TSharedPtr<FJsonObject>& ObjectPtr, const FString& InClassName) {
	if (!ObjectPtr.IsValid()) {
		return nullptr;
	}
	TSharedPtr<FJsonObject> mObjectPtr = ObjectPtr;
	mObjectPtr->SetStringField("className", InClassName);
	return MakeLCObject(mObjectPtr);
}



FString FTDSUser::GetAvatar() const {
	return GetStringFromKey(KeyAvatar);
}

FString FTDSUser::GetNickName() const {
	return GetStringFromKey(KeyNickName);
}

FString FTDSUser::GetShortID() const {
	return GetStringFromKey(KeyShortID);
}

void FTDSUser::SetAvatar(const FString& InAvatar) {
	SetStringForKey(KeyAvatar, InAvatar);
}

void FTDSUser::SetNickName(const FString& InNickName) {
	SetStringForKey(KeyNickName, InNickName);
}

void FTDSUser::SetShortID(const FString& InShortID) {
	SetStringForKey(KeyShortID, InShortID);
}

#if !PLATFORM_IOS && !PLATFORM_ANDROID
void FTDSUser::SetStringForKey(const FString& InKey, const FString& InValue) {
	Set(InKey, InValue);
}

void FTDSUser::SetDoubleForKey(const FString& InKey, double InValue) {
	Set(InKey, InValue);
}

void FTDSUser::SetIntergerForKey(const FString& InKey, int64 InValue) {
	Set(InKey, InValue);
}

void FTDSUser::SetBooleanForKey(const FString& InKey, bool InValue) {
	Set(InKey, InValue);
}

FString FTDSUser::GetStringFromKey(const FString& InKey) const {
	return Get(InKey).AsString();
}

double FTDSUser::GetDoubleFromKey(const FString& InKey) const {
	return Get(InKey).AsDouble();
}

int64 FTDSUser::GetIntergerFromKey(const FString& InKey) const {
	return Get(InKey).AsInteger();
}

bool FTDSUser::GetBooleanFromKey(const FString& InKey) const {
	return Get(InKey).AsBoolean();
}

TSharedPtr<FJsonObject> FTDSUser::GetServerData() const {
	FString ServerDataStr = LC_MapToString(FLCUser::GetServerData());
	return TUJsonHelper::GetJsonObject(ServerDataStr);
}
#endif

void FTDSUser::Save(const FCallBackDelegate& CallBack) {
	TSharedPtr<FTDSUser> TempUser = nullptr;
	if (DoesSharedInstanceExist()) {
		TempUser = StaticCastSharedRef<FTDSUser>(AsShared());
	}
	else {
		TempUser = MakeShared<FTDSUser>(*this);
	}
	FLeanCloudBoolResultDelegate Delegate;
	LoginWithBoolCallBack(CallBack, Delegate, TempUser);
	TempUser->FLCUser::Save(Delegate);
}

TSharedPtr<FTDSUser> FTDSUser::GetCurrentUser() {
	return StaticCastSharedPtr<FTDSUser>(FLCUser::GetCurrentUser());
}

void FTDSUser::Logout() {
#if PLATFORM_IOS || PLATFORM_ANDROID
	TUMobileBridge::AsyncPerform(BOOTSTRAP_SERVICE,"logout","");
#else	
	FLCUser::LogOut();
	TapUELogin::Logout();
#endif
	
}

void FTDSUser::LoginAnonymously(const FCallBackDelegate& CallBack) {
	FLeanCloudUserDelegate Delegate;
	LoginWithUserCallBack(CallBack, Delegate);
#if PLATFORM_IOS || PLATFORM_ANDROID
	TUMobileBridge::AsyncPerform(BOOTSTRAP_SERVICE, "loginAnonymously","", [=](const FString& ResultStr)
	{
		const TSharedPtr<FJsonObject> JsonObject = TUJsonHelper::GetJsonObject(ResultStr);
		if (JsonObject.IsValid())
		{
			const double Code = JsonObject->GetNumberField(LOGIN_RESULT_CODE_KEY);
			if(Code == CALLBACK_SUCCESS_CODE)
			{
				const TSharedPtr<FTDSUser> CurrentUser = GetCurrentUser();
				Delegate.ExecuteIfBound(CurrentUser,FLCError());
			}else
			{
				const FString Wrapper = JsonObject->GetStringField(BRIDGE_DATA_KEY);
				const TSharedPtr<FJsonObject> DataObject = TUJsonHelper::GetJsonObject(Wrapper);
				const FString Error_description = DataObject->GetStringField("error_description");
				Delegate.ExecuteIfBound(nullptr, FLCError(FTUError::UNDEFINED, Error_description));
			}
		}else
		{
			Delegate.ExecuteIfBound(nullptr, FLCError(FTUError::BRIDGE_EXECUTE, "unknown error:invalid data"));
		}
		
	});
#else
	FLCUser::LoginAnonymously(TUDeviceInfo::GetLoginId(), Delegate);
#endif
}

void FTDSUser::LoginWithTapTap(const TArray<FString>& Permissions, const FCallBackDelegate& CallBack) {
	auto TapLoginResult = [=](const TUAuthResult& Result) {
		switch (Result.GetType()) {
		case TUAuthResult::Success: {
			const TSharedPtr<FTUAccessToken> TapAccessToken = TapUELogin::GetAccessToken();
			const TSharedPtr<FTULoginProfileModel> TapProfile = TapUELogin::GetProfile();
			ensure(TapAccessToken && TapProfile);
			TSharedPtr<FJsonObject> AuthData = MakeShareable(new FJsonObject);
			AuthData->SetStringField(TEXT("access_token"), TapAccessToken->access_token);
			AuthData->SetStringField(TEXT("kid"), TapAccessToken->kid);
			AuthData->SetStringField(TEXT("mac_key"), TapAccessToken->mac_key);
			AuthData->SetStringField(TEXT("token_type"), TapAccessToken->token_type);
			AuthData->SetStringField(TEXT("mac_algorithm"), TapAccessToken->mac_algorithm);
			AuthData->SetStringField(TEXT("openid"), TapProfile->openid);
			AuthData->SetStringField(TEXT("unionid"), TapProfile->unionid);
			AuthData->SetStringField(TEXT("name"), TapProfile->name);
			AuthData->SetStringField(TEXT("avatar"), TapProfile->avatar);
			LoginWithAuthData("taptap", AuthData, CallBack);
		}
		break;
		case TUAuthResult::Cancel:
		case TUAuthResult::Fail:
			if (TSharedPtr<FTUError> Error = Result.GetError()) {
				CallBack.ExecuteIfBound(nullptr, *Error);
			}
			else {
				CallBack.ExecuteIfBound(nullptr, FTUError(-1, TEXT("TapTap login failed.")));
			}
			break;
		default:
			CallBack.ExecuteIfBound(nullptr, FTUError(-1, TEXT("Login taptap error")));
		}
	};
	TapUELogin::Login(Permissions, TapLoginResult);
}

void FTDSUser::BecomeWithSessionToken(const FString& SessionToken, const FCallBackDelegate& CallBack) {
#if PLATFORM_IOS || PLATFORM_ANDROID
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("loginWithToken"), SessionToken);
	Writer->WriteObjectEnd();
	Writer->Close();
	TUMobileBridge::AsyncPerform(BOOTSTRAP_SERVICE, "loginWithToken",JsonOutString, [=](const FString& ResultStr)
	{
		const TSharedPtr<FJsonObject> JsonObject = TUJsonHelper::GetJsonObject(ResultStr);
		if (JsonObject.IsValid())
		{
			const double Code = JsonObject->GetNumberField(LOGIN_RESULT_CODE_KEY);
			if(Code == CALLBACK_SUCCESS_CODE)
			{
				const TSharedPtr<FTDSUser> CurrentUser = GetCurrentUser();
				CallBack.ExecuteIfBound(CurrentUser,FTUError());
			}else
			{
				const FString Wrapper = JsonObject->GetStringField(BRIDGE_DATA_KEY);
				const TSharedPtr<FJsonObject> DataObject = TUJsonHelper::GetJsonObject(Wrapper);
				const FString Error_description = DataObject->GetStringField("error_description");
				CallBack.ExecuteIfBound(nullptr, FTUError(FTUError::UNDEFINED, Error_description));
			}
		}else
		{
			CallBack.ExecuteIfBound(nullptr, FTUError(FTUError::BRIDGE_EXECUTE, "unknown error:invalid data"));
		}
		
	});
#else
	FLeanCloudUserDelegate Delegate;
	LoginWithUserCallBack(CallBack, Delegate);
	FLCUser::LoginWithSessionToken(SessionToken, Delegate);
#endif
}

void FTDSUser::LoginWithAuthData(const FString& Platform, TSharedPtr<FJsonObject> AuthData,
                                 const FCallBackDelegate& CallBack) {
	FLeanCloudUserDelegate Delegate;
	LoginWithUserCallBack(CallBack, Delegate);
#if PLATFORM_IOS || PLATFORM_ANDROID
	FString AuthDataString = TEXT("");
	if (AuthData.IsValid())
	{
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&AuthDataString);
		FJsonSerializer::Serialize(AuthData.ToSharedRef(), Writer);
	}
	
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> DataWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	DataWriter->WriteObjectStart();
	DataWriter->WriteValue(TEXT("loginWithAuthData"), Platform);
	DataWriter->WriteValue(TEXT("authData"), AuthDataString);
	DataWriter->WriteObjectEnd();
	DataWriter->Close();
	TUMobileBridge::AsyncPerform(BOOTSTRAP_SERVICE, "loginWithAuthData",JsonOutString, [=](const FString& ResultStr)
	{
		const TSharedPtr<FJsonObject> JsonObject = TUJsonHelper::GetJsonObject(ResultStr);
		if (JsonObject.IsValid())
		{
			const double Code = JsonObject->GetNumberField(LOGIN_RESULT_CODE_KEY);
			if(Code == CALLBACK_SUCCESS_CODE)
			{
				const TSharedPtr<FTDSUser> CurrentUser = GetCurrentUser();
				Delegate.ExecuteIfBound(CurrentUser,FLCError());
			}else
			{
				const FString Wrapper = JsonObject->GetStringField(BRIDGE_DATA_KEY);
				const TSharedPtr<FJsonObject> DataObject = TUJsonHelper::GetJsonObject(Wrapper);
				const FString Error_description = DataObject->GetStringField("error_description");
				Delegate.ExecuteIfBound(nullptr, FLCError(FTUError::UNDEFINED, Error_description));
			}
		}else
		{
			Delegate.ExecuteIfBound(nullptr, FLCError(FTUError::BRIDGE_EXECUTE, "unknown error:invalid data"));
		}
		
	});

#else
	FString AuthDataStr = TUJsonHelper::GetJsonString(AuthData);
	TLCMap AuthDataMap = LC_StringToMap(AuthDataStr);
	FLCUser::LoginWithAuthData(AuthDataMap, Platform, Delegate);
#endif
}

void FTDSUser::AssociateWithAuthData(const FString& Platform, TSharedPtr<FJsonObject> AuthData,
                                     const FCallBackDelegate& CallBack) {
	TSharedPtr<FTDSUser> TempUser = nullptr;
	if (DoesSharedInstanceExist()) {
		TempUser = StaticCastSharedRef<FTDSUser>(AsShared());
	}
	else {
		TempUser = MakeShared<FTDSUser>(*this);
	}
	FLeanCloudBoolResultDelegate Delegate;
	LoginWithBoolCallBack(CallBack, Delegate, TempUser);
#if PLATFORM_IOS || PLATFORM_ANDROID

	FString AuthDataString = TEXT("");
	if (AuthData.IsValid())
	{
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&AuthDataString);
		FJsonSerializer::Serialize(AuthData.ToSharedRef(), Writer);
	}
	
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> DataWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	DataWriter->WriteObjectStart();
	DataWriter->WriteValue(TEXT("associateWithAuthData"), Platform);
	DataWriter->WriteValue(TEXT("authData"), AuthDataString);
	DataWriter->WriteObjectEnd();
	DataWriter->Close();
	TUMobileBridge::AsyncPerform(BOOTSTRAP_SERVICE, "associateWithAuthData",JsonOutString, [=](const FString& ResultStr)
	{
		const TSharedPtr<FJsonObject> JsonObject = TUJsonHelper::GetJsonObject(ResultStr);
		if (JsonObject.IsValid())
		{
			const double Code = JsonObject->GetNumberField(USER_STATUS_CODE_KEY);
			if(Code == CALLBACK_SUCCESS_CODE)
			{
				Delegate.ExecuteIfBound(true,FLCError());
			}else
			{
				const FString Wrapper = JsonObject->GetStringField(BRIDGE_DATA_KEY);
				const TSharedPtr<FJsonObject> DataObject = TUJsonHelper::GetJsonObject(Wrapper);
				const FString Error_description = DataObject->GetStringField("error_description");
				Delegate.ExecuteIfBound(false, FLCError(FTUError::BIND_ERROR, Error_description));
			}
		}else
		{
			Delegate.ExecuteIfBound(false, FLCError(FTUError::BRIDGE_EXECUTE, "unknown error:invalid data"));
		}
		
	});
#else
	FString AuthDataStr = TUJsonHelper::GetJsonString(AuthData);
	TLCMap AuthDataMap = LC_StringToMap(AuthDataStr);
	TempUser->FLCUser::AssociateWithAuthData(AuthDataMap, Platform, Delegate);
#endif
}

void FTDSUser::DisassociateAuthData(const FString& Platform, const FCallBackDelegate& CallBack) {
	TSharedPtr<FTDSUser> TempUser = nullptr;
	if (DoesSharedInstanceExist()) {
		TempUser = StaticCastSharedRef<FTDSUser>(AsShared());
	}
	else {
		TempUser = MakeShared<FTDSUser>(*this);
	}
	FLeanCloudBoolResultDelegate Delegate;
	LoginWithBoolCallBack(CallBack, Delegate, TempUser);
#if PLATFORM_IOS || PLATFORM_ANDROID
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> DataWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	DataWriter->WriteObjectStart();
	DataWriter->WriteValue(TEXT("disassociateWithPlatform"), Platform);
	DataWriter->WriteObjectEnd();
	DataWriter->Close();
	TUMobileBridge::AsyncPerform(BOOTSTRAP_SERVICE, "disassociateWithPlatform",JsonOutString, [=](const FString& ResultStr)
	{
		const TSharedPtr<FJsonObject> JsonObject = TUJsonHelper::GetJsonObject(ResultStr);
		if (JsonObject.IsValid())
		{
			const double Code = JsonObject->GetNumberField(USER_STATUS_CODE_KEY);
			if(Code == CALLBACK_SUCCESS_CODE)
			{
				Delegate.ExecuteIfBound(true,FLCError());
			}else
			{
				const FString Wrapper = JsonObject->GetStringField(BRIDGE_DATA_KEY);
				const TSharedPtr<FJsonObject> DataObject = TUJsonHelper::GetJsonObject(Wrapper);
				const FString Error_description = DataObject->GetStringField("error_description");
				Delegate.ExecuteIfBound(false, FLCError(FTUError::UNDEFINED, Error_description));
			}
		}else
		{
			Delegate.ExecuteIfBound(false, FLCError(FTUError::BRIDGE_EXECUTE, "unknown error:invalid data"));
		}
		
	});
#else
	TempUser->FLCUser::DisassociateWithPlatform(Platform, Delegate);
#endif
}

void FTDSUser::RetrieveShortToken(const FStringSignature& OnSuccess, const FTUError::FDelegate& OnFailed) {
	FLCUser::RetrieveShortToken(OnSuccess, FLCError::FDelegate::CreateLambda([=](const FLCError& Error) {
		FTUError TapError;
		TapError.code = Error.Code;
		TapError.error_description = Error.Reason;
		OnFailed.ExecuteIfBound(TapError);
	}));
}


void FTDSUser::LoginWithUserCallBack(const FCallBackDelegate& CallBack, FLeanCloudUserDelegate& LCCallBack) {
	LCCallBack = FLeanCloudUserDelegate::CreateLambda([=](TSharedPtr<FLCUser> UserPtr, const FLCError& Error) {
		if (UserPtr.IsValid()) {
			CallBack.ExecuteIfBound(StaticCastSharedPtr<FTDSUser>(UserPtr), FTUError());
		}
		else {
			FTUError TapError;
			TapError.code = Error.Code;
			TapError.error_description = Error.Reason;
			CallBack.ExecuteIfBound(nullptr, TapError);
		}
	});
}

void FTDSUser::LoginWithBoolCallBack(const FCallBackDelegate& CallBack, FLeanCloudBoolResultDelegate& LCCallBack,
                                     const TSharedPtr<FTDSUser>& InUserPtr) {
	LCCallBack = FLeanCloudBoolResultDelegate::CreateLambda([=](bool bIsSuccess, const FLCError& Error) {
		if (bIsSuccess) {
			CallBack.ExecuteIfBound(InUserPtr, FTUError());
		}
		else {
			FTUError TapError;
			TapError.code = Error.Code;
			TapError.error_description = Error.Reason;
			CallBack.ExecuteIfBound(nullptr, TapError);
		}
	});
}
