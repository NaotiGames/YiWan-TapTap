#pragma once
#include "AAUChinaSever.h"
#include "Model/China/AAURealNameConfigModel.h"
#include "Server/AAUImpl.h"
#include "Server/AAUServerDelegate.h"
#include "AntiAddictionUE.h"


struct FAAUserConfigModel;
struct FTUAccessToken;
struct FAntiAddictionError;
class SVerifyEntryWidget;
class SManualVerifyWidget;
struct FAAURealNameResultModel;
struct FAntiAddictionError;



// class AAUChinaSever;
class AAUChinaImpl: public AAUImpl, AAUServerDelegate {
public:
	AAUChinaImpl();
	virtual ~AAUChinaImpl() override;
	virtual void StartupWithTapTap(const FString& UserId) override;
	virtual void SetTestEnv(bool Enable) override;
	virtual void Exit() override;
	virtual EAAUAgeLimit GetAgeRange() override;
	virtual int GetRemainingTime() override;
	virtual void KickOut(AAUTimeBoundary Boundary, const FString& Title, const FString& Content) override;
	virtual void InitImpl(const FAAUConfig& _Config) override;
	virtual void EnterGame() override;
	virtual void LeaveGame() override;
	virtual void CheckPayLimit(int Amount, TFunction<void(bool Status)> CallBack, TFunction<void(const FString& Msg)> FailureHandler) override;
	virtual void SubmitPayResult(int Amount, TFunction<void(bool Success)> CallBack, TFunction<void(const FString& Msg)> FailureHandler) override;
	virtual FString CurrentToken() override;
	
	void TryAgainStartUp(const FString& ErrMsg);
	
	void Login(const FString& AccessToken,  bool IsFirst = false, bool bTapUser = false);

	void ShowRealNameUI(AAURealNameWordType Type, bool bFallback = true);

	void ShowHealthTipUI(const FString& Title, const FString& Content, bool NeedStrict);

	void GotoTapTapQuickVerify();
	static bool HasComplianceInTapToken(const TSharedPtr<FTUAccessToken> TapToken, bool useAgeRange);


	FString CurrentUserID;
	 bool UseAgeRange;
	
protected:
	void StartupQuickVerifyTapAuth();

	void GoToRealName();
	
	void HandleQuickVerifyTdsResult(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error);

	void FetchRealNameConfig(const FString& UserId,TFunction<void(TSharedPtr<FAAURealNameConfigModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);
	void FetchUserConfig(const FString& UserId, const FString& Token, TFunction<void(TSharedPtr<FAAUserConfigModel> ModelPtr, const FAntiAddictionError& Error)> CallBack);
	void QuickVerify(const TSharedRef<FTUAccessToken>& TapToken);

	TWeakPtr<SVerifyEntryWidget> EntryWidget;
	TWeakPtr<SManualVerifyWidget> ManualVerifyWidget;
	TSharedPtr<AAUChinaSever> Server;

	bool bHasTapTokenButNoCompliance = false;

	FString CurrentSession;
	// 检查 startUp 接口是否正在处理中
	bool IsCheckingUser = false;
	AntiAddictionUE::FCallBack InternalCallback;
};
