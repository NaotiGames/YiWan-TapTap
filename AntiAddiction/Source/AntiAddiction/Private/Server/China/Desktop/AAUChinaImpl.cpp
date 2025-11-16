#include "AAUChinaImpl.h"

#include "AAUChinaRealName.h"
#include "AntiAddictionUE.h"
#include "TapCommon.h"
#include "TapSubsystem.h"
#include "TUSettings.h"
#include "Model/AAUUser.h"
#include "Server/AAUNet.h"
#include "Server/AAUStorage.h"
#include "Slate/China/ManualVerifyWidget.h"
#include "Slate/Widgets/TapMessageBox.h"
#include "Slate/Widgets/TapToast.h"
#include "Slate/China/VerifyEntryWidget.h"
#include "TapUELogin.h"
#include "TUHelper.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Desktop/Slate/TapLoginWidget.h"
#include "Model/China/AAURealNameConfigModel.h"
#include "Model/China/AAUserConfigModel.h"
#include "Model/China/AntiAddictionError.h"

#define LOCTEXT_NAMESPACE "TapTap"

AAUChinaImpl::AAUChinaImpl()
{
	Server = MakeShareable(new AAUChinaSever);
	Server->Delegate = this;
}

AAUChinaImpl::~AAUChinaImpl()
{
}

void AAUChinaImpl::InitImpl(const FAAUConfig& _Config)
{
	UseAgeRange = _Config.UseAgeRange;
	if(_Config.UseAgeRange)
	{
		TapUELogin::AppendPermission(TEXT("compliance"));
	}else
	{
		TapUELogin::AppendPermission(TEXT("compliance_basic"));
	}

	InternalCallback.BindLambda([this](AntiAddictionUE::ResultHandlerCode Code, const FString& Message)
	{
		if ( Code == AntiAddictionUE::LoginSuccess
			|| Code == AntiAddictionUE::Exited
			|| Code == AntiAddictionUE::SwitchAccount
			|| Code == AntiAddictionUE::RealNameStop
			|| Code == AntiAddictionUE::AgeRestrict
			|| Code == AntiAddictionUE::InvalidClientOrNetworkError)
		{
			IsCheckingUser  = false;
		}
		AntiAddictionUE::OnCallBack.ExecuteIfBound(Code, Message);
	});
	
}

void AAUChinaImpl::StartupWithTapTap(const FString& UserId)
{
	if (IsCheckingUser)
	{
		UE_LOG(LogTap, Log, TEXT("AntiAddiction startUp is checking ,so just return"));
		return;
	}
	IsCheckingUser = true;
	CurrentUserID = UserId;
	if (TSharedPtr<SManualVerifyWidget> Widget = ManualVerifyWidget.Pin())
	{
		UTapSubsystem::RemoveWidget(Widget.ToSharedRef());
	}
	if (TSharedPtr<SVerifyEntryWidget> Widget = EntryWidget.Pin())
	{
		UTapSubsystem::RemoveWidget(Widget.ToSharedRef());
	}
	//先获取 realName Config

	FetchRealNameConfig(UserId, [this, UserId](TSharedPtr<FAAURealNameConfigModel> ConfigModelPtr, const FAntiAddictionError& ConfigError)
	{
		if(ConfigModelPtr.IsValid())
		{
			FTapCommonModule::TapThrobberShowWait();
			auto ResultBlock = [this, UserId](TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)
			{
				FTapCommonModule::TapThrobberDismiss();
				if (ModelPtr.IsValid())
				{
					switch (ModelPtr->GetAuthState())
					{
					case FAAURealNameResultModel::Success:
						Login(ModelPtr->anti_addiction_token);
						break;
					case FAAURealNameResultModel::Verifying:
						ShowRealNameUI(AAURealNameWordTypeVerifying);
						break;
					case FAAURealNameResultModel::Fail:
						GoToRealName();
						break;
					}
				}
				else
				{
					GoToRealName();
				}
			};

			AAUChinaRealName::CheckRealNameState(UserId, UseAgeRange,ResultBlock);
		}else
		{
			InternalCallback.ExecuteIfBound(AntiAddictionUE::InvalidClientOrNetworkError, "failed");
		}
	});
	
}
void AAUChinaImpl::FetchRealNameConfig(const FString& UserId,TFunction<void(TSharedPtr<FAAURealNameConfigModel> ModelPtr, const FAntiAddictionError& Error)> CallBack){
	AAUNet::GetSDKConfig(UserId ,[=](TSharedPtr<FAAURealNameConfigModel> ModelPtr, const FAntiAddictionError& Error)
	{
		if (ModelPtr.IsValid())
		{
			FAAURealNameConfigModel::SaveToLocal(ModelPtr);
		}
		else
		{
			if(Error.httpState == TUHttpResponse::networkError || Error.httpState == TUHttpResponse::serverError)
			{
				FAAURealNameConfigModel::GetLocalModel();
			}else
			{
				FAAURealNameConfigModel::ResetModel();
			}
		}
		if(CallBack)
		{
			CallBack(FAAURealNameConfigModel::CurrentModel,Error);
		}
	});
}

