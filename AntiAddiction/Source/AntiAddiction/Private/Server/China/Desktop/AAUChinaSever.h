#pragma once
#include "Model/AAULoginResult.h"
#include "Model/AAUUser.h"
#include "Model/China/AntiAddictionError.h"
#include "Server/AAUServerDelegate.h"
#include "Model/AAUPlayableModel.h"

class AAUChinaSever {
public:
	AAUServerDelegate *Delegate;
	
	void Login(const FAAUUser& User, bool useAgeRange, const FString& Session, TFunction<void(TSharedPtr<AAULoginResult> Result, const FAntiAddictionError& Error)> CallBack);

	void EnterGame();

	void LeaveGame();

	void Logout();

	int GetRemainingTime();

	EAAUAgeLimit GetAgeRange();
	
	FString CurrentToken();
	
	void CheckPayLimit(int Amount,
	TFunction<void(bool Status, const FString& Title, const FString& Description)> CallBack,
	TFunction<void(const FAntiAddictionError& Error)> FailureHandler);

	void SubmitPayResult(int Amount,
		TFunction<void(bool Success)> CallBack,
		TFunction<void(const FAntiAddictionError& Error)> FailureHandler);

	

private:
	bool IsInGame = false;
	int PlayableServerCrashCount = 0;
	
	int64 TimeSpan = 0;   // 本地时间与服务器时间的差值
	double RemainTime = 0; // 剩余时间

	FTimerHandle CheckTimer;
	FTimerHandle CountDownTimer;

	TSharedPtr<FAAUUser> CurrentUser;
	FString CurrentSession;
	bool UseAgeRange = true;
	
	void ResetValues();

	FDateTime GetCurrentTime();
	int64 CalculateRemainTime(AAUTimeBoundary& Boundary);

	void LoginStandAlone(const FAAUUser& User, TFunction<void(const AAULoginResult& Result)> CallBack);
	
	void SetCurrentUser(const FAAUUser& User, const FString& session);

	void StartCheckTimer();
	void StartCountDownTimer();
	void StopTimer();

	void CheckPlayable();
	TSharedPtr<FAAUPlayableModel> CheckPlayableLocal();


	bool AccessTokenIsInvaild(const FString& AccessToken);

	void KickOut(AAUTimeBoundary Boundary, const FString& Title, const FString& Content, bool IsServer);

	
};


