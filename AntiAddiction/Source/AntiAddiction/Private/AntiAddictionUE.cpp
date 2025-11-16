#include "AntiAddictionUE.h"

#include "TUDebuger.h"
#include "Server/AAUImpl.h"

#include "TUType.h"


AntiAddictionUE::FCallBack AntiAddictionUE::OnCallBack;


void AntiAddictionUE::SetTestEnvironment(bool Enable)
{
	AAUImpl::Get()->SetTestEnv(Enable);
}

void AntiAddictionUE::Init(const FAAUConfig& Config) {
	AAUImpl::Get()->Init(Config);
}

void AntiAddictionUE::Startup(const FString& UserID, bool bIsTapUser) {
	if (UserID.IsEmpty()) {
		TUDebuger::ErrorLog("AntiAddiction UserID is Empty");
		return;
	}
	AAUImpl::Get()->StartupWithTapTap(UserID);
}

void AntiAddictionUE::StartupWithTapTap(const FString& UserID)
{
	if (UserID.IsEmpty())
	{
		UE_LOG(LogTap, Warning, TEXT("AntiAddiction UserID is Empty"));
		return;
	}
	AAUImpl::Get()->StartupWithTapTap(UserID);
}

void AntiAddictionUE::Exit() {
	AAUImpl::Get()->Exit();
}

EAAUAgeLimit AntiAddictionUE::GetAgeRange() {
	return AAUImpl::Get()->GetAgeRange();
}

int AntiAddictionUE::GetRemainingTimeInMinutes() {
	double Remaintime = GetRemainingTime();
	return FMath::CeilToDouble(Remaintime / 60);
}

int AntiAddictionUE::GetRemainingTime() {
	return AAUImpl::Get()->GetRemainingTime();
}

void AntiAddictionUE::EnterGame() {
	// AAUImpl::Get()->EnterGame();
}

void AntiAddictionUE::LeaveGame() {
	// AAUImpl::Get()->LeaveGame();
}

void AntiAddictionUE::CheckPayLimit(int Amount,TFunction<void(bool Status)> CallBack,TFunction<void(const FString& Msg)> FailureHandler) {
	AAUImpl::Get()->CheckPayLimit(Amount, CallBack, FailureHandler);
}

void AntiAddictionUE::SubmitPayResult(int Amount, TFunction<void(bool Success)> CallBack,
	TFunction<void(const FString& Msg)> FailureHandler) {
	AAUImpl::Get()->SubmitPayResult(Amount, CallBack, FailureHandler);
}

FString AntiAddictionUE::CurrentToken() {
	return AAUImpl::Get()->CurrentToken();
}