void AAUChinaImpl::GoToRealName() {
	auto AccessToken = TapUELogin::GetAccessToken();
	bHasTapTokenButNoCompliance = false;
	
	if (AccessToken.IsValid())
	{
		if (HasComplianceInTapToken(AccessToken, UseAgeRange)) {
			ShowRealNameUI(AAURealNameWordTypeQuickVerify);
		} else {
			bHasTapTokenButNoCompliance = true;
			ShowRealNameUI(AAURealNameWordTypeQuickVerify);
		}
	}
	else
	{
		ShowRealNameUI(AAURealNameWordTypeQuickVerify);
	}
}

 bool AAUChinaImpl::HasComplianceInTapToken(const TSharedPtr<FTUAccessToken> TapToken, bool useAgeRange)
{
	if (TapToken != nullptr && TapToken.IsValid())
	{
		if(useAgeRange)
		{
			return TapToken->scope.Contains(TEXT("compliance"));
		}else
		{
			return TapToken->scope.Contains(TEXT("compliance")) || TapToken->scope.Contains(TEXT("compliance_basic"));
		}
	}
	return false;
}


void AAUChinaImpl::SetTestEnv(bool Enable)
{
	AsyncTask(ENamedThreads::GameThread, [Enable]
	{
		bTestEnvEnable = Enable;
		static TWeakObjectPtr<UUserWidget> widget = nullptr;
		if (Enable && (widget == nullptr || !widget.IsValid() || !widget->IsInViewport()))
		{
			if (UClass* MyWidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("WidgetBlueprint'/AntiAddiction/AAUEnvTipWidget.AAUEnvTipWidget_C'")))
			{
				if (TUSettings::GetGameInstance().IsValid())
				{
					widget = CreateWidget<UUserWidget>(TUSettings::GetGameInstance().Get(), MyWidgetClass);
					widget->AddToViewport(INT32_MAX - 10);
				}
			}
		}
		else
		{
			if (widget != nullptr && widget.IsValid() && widget->IsInViewport())
			{
				widget->RemoveFromParent();
				widget = nullptr;
			}
		}
	});
}

void AAUChinaImpl::Exit()
{
	InternalCallback.ExecuteIfBound(AntiAddictionUE::Exited, "");
	Server->Logout();
}

EAAUAgeLimit AAUChinaImpl::GetAgeRange()
{
	return Server->GetAgeRange();
}

int AAUChinaImpl::GetRemainingTime()
{
	return Server->GetRemainingTime();
}

