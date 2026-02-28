#include "AAUImpl.h"
#include "AAUNet.h"
#include "AntiAddictionUE.h"
#include "TUDebuger.h"
#if PLATFORM_ANDROID
#include "China/Android/AAUChinaAndroidImpl.h"
#elif  PLATFORM_IOS
#include "China/iOS/AAUChinaIOSImpl.h"
#elif PLATFORM_MAC || PLATFORM_WINDOWS
#include "China/Desktop/AAUChinaImpl.h"
#endif

TSharedPtr<AAUImpl> AAUImpl::Instance = nullptr;
FAAUConfig AAUImpl::Config;
bool AAUImpl::bTestEnvEnable = false;
int AAUImpl::AdultRemainTime = 9999;

TSharedPtr<AAUImpl>& AAUImpl::Get() {
	if (!Instance.IsValid()) {
		Instance = MakeShareable(new AAUImpl);
	}
	return Instance;
}

void AAUImpl::Init(const FAAUConfig& _Config) {
	Config = _Config;
#if PLATFORM_ANDROID
		Instance = MakeShareable(new AAUChinaAndroidImpl);
#elif  PLATFORM_IOS
		Instance = MakeShareable(new AAUChinaIOSImpl);
#elif PLATFORM_MAC || PLATFORM_WINDOWS
		Instance = MakeShareable(new AAUChinaImpl);
#endif
		Instance->InitImpl(_Config);
		Instance->SetTestEnv(bTestEnvEnable);
}

void AAUImpl::SetTestEnv(bool Enable)
{
	bTestEnvEnable = Enable;
}

void AAUImpl::InitImpl(const FAAUConfig& _Config) {
	TUDebuger::ErrorLog(TEXT("Unsupported"));
}

void AAUImpl::StartupWithTapTap(const FString& UserId)
{
	TUDebuger::ErrorLog(TEXT("Unsupported"));
}

void AAUImpl::Exit() {
	TUDebuger::ErrorLog(TEXT("Unsupported"));
}

EAAUAgeLimit AAUImpl::GetAgeRange() {
	TUDebuger::ErrorLog(TEXT("Unsupported"));
	return EAAUAgeLimit::Unknown;
}

int AAUImpl::GetRemainingTime() {
	TUDebuger::ErrorLog(TEXT("Unsupported"));
	return 0;
}

void AAUImpl::EnterGame() {
	TUDebuger::ErrorLog(TEXT("Unsupported"));
}

void AAUImpl::LeaveGame() {
	TUDebuger::ErrorLog(TEXT("Unsupported"));
}

void AAUImpl::CheckPayLimit(int Amount, TFunction<void(bool Status)> CallBack, TFunction<void(const FString& Msg)> FailureHandler) {
	TUDebuger::ErrorLog(TEXT("Unsupported"));
}

void AAUImpl::SubmitPayResult(int Amount, TFunction<void(bool Success)> CallBack,
	TFunction<void(const FString& Msg)> FailureHandler) {
	TUDebuger::ErrorLog(TEXT("Unsupported"));
}

FString AAUImpl::CurrentToken() {
	TUDebuger::ErrorLog(TEXT("Unsupported"));
	return "";
}



