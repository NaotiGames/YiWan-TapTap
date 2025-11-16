#include "AAUChinaSever.h"

#include "AntiAddictionUE.h"
#include "TUDataStorage.h"
#include "TUDebuger.h"
#include "TUSettings.h"
#include "Model/AAUPlayableModel.h"
#include "Model/China/AAUserConfigModel.h"
#include "Model/China/AntiAddictionError.h"
#include "Server/AAUHelper.h"
#include "Server/AAUImpl.h"
#include "Server/AAUNet.h"
#include "Server/AAUStorage.h"



void AAUChinaSever::Login(const FAAUUser& User, bool useAgeRange, const FString& session,TFunction<void(TSharedPtr<AAULoginResult> Result, const FAntiAddictionError& Error)> CallBack) {
	if (CallBack == nullptr) {
		TUDebuger::ErrorLog("AAUChinaSever::Login CallBack can not be nullptr");
		return;
	}
	ResetValues();
	UseAgeRange = useAgeRange;
	SetCurrentUser(User, session);

	AAUNet::CheckPlayable(User.UserID, User.AccessTokenV2, session, [=](TSharedPtr<FAAUPlayableModel> ModelPtr,  const FAntiAddictionError& Error) {
		if(!ModelPtr.IsValid())
		{
			if(Error.httpState == TUHttpResponse::networkError || Error.httpState == TUHttpResponse::serverError)
			{
				if(FAAUserConfigModel::CurrentModel->anti_addiction.policy_active.Equals("time_range"))
				{
					//使用本地实名
					ModelPtr = CheckPlayableLocal();
				}
			}
		}
		if (ModelPtr.IsValid()) {
			AAULoginResult Result;
			Result.Title = ModelPtr->title;
			Result.Description = ModelPtr->description_plain;
			Result.RemainTime = ModelPtr->remain_time;
			this->RemainTime = ModelPtr->remain_time;
			// this->CostTime = ModelPtr->cost_time;
			
			if (User.AgeLimit == EAAUAgeLimit::Adult || User.AgeLimit == EAAUAgeLimit::UnknownAgeRangeAdult) {
				Result.LoginState = AAULoginResult::SuccessWithNoLimit;
				CallBack(MakeShared<AAULoginResult>(Result),  Error);
			} else {
				if (ModelPtr->remain_time > 0) {
					Result.LoginState = AAULoginResult::SuccessWithLimit;
					CallBack(MakeShared<AAULoginResult>(Result),Error);
				}
				else {
					Result.LoginState = AAULoginResult::FailWithCurfew;
					CallBack(MakeShared<AAULoginResult>(Result),Error);
				}
			}
		}
		else {
			CallBack(nullptr,Error);
		}
	});
		

	
}

void AAUChinaSever::EnterGame() {
	if (!CurrentUser.IsValid()) {
		return;
	}
	if (IsInGame == true) {
		return;
	}
	IsInGame = true;
	StartCheckTimer();
}

void AAUChinaSever::LeaveGame() {
	if (!CurrentUser.IsValid()) {
		return;
	}
	if (IsInGame == false) {
		return;
	}
	IsInGame = false;
	StopTimer();
	
}

void AAUChinaSever::Logout() {
	if (!CurrentUser.IsValid()) {
		return;
	}
	TUDataStorage<FAAUStorage>::Remove(FAAUStorage::UserConfig + CurrentUser.Get()->UserID);
	TUDataStorage<FAAUStorage>::Remove(FAAUStorage::HasLoginedUser +  CurrentUser.Get()->UserID);
	LeaveGame();
	CurrentUser = nullptr;
	FAAUserConfigModel::ResetModel();
}

void AAUChinaSever::KickOut(AAUTimeBoundary Boundary, const FString& Title, const FString& Content, bool IsServer) {
	IsInGame = false;
	StopTimer();
	CurrentUser = nullptr;
	if (Delegate) {
		Delegate->KickOut(Boundary, Title, Content);
	}
}

void AAUChinaSever::ResetValues() {
	IsInGame = false;
	PlayableServerCrashCount = 0;
	RemainTime = 0;
	StopTimer();
	CurrentUser = nullptr;
}

int AAUChinaSever::GetRemainingTime() {
	if (!CurrentUser.IsValid()) {
		return 0;
	}
	if (CurrentUser->AgeLimit == EAAUAgeLimit::Adult || CurrentUser->AgeLimit == EAAUAgeLimit::UnknownAgeRangeAdult) {
		return AAUImpl::AdultRemainTime;;
	}
	return RemainTime;
}