void AAUChinaImpl::KickOut(AAUTimeBoundary Boundary, const FString& Title, const FString& Content)
{
	if (Boundary == AAUTimeBoundaryCurfew)
	{
		ShowHealthTipUI(Title, Content,  true);
	}
	else if(Boundary == AAUTokenExpired)
	{
		Exit();
	}else if(Boundary == AAUInvalidClientOrNetworkError)
	{
		InternalCallback.ExecuteIfBound(AntiAddictionUE::InvalidClientOrNetworkError, "");
	}
}

void AAUChinaImpl::EnterGame()
{
	Server->EnterGame();
}

void AAUChinaImpl::LeaveGame()
{
	Server->LeaveGame();
}

void AAUChinaImpl::CheckPayLimit(int Amount, TFunction<void(bool Status)> CallBack,
                                 TFunction<void(const FString& Msg)> FailureHandler)
{
	Server->CheckPayLimit(Amount, [=](bool Status, const FString& Title, const FString& Description)
	{
		if (CallBack)
		{
			CallBack(Status);
		}
		if (Status == false)
		{
			TSharedRef<STapMessageBox> Box = SNew(STapMessageBox)
			.ForceControllerLanguageType(ELanguageType::ZH)
			.bShowCloseButton(false)
			.TitleText(FText::FromString(Title))
			.ContentRichText(FText::FromString(Description))
			.BlueButtonText(LOCTEXT("BackGame", "返回游戏"));
			UTapSubsystem::AddWidget(Box);
		}
	}, [this, FailureHandler](const FAntiAddictionError& Error)->void
	{
		if(FailureHandler)
		{
			FailureHandler(Error.msg);
		}
		if(Error.IsTokenExpired())
		{
			Exit();
		}
	});
}

void AAUChinaImpl::SubmitPayResult(int Amount, TFunction<void(bool Success)> CallBack,
                                   TFunction<void(const FString& Msg)> FailureHandler)
{
	Server->SubmitPayResult(Amount, CallBack, [this, FailureHandler](const FAntiAddictionError& Error)->void
	{
		if(FailureHandler)
		{
			FailureHandler(Error.msg);
		}
		if(Error.IsTokenExpired())
		{
			Exit();
		}
	});
}

FString AAUChinaImpl::CurrentToken()
{
	return Server->CurrentToken();
}

void AAUChinaImpl::TryAgainStartUp(const FString& ErrMsg)
{
	FString Msg = TEXT("网络错误，请重试");
	if (!ErrMsg.IsEmpty())
	{
		Msg = ErrMsg;
	}

	TSharedRef<STapMessageBox> Box = SNew(STapMessageBox)
		.ForceControllerLanguageType(ELanguageType::ZH)
		.bShowCloseButton(false)
		.ContentRichText(FText::FromString(Msg))
		.BlueButtonText(LOCTEXT("Retry", "重试"))
		.OnBlueButtonClicked(FMessageBoxButtonDelegate::CreateLambda([this](const TSharedRef<class STapMessageBox>& Box)
	                                                     {
		                                                     StartupWithTapTap(CurrentUserID);
	                                                     }));
	UTapSubsystem::AddWidget(Box);
}


