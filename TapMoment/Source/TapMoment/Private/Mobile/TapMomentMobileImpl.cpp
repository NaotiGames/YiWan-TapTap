#include "TapMomentMobileImpl.h"

#include "TUJsonHelper.h"
#include "TUMobileBridge.h"

#include "Policies/CondensedJsonPrintPolicy.h"

#define TAP_MOMENT_CLZ "com.tapsdk.moment.wrapper.TapMomentService"
#define TAP_MOMENT_IMPL "com.tapsdk.moment.wrapper.TapMomentServiceImpl"
#define TAP_MOMENT_SERVICE "TapMomentService"

FTapMomentMobileImpl::FTapMomentMobileImpl() {
	TUMobileBridge::Register(TEXT(TAP_MOMENT_CLZ),TEXT(TAP_MOMENT_IMPL));
}

void FTapMomentMobileImpl::Init(const FTapMomentConfig& InConfig) {
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("clientId"), InConfig.ClientID);
	Writer->WriteValue(TEXT("regionType"), InConfig.RegionType == ERegionType::CN);
	Writer->WriteObjectEnd();
	Writer->Close();
	TUMobileBridge::AsyncPerform(TAP_MOMENT_SERVICE, TEXT("initWithRegion"), JsonOutString);
}

void FTapMomentMobileImpl::SetCallback(FTapMoment::FDelegate CallBack) {
	TUMobileBridge::AsyncPerform(TAP_MOMENT_SERVICE, TEXT("setCallback"), TEXT(""), [CallBack](const FString& ResultJsonStr) {
		int Code = -1;
		FString Msg = "";
		auto ResultPtr = TUJsonHelper::GetJsonObject(ResultJsonStr);
		if (ResultPtr.IsValid()) {
			ResultPtr->TryGetNumberField(TEXT("code"), Code);
			ResultPtr->TryGetStringField(TEXT("message"), Msg);
		}
		CallBack.ExecuteIfBound(Code, Msg);
	}, false);
}

void FTapMomentMobileImpl::Open(ETapMomentOrientation Orientation) {
	FString args;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&args);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("config"), (int)Orientation);
	Writer->WriteObjectEnd();
	Writer->Close();
	TUMobileBridge::AsyncPerform(TAP_MOMENT_SERVICE, TEXT("open"), args);
}

void FTapMomentMobileImpl::Publish(ETapMomentOrientation Orientation, const TArray<FString>& ImagePaths,
	const FString& Content) {
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("imagePaths"), ImagePaths);
	Writer->WriteValue(TEXT("config"), (int)Orientation);
	Writer->WriteValue(TEXT("content"), Content);
	Writer->WriteObjectEnd();
	Writer->Close();
	TUMobileBridge::AsyncPerform(TAP_MOMENT_SERVICE, TEXT("publish"), JsonOutString);
}

void FTapMomentMobileImpl::PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths,
	const TArray<FString>& ImagePaths, const FString& Title, const FString& Desc) {
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("imagePaths"), ImagePaths);
	Writer->WriteValue(TEXT("videoPaths"), VideoPaths);
	Writer->WriteValue(TEXT("config"), (int)Orientation);
	Writer->WriteValue(TEXT("title"), Title);
	Writer->WriteValue(TEXT("desc"), Desc);
	Writer->WriteObjectEnd();
	Writer->Close();
	TUMobileBridge::AsyncPerform(TAP_MOMENT_SERVICE, TEXT("publishVideoImage"), JsonOutString);
}

void FTapMomentMobileImpl::PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths,
	const FString& Title, const FString& Desc) {
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("videoPaths"), VideoPaths);
	Writer->WriteValue(TEXT("config"), (int)Orientation);
	Writer->WriteValue(TEXT("title"), Title);
	Writer->WriteValue(TEXT("desc"), Desc);
	Writer->WriteObjectEnd();
	Writer->Close();
	TUMobileBridge::AsyncPerform(TAP_MOMENT_SERVICE, TEXT("publishVideo"), JsonOutString);
}

void FTapMomentMobileImpl::FetchNotification() {
	TUMobileBridge::AsyncPerform(TAP_MOMENT_SERVICE, TEXT("fetchNotification"), TEXT(""));
}

void FTapMomentMobileImpl::Close() {
	TUMobileBridge::AsyncPerform(TAP_MOMENT_SERVICE, TEXT("close"), TEXT(""));
}

void FTapMomentMobileImpl::Close(const FString& Title, const FString& Content) {
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("title"), Title);
	Writer->WriteValue(TEXT("content"), Content);
	Writer->WriteObjectEnd();
	Writer->Close();
	TUMobileBridge::AsyncPerform(TAP_MOMENT_SERVICE, TEXT("closeWithConfirmWindow"), JsonOutString);
}

// void FTapMomentMobileImpl::SetUseAutoRotate(bool UseAuto) {
// 	FTapMomentImpl::SetUseAutoRotate(UseAuto);
// }

void FTapMomentMobileImpl::DirectlyOpen(ETapMomentOrientation Orientation, const FString& Page,
	TSharedPtr<FJsonObject> Extras) {
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("config"), (int)Orientation);
	Writer->WriteValue(TEXT("page"), Page);
	Writer->WriteValue(TEXT("extras"), TUJsonHelper::GetJsonString(Extras));
	Writer->WriteObjectEnd();
	Writer->Close();
	TUMobileBridge::AsyncPerform(TAP_MOMENT_SERVICE, TEXT("directlyOpen"), JsonOutString);
}

void FTapMomentMobileImpl::NeedDeferSystemGestures() {
#if PLATFORM_IOS
	TUMobileBridge::AsyncPerform(TAP_MOMENT_SERVICE, TEXT("needDeferSystemGestures"), TEXT(""));
#else
	FTapMomentImpl::NeedDeferSystemGestures();
#endif
}
