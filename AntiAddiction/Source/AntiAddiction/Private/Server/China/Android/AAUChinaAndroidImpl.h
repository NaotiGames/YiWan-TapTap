#pragma once
#include "Server/AAUImpl.h"

class AAUChinaAndroidImpl: public AAUImpl {
public:
	AAUChinaAndroidImpl();
	virtual ~AAUChinaAndroidImpl() override;
	virtual void StartupWithTapTap(const FString& UserId) override;
	virtual void SetTestEnv(bool Enable) override;
	virtual void Exit() override;
	virtual EAAUAgeLimit GetAgeRange() override;
	virtual int GetRemainingTime() override;
	virtual void InitImpl(const FAAUConfig& _Config) override;
	virtual void EnterGame() override;
	virtual void LeaveGame() override;
	virtual void CheckPayLimit(int Amount, TFunction<void(bool Status)> CallBack,
		TFunction<void(const FString& Msg)> FailureHandler) override;
	virtual void SubmitPayResult(int Amount, TFunction<void(bool Success)> CallBack,
		TFunction<void(const FString& Msg)> FailureHandler) override;
	virtual FString CurrentToken() override;
	
	static TFunction<void(bool Status)> OnCheckPayLimitSuccess;
	static TFunction<void(bool Status)> OnSubmitPayResultSuccess;
	static TFunction<void(const FString& Msg)> OnCheckPayLimitFail;
	static TFunction<void(const FString& Msg)> OnSubmitPayResultFail;
};