void AAUChinaImpl::Login(const FString& AccessToken, bool IsFirst, bool bTapUser)
{
	FAAUUser User;
	User.AccessTokenV2 = AccessToken;
	User.UserID = CurrentUserID;
	TUDataStorage<FAAUStorage>::SaveStruct(FAAUStorage::HasLoginedUser + CurrentUserID, User);

	FTapCommonModule::TapThrobberShowWait();
	//先获取 用户配置
	FetchUserConfig(CurrentUserID, AccessToken,[this, AccessToken, IsFirst, bTapUser](TSharedPtr<FAAUserConfigModel> ModelPtr, const FAntiAddictionError& Error)->void
	{
		if(ModelPtr.IsValid())
		{
			if(!ModelPtr.Get()->content_rating_check.allow)
			{
				FTapCommonModule::TapThrobberDismiss();
				InternalCallback.ExecuteIfBound(AntiAddictionUE::AgeRestrict, TEXT("failed"));
				return;
			}
			CurrentSession = TUHelper::GetRandomStr(32);
			FAAUUser* currentUser = new FAAUUser(CurrentUserID, AccessToken);
			currentUser->ResetAgeLimit(ModelPtr.Get()->real_name.age_limit, ModelPtr.Get()->real_name.is_adult);
		
			Server->Login(*currentUser, UseAgeRange, CurrentSession, [this, IsFirst, bTapUser](TSharedPtr<AAULoginResult> Result, const FAntiAddictionError& Error)
			{
				FTapCommonModule::TapThrobberDismiss();
				if(Result != nullptr && Result.IsValid())
				{
					if (IsFirst && bTapUser)
					{
						if (TSharedPtr<FTULoginProfileModel> Profile = TapUELogin::GetProfile())
						{
							STapToast::Show(ETapToastType::TapTap, NSLOCTEXT("TapTap", "RealNameSuccess", "您已在 TapTap 实名！"), Profile->avatar);
						}
						else
						{
							STapToast::Show(ETapToastType::TapTap, NSLOCTEXT("TapTap", "RealNameSuccess", "您已在 TapTap 实名！"));
						}
					}
					if (Result.Get()->LoginState == AAULoginResult::SuccessWithNoLimit)
					{
						InternalCallback.ExecuteIfBound(AntiAddictionUE::LoginSuccess, TEXT("Success"));
						EnterGame();
					}
					else if (Result.Get()->LoginState == AAULoginResult::SuccessWithLimit)
					{
						ShowHealthTipUI(Result.Get()->Title, Result.Get()->Description, false);
					}
					else //AAULoginResult::FailWithCurfew
					{
						ShowHealthTipUI(Result.Get()->Title, Result.Get()->Description, true);
					}
				}else
				{
					if(Error.IsTokenExpired())
					{
						Exit();
					}else
					{
						InternalCallback.ExecuteIfBound(AntiAddictionUE::InvalidClientOrNetworkError, TEXT("failed"));
					}
				}
			});
		}else
		{
			FTapCommonModule::TapThrobberDismiss();
			if(Error.IsTokenExpired())
			{
				Exit();
			}else
			{
				InternalCallback.ExecuteIfBound(AntiAddictionUE::InvalidClientOrNetworkError, "failed");
			}
		}
	});

	
}

void AAUChinaImpl::FetchUserConfig(const FString& UserId, const FString& Token, TFunction<void(TSharedPtr<FAAUserConfigModel> ModelPtr, const FAntiAddictionError& Error)> CallBack)
{
	AAUNet::FetchUserConfig(UserId,Token,[=](TSharedPtr<FAAUserConfigModel> ModelPtr, const FAntiAddictionError& Error)->void
	{
		if(ModelPtr.IsValid())
		{
			FAAUserConfigModel::SaveToLocal(UserId,ModelPtr);
			CallBack(ModelPtr,Error);
		}else
		{
			if(Error.httpState == TUHttpResponse::networkError || Error.httpState == TUHttpResponse::serverError)
			{
				//尝试从本地加载
				TSharedPtr<FAAUserConfigModel> UserConfig = TUDataStorage<FAAUStorage>::LoadStruct<FAAUserConfigModel>(FAAUStorage::UserConfig + UserId);
				if(UserConfig.IsValid())
				{
					CallBack(UserConfig,Error);
					return;
				}
			}else
			{
				FAAUserConfigModel::ResetModel();
			}
			CallBack(ModelPtr,Error);
		}
	});
}