EAAUAgeLimit AAUChinaSever::GetAgeRange() {
	if (!CurrentUser.IsValid()) {
		return EAAUAgeLimit::Unknown;
	}
	if(!UseAgeRange) return EAAUAgeLimit::Unknown;
	if(CurrentUser->AgeLimit < EAAUAgeLimit::UnknownAgeRange)
	{
		return CurrentUser->AgeLimit;
	}else
	{
		return EAAUAgeLimit::Unknown;
	}
	
}

FString AAUChinaSever::CurrentToken() {
	if (!CurrentUser.IsValid()) {
		return "";
	}
	return CurrentUser->AccessTokenV2;
}

void AAUChinaSever::CheckPayLimit(int Amount,
	TFunction<void(bool Status, const FString& Title, const FString& Description)> CallBack,
	TFunction<void(const FAntiAddictionError& Error)> FailureHandler) {
	if (!CurrentUser.IsValid()) {
		if (FailureHandler) {
			FAntiAddictionError AntiError = FAntiAddictionError();
			AntiError.msg = "还未登录，请先登录";
			FailureHandler(AntiError);
		}
		return;
	}
	AAUNet::CheckPayment(Amount, *CurrentUser.Get(), [=](TSharedPtr<FAAUPayableModel> ModelPtr, const FAntiAddictionError& Error) {
		if (ModelPtr.IsValid()) {
			if (CallBack) {
				CallBack(ModelPtr->allow, ModelPtr->title, ModelPtr->description_plain);
			}
		}
		else {
			if (FailureHandler) {
				FailureHandler(Error);
			}
		}
	});
}

void AAUChinaSever::SubmitPayResult(int Amount, TFunction<void(bool Success)> CallBack,
	TFunction<void(const FAntiAddictionError& Error)> FailureHandler) {
	if (!CurrentUser.IsValid()) {
		if (FailureHandler) {
			FAntiAddictionError AntiError = FAntiAddictionError();
			AntiError.msg = "还未登录，请先登录";
			FailureHandler(AntiError);
		}
		return;
	}
	AAUNet::SetPayment(Amount, *CurrentUser.Get(), [=](TSharedPtr<FAAUPaymentModel> ModelPtr, const FAntiAddictionError& Error) {
		if (ModelPtr.IsValid()) {
			if (CallBack) {
				CallBack(true);
			}
		}
		else {
			if (FailureHandler) {
				FailureHandler(Error);
			}
		}
	});
}

FDateTime AAUChinaSever::GetCurrentTime() {
	const int64 LastServerTime = AAUNet::GetServerTimeByLastRequest();
	if (LastServerTime > 0)
	{
		return FDateTime::FromUnixTimestamp(LastServerTime) + 8 * ETimespan::TicksPerHour; // 东八区;
	}
	return AAUHelper::GetChinaCurrentTime() + FTimespan(TimeSpan * ETimespan::TicksPerSecond);
}

int64 AAUChinaSever::CalculateRemainTime(AAUTimeBoundary& Boundary) {
	// 确定是成年人不用计算时长
	if (CurrentUser.IsValid() && (CurrentUser->AgeLimit == EAAUAgeLimit::Adult || CurrentUser->AgeLimit == EAAUAgeLimit::UnknownAgeRangeAdult)) {
		Boundary = AAUTimeBoundaryNoLimit;
		return AAUImpl::AdultRemainTime;
	}
	if(!CurrentUser.IsValid())
	{
		return 0;
	}
	const bool IsHoliday = AAUHelper::IsHoliday(GetCurrentTime(), CurrentUser->UserID);
	if (!IsHoliday) {
		Boundary = AAUTimeBoundaryCurfew;
		return 0;
	}
	// 宵禁时间。
	const auto IntervalForCurfew = AAUHelper::IntervalForCurfew(GetCurrentTime());
	if (IntervalForCurfew <= 0) {
		Boundary = AAUTimeBoundaryCurfew;
		return 0;
	}else
	{
		Boundary = AAUTimeBoundaryNoLimit;
		return IntervalForCurfew;
	}
	
}


void AAUChinaSever::SetCurrentUser(const FAAUUser& User,const FString& session)
{
	CurrentUser = MakeShareable(new FAAUUser(User));
	CurrentSession = session;
}


