#include "TUCommonMobileImpl.h"
#include "TULanguage.h"
#include "TUMobileBridge.h"

#define TAPCOMMON_SERVICE "TDSCommonService"
#define TAPCOMMON_SERVICE_CLZ "com.tds.common.wrapper.TDSCommonService"
#define TAPCOMMON_SERVICE_IMPL "com.tds.common.wrapper.TDSCommonServiceImpl"

TUCommonMobileImpl::TUCommonMobileImpl() {
	TUMobileBridge::Register(TAPCOMMON_SERVICE_CLZ, TAPCOMMON_SERVICE_IMPL);
}

void TUCommonMobileImpl::SetLanguage(ELanguageType LanguageType) {
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("preferredLanguage"), (int)LanguageType);
	Writer->WriteObjectEnd();
	Writer->Close();
	TUMobileBridge::AsyncPerform(TAPCOMMON_SERVICE, "setPreferredLanguage", JsonOutString);
	TULanguage::SetCurrentType(LanguageType);
}

void TUCommonMobileImpl::setDurationStatisticsEnabled(bool bEnable) {
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("setDurationStatisticsEnabled"), bEnable);
	Writer->WriteObjectEnd();
	Writer->Close();
	TUMobileBridge::AsyncPerform(TAPCOMMON_SERVICE, "setDurationStatisticsEnabled", JsonOutString);
}

void TUCommonMobileImpl::SetXUA() {
	FString EngineJsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> EngineInfoWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&EngineJsonOutString);
	EngineInfoWriter->WriteObjectStart();
	EngineInfoWriter->WriteValue(TEXT("Engine-Platform"), TEXT("UE"));
	EngineInfoWriter->WriteValue(TEXT("Engine-Version"), TapCommon_UE_VERSION);
	EngineInfoWriter->WriteObjectEnd();
	EngineInfoWriter->Close();

	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonOutString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("setXUA"), EngineJsonOutString);
	Writer->WriteObjectEnd();
	Writer->Close();
	TUMobileBridge::AsyncPerform(TAPCOMMON_SERVICE, "setXUA", JsonOutString);
}