void AAUChinaImpl::ShowRealNameUI(AAURealNameWordType Type, bool bFallback)
{
	 FAAUAuthIdentifyWord CurrentWordP;
	if(Type == AAURealNameWordType::AAURealNameWordTypeQuickVerify)
	{
		CurrentWordP = FAAURealNameConfigModel::GetLocalModel()->real_name_text.taptap_auth;
	}else if(Type == AAURealNameWordTypeVerifying)
	{
		CurrentWordP = FAAURealNameConfigModel::GetLocalModel()->real_name_text.auth_waiting;
	}else if(Type == AAURealNameWordTypeVerify)
	{
		CurrentWordP = FAAURealNameConfigModel::GetLocalModel()->real_name_text.manual_auth;
	}else{
		CurrentWordP = FAAURealNameConfigModel::GetLocalModel()->real_name_text.auth_failed;
	}
	
	
	switch (Type)
	{
	case AAURealNameWordTypeQuickVerify:
		{
			TWeakPtr<AAUChinaImpl> WImpl = StaticCastSharedRef<AAUChinaImpl>(AsShared());
			auto TempWord = CurrentWordP;
			if (this->bHasTapTokenButNoCompliance) {
				TempWord.negative_button = "";
			}
			TSharedRef<SVerifyEntryWidget> Widget = SNew(SVerifyEntryWidget, StaticCastSharedRef<AAUChinaImpl>(AsShared()), TempWord).OnClose(FSimpleDelegate::CreateLambda([this,WImpl]()
			{
				if (TSharedPtr<AAUChinaImpl> Impl = WImpl.Pin())
				{
					if (!Impl->ManualVerifyWidget.IsValid())
					{
						InternalCallback.ExecuteIfBound(AntiAddictionUE::RealNameStop, "");
					}
				}
			}));
			EntryWidget = Widget;
			UTapSubsystem::AddWidget(Widget);
		}
		break;
	case AAURealNameWordTypeVerify:
		{
			if (TSharedPtr<SVerifyEntryWidget> Widget = EntryWidget.Pin())
			{
				UTapSubsystem::RemoveWidget(Widget.ToSharedRef());
			}
			TWeakPtr<AAUChinaImpl> WImpl = StaticCastSharedRef<AAUChinaImpl>(AsShared());
			TSharedRef<SManualVerifyWidget> Widget = SNew(SManualVerifyWidget, StaticCastSharedRef<AAUChinaImpl>(AsShared()), CurrentWordP).OnClose(FSimpleDelegate::CreateLambda(
				[this,WImpl]()
				{
					if (TSharedPtr<AAUChinaImpl> Impl = WImpl.Pin())
					{
						if (!Impl->EntryWidget.IsValid())
						{
							InternalCallback.ExecuteIfBound(AntiAddictionUE::RealNameStop, "");
						}
					}
				}));
			ManualVerifyWidget = Widget;
			UTapSubsystem::AddWidget(Widget);
		}
		break;
	case AAURealNameWordTypeVerifying:
		{
			TWeakPtr<AAUChinaImpl> WImpl = StaticCastSharedRef<AAUChinaImpl>(AsShared());
			TSharedRef<STapMessageBox> Box = SNew(STapMessageBox)
				.ForceControllerLanguageType(ELanguageType::ZH)
				.TitleText(FText::FromString(CurrentWordP.title))
				.ContentRichText(FText::FromString(CurrentWordP.description_plain))
				.BlueButtonText(FText::FromString(CurrentWordP.negative_button))
			.WhiteButtonText(FText::FromString(CurrentWordP.positive_button)).OnWhiteButtonClicked(FMessageBoxButtonDelegate::CreateLambda([](const TSharedRef<class STapMessageBox>& Box)
			{
				if (UGameInstance* GI = TUSettings::GetGameInstance().Get())
				{
					UKismetSystemLibrary::QuitGame(GI, GI->GetFirstLocalPlayerController(), EQuitPreference::Quit, true);
					UTapSubsystem::RemoveWidget(Box);
				}
			}));
			UTapSubsystem::AddWidget(Box);
		}
		break;
	case AAURealNameWordTypeVerifyFail:
		STapToast::Show(ETapToastType::Error, FText::FromString(CurrentWordP.description_plain));
		break;
	default:
		ensure(false);
	}
	
}