void AAUChinaSever::StartCheckTimer() {
	if (!CurrentUser.IsValid()) {
		return;
	}
	TUSettings::GetGameInstance()->GetTimerManager().SetTimer(CheckTimer, [=]() {
		if (!CurrentUser.IsValid()) {
			return;
		}
		CheckPlayable();
		
	}, FAAUserConfigModel::CurrentModel.Get()->anti_addiction.policy_heartbeat_interval, true,0);
}

void AAUChinaSever::StartCountDownTimer() {
	if (!CurrentUser.IsValid()) {
		return;
	}
	TUSettings::GetGameInstance()->GetTimerManager().SetTimer(CountDownTimer, [=]() {
		if (!CurrentUser.IsValid()) {
			return;
		}
		CheckPlayable();
	}, FMath::Max<int>(1, RemainTime), false);
}

void AAUChinaSever::StopTimer() {
	if (CheckTimer.IsValid()) {
		TUSettings::GetGameInstance()->GetTimerManager().ClearTimer(CheckTimer);
	}
	CheckTimer.Invalidate();

	if (CountDownTimer.IsValid()) {
		TUSettings::GetGameInstance()->GetTimerManager().ClearTimer(CountDownTimer);
	}
	CountDownTimer.Invalidate();
}

void AAUChinaSever::CheckPlayable() {
	if (!CurrentUser.IsValid() || CurrentSession.IsEmpty()) {
		return;
	}
	static bool IsUploading = false;
	if (IsUploading) {
		return;
	}
	IsUploading = true;
	
	
	AAUNet::CheckPlayable(CurrentUser->UserID, CurrentUser->AccessTokenV2, CurrentSession, [=](TSharedPtr<FAAUPlayableModel> ModelPtr,  const FAntiAddictionError& Error)
	{
		IsUploading = false;
		if (!CurrentUser.IsValid()) {
			return;
		}
		if(!ModelPtr.IsValid())
		{
			if(Error.httpState == TUHttpResponse::networkError || Error.httpState == TUHttpResponse::serverError)
			{
				if(FAAUserConfigModel::CurrentModel->anti_addiction.policy_active.Equals("time_range"))
				{
					//使用本地实名
					ModelPtr = CheckPlayableLocal();
				}
			}
		}
		if (ModelPtr.IsValid())
		{
			PlayableServerCrashCount = 0;
			this->RemainTime = ModelPtr->remain_time;
			if (CurrentUser->AgeLimit != EAAUAgeLimit::Adult) {
				// 踢出
				if (RemainTime <= 0) {
					KickOut(AAUTimeBoundaryCurfew, ModelPtr->title, ModelPtr->description_plain, true);
				}
				else if (this->RemainTime <= FAAUserConfigModel::CurrentModel.Get()->anti_addiction.policy_heartbeat_interval) {
					//先关闭主 Timer 再开启倒计时
					if (CheckTimer.IsValid()) {
						TUSettings::GetGameInstance()->GetTimerManager().ClearTimer(CheckTimer);
					}
					CheckTimer.Invalidate();
					
					StartCountDownTimer();
				}
			}
		}
		else {
			if (Error.IsTokenExpired()) {
				KickOut(AAUTokenExpired, TEXT("健康提醒"), Error.msg, true);
				return;
			}
			KickOut(AAUInvalidClientOrNetworkError, ModelPtr->title, ModelPtr->description_plain, true);
		}
	});
}

TSharedPtr<FAAUPlayableModel> AAUChinaSever::CheckPlayableLocal()
{
	AAUTimeBoundary Boundary;
	RemainTime = CalculateRemainTime(Boundary);
	FAAUPlayableModel* PlayableModel = new FAAUPlayableModel();
	PlayableModel->remain_time = RemainTime;
	auto tip = FAAUserConfigModel::CurrentModel.Get()->local.time_range.text;
	if(RemainTime > 0)
	{
		const FString Minutes = FString::Printf(TEXT("%lld"), static_cast<int64> (RemainTime));
		PlayableModel->title = tip.allow.title;
		PlayableModel->description_plain = tip.allow.description_plain.Replace(TEXT("# ${remaining} #"), *Minutes);
	}else
	{
		PlayableModel->title = tip.reject.title;
		PlayableModel->description_plain = tip.reject.description_plain;
	}
	return MakeShareable(PlayableModel);
}



bool AAUChinaSever::AccessTokenIsInvaild(const FString& AccessToken) {
	return AccessToken.IsEmpty();
	
}