void AAUChinaImpl::GotoTapTapQuickVerify()
{
	StartupQuickVerifyTapAuth();
}


void AAUChinaImpl::ShowHealthTipUI(const FString& Title, const FString& Content, bool NeedStrict)
{
	if (NeedStrict)
	{
		InternalCallback.ExecuteIfBound(AntiAddictionUE::PeriodRestrict, "");
		TSharedPtr<STapMessageBox> Box;
		if (Config.ShowSwitchAccount)
		{
			Box = SNew(STapMessageBox)
				.ForceControllerLanguageType(ELanguageType::ZH)
				.bShowCloseButton(false)
				.TitleText(FText::FromString(Title))
				.ContentRichText(FText::FromString(Content))
				.WhiteButtonText(LOCTEXT("QuitGame", "退出游戏"))
				.BlueButtonText(LOCTEXT("SwitchAccount", "切换账号"))
				.bCloseBoxAfterWhiteButtonClicked(false)
				.OnWhiteButtonClicked(FMessageBoxButtonDelegate::CreateLambda([](const TSharedRef<class STapMessageBox>& Box)
																 {
																	 if (UGameInstance* GI = TUSettings::GetGameInstance().Get())
																	 {
																		 UKismetSystemLibrary::QuitGame(GI, GI->GetFirstLocalPlayerController(), EQuitPreference::Quit, true);
																		 UTapSubsystem::RemoveWidget(Box);
																	 }
																	 else
																	 {
																		 UE_LOG(LogTap, Warning, TEXT("Quit game, game instance is null."));
																	 }
																 }))
				.OnBlueButtonClicked(FMessageBoxButtonDelegate::CreateLambda([this](const TSharedRef<class STapMessageBox>& Box)
													 {
														 InternalCallback.ExecuteIfBound(AntiAddictionUE::SwitchAccount, "");
													 }));
		}
		else
		{
			Box = SNew(STapMessageBox)
				.ForceControllerLanguageType(ELanguageType::ZH)
				.bShowCloseButton(false)
				.TitleText(FText::FromString(Title))
				.ContentRichText(FText::FromString(Content))
				.BlueButtonText(LOCTEXT("QuitGame", "退出游戏"))
				.bCloseBoxAfterWhiteButtonClicked(false)
				.OnBlueButtonClicked(FMessageBoxButtonDelegate::CreateLambda([](const TSharedRef<class STapMessageBox>& Box)
																 {
																	 if (UGameInstance* GI = TUSettings::GetGameInstance().Get())
																	 {
																		 UKismetSystemLibrary::QuitGame(GI, GI->GetFirstLocalPlayerController(), EQuitPreference::Quit, true);
																		 UTapSubsystem::RemoveWidget(Box);
																	 }
																	 else
																	 {
																		 UE_LOG(LogTap, Warning, TEXT("Quit game, game instance is null."));
																	 }
																 }));
		}
		UTapSubsystem::AddWidget(Box.ToSharedRef());
	}
	else 
	{
		TSharedRef<STapMessageBox> Box = SNew(STapMessageBox)
				.ForceControllerLanguageType(ELanguageType::ZH)
				.bShowCloseButton(false)
				.TitleText(FText::FromString(Title))
				.ContentRichText(FText::FromString(Content))
				.BlueButtonText(LOCTEXT("EnterGame", "进入游戏"))
				.OnBlueButtonClicked(FMessageBoxButtonDelegate::CreateLambda([this](const TSharedRef<class STapMessageBox>& Box)
		                                                     {
			                                                     InternalCallback.ExecuteIfBound(AntiAddictionUE::LoginSuccess, "Success");
																 EnterGame();
		                                                     }));
		UTapSubsystem::AddWidget(Box);
	}
}


void AAUChinaImpl::StartupQuickVerifyTapAuth()
{
	// Request token
	FTapAuthComplete TapAuthCallback = FTapAuthComplete::CreateLambda(
		[this](const TUAuthResult& Result)
		{
			UE_LOG(LogTap, Log, TEXT("Tap quick verify, tap auth result type: %d."), Result.GetType());
			switch (Result.GetType())
			{
			case TUAuthResult::Success:
				{
					TSharedPtr<FTUAccessToken> TapToken = Result.GetToken();
					check(TapToken.IsValid());
					if (HasComplianceInTapToken(TapToken,UseAgeRange))
					{
						QuickVerify(TapToken.ToSharedRef());
					}
					else
					{
						UE_LOG(LogTap, Warning, TEXT("Compliance auth result token doesn't contains scope 'compliance'."));
						STapToast::Show(ETapToastType::Error, LOCTEXT("AuthError", "授权错误"));
						ShowRealNameUI(AAURealNameWordTypeVerify, true);
					}
				}
				break;
			case TUAuthResult::Cancel:
				STapToast::Show(ETapToastType::Warning, LOCTEXT("AuthCancel", "授权取消"));
				if (!EntryWidget.IsValid())
				{
					InternalCallback.ExecuteIfBound(AntiAddictionUE::RealNameStop, "");
				}
				break;
			case TUAuthResult::Fail:
				STapToast::Show(ETapToastType::Error, LOCTEXT("AuthError", "授权错误"));
				ShowRealNameUI(AAURealNameWordTypeVerify, true);
				break;
			default: ;
			}
		});
	const FString permission = UseAgeRange ? TEXT("compliance") : TEXT("compliance_basic");
	TArray<FString> Permissions{permission};
	UTapSubsystem::AddWidget(
		SNew(STapLoginWidget, Permissions, ERegionType::CN)
		.bRequestProfile(false)
		.OnAuthComplete(TapAuthCallback));
}

void AAUChinaImpl::QuickVerify(const TSharedRef<FTUAccessToken>& TapToken)
{
	FTapCommonModule::TapThrobberShowWait();
	AAUChinaRealName::CheckRealNameStateByTapToken(CurrentUserID,TapToken,"",[this](TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)->void
	{
		HandleQuickVerifyTdsResult(ModelPtr, Error);
	});
}


void AAUChinaImpl::HandleQuickVerifyTdsResult(TSharedPtr<FAAURealNameResultModel> ModelPtr, const FAntiAddictionError& Error)
{
	FTapCommonModule::TapThrobberDismiss();
	if (ModelPtr.IsValid())
	{
		UE_LOG(LogTap, Log, TEXT("Tap quick verify, state: %d."), ModelPtr->GetAuthState());
		switch (ModelPtr->GetAuthState())
		{
		case FAAURealNameResultModel::Success:
			{
				if (TSharedPtr<SVerifyEntryWidget> Widget = EntryWidget.Pin())
				{
					UTapSubsystem::RemoveWidget(Widget.ToSharedRef());
				}
				Login(ModelPtr->anti_addiction_token, true, true);
			}
			break;
		case FAAURealNameResultModel::Verifying:
			ShowRealNameUI(AAURealNameWordTypeVerifying);
			break;
		case FAAURealNameResultModel::Fail:
			{
				STapToast::Show(ETapToastType::Error, NSLOCTEXT("TapTap", "AuthFailed", "认证未通过，请在 Tap 客户端重新提交实名信息"));
				if (!EntryWidget.IsValid() && !ManualVerifyWidget.IsValid())
				{
					InternalCallback.ExecuteIfBound(AntiAddictionUE::RealNameStop, "");
				}
			}
			break;
		}
	}
	else
	{
		ShowRealNameUI(AAURealNameWordTypeVerify, true);
	}
}

#undef LOCTEXT_NAMESPACE