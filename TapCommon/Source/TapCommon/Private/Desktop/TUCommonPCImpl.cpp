#include "TUCommonPCImpl.h"

#include "MessageEndpoint.h"
#include "TapClientBridge.hpp"
#include "TapSubsystem.h"
#include "TUError.h"
#include "TULanguage.h"
#include "Controller/TapControllerManager.h"
#include "Slate/Widgets/TapClientVerifyTipWidget.h"
#include "Slate/Widgets/TapToast.h"
#include "TapCommon.h"
#include "MessageEndpointBuilder.h"


bool TUCommonPCImpl::HasCheckLaunchStateByTapClient = false;
// 上次检查 launch 结果 0 成功 1 未知错误 2 未找到客户端  3 未通过客户端启动
int TUCommonPCImpl::LastIsLaunchByTapPCResult = -1;
TFunction<void(const bool IsCancel,  const bool IsFail, FString ErrMsg, FString RedirectUrl)> TUCommonPCImpl::CurrentLoginCallBack = nullptr;

FDelegateHandle TUCommonPCImpl::TickDelegateHandle;

void TUCommonPCImpl::SetLanguage(ELanguageType LanguageType) {
	TULanguage::SetCurrentType(LanguageType);
}

void TUCommonPCImpl::IsLaunchedFromTapTapPC(TFunction<void(const bool Pass)> CallBack)
{
#if PLATFORM_WINDOWS
	
	if (LastIsLaunchByTapPCResult > -1)
	{
		CallBack(LastIsLaunchByTapPCResult == 0);
		return;
	}
	if(HasCheckLaunchStateByTapClient)
	{
		STapToast::Show(ETapToastType::Error,FText::FromString( TEXT("IsLaunchedFromTapTapPC 正在处理中，不能多次同时调用")));
		UE_LOG(LogTap, Error, TEXT("IsLaunchedFromTapTapPC IsLaunchedFromTapTapPC 正在处理中，不能多次同时调用"));
		CallBack(false);
		return;
	}
	HasCheckLaunchStateByTapClient = true;
	static int32 bTaskCompleted = 0;  // 局部标志变量，用来标记任务是否完成
	static int32 bTaskTimedOut = 0;   // 局部标志变量，用来标记任务是否超时
	const FString ClientID = FTUConfig::Get()->ClientID;
	const FString ClientPublicKey = FTUConfig::Get()->ClientPublicKey;
	if (ClientID.IsEmpty() || ClientPublicKey.IsEmpty() )
	{
		STapToast::Show(ETapToastType::Error,FText::FromString( TEXT("IsLaunchedFromTapTapPC : ClientID & ClientPublicKey 不能为空 ClientId = " + ClientID + " PublicKey = " + ClientPublicKey)));
		UE_LOG(LogTap, Error, TEXT("IsLaunchedFromTapTapPC : ClientID & ClientPublicKey 不能为空 ClientId = %s PublicKey = %s"), *ClientID, *ClientPublicKey);
		CallBack(false);
		return;
	}
	// 在 I/O 线程中执行任务
	AsyncTask(ENamedThreads::AnyThread, [CallBack, ClientID, ClientPublicKey]()
	{
		UE_LOG(LogTemp, Log, TEXT("Task started on I/O thread"));
		uint32 Result = 0;
		bool NeedRestart = TapClientBridge::RestartApp(TCHAR_TO_UTF8(*ClientID));
		if(NeedRestart)
		{
			LastIsLaunchByTapPCResult = 0;
			// 退出游戏
			FGenericPlatformMisc::RequestExit(false);
		}else
		{	ErrMsg ErrorMessage;
			Result = TapClientBridge::InitSDK(ErrorMessage,TCHAR_TO_UTF8(*ClientPublicKey));
			LastIsLaunchByTapPCResult = Result;
			UE_LOG(LogTap, Log, TEXT("InitSDK by TapClient result = %d %hs"), Result, ErrorMessage);
		}
		// 完成任务后，标记任务完成
		FPlatformAtomics::InterlockedIncrement(&bTaskCompleted);
		// 在主线程中执行回调
		AsyncTask(ENamedThreads::GameThread, [CallBack, &Result, ClientID]()
		{
			if (FPlatformAtomics::InterlockedCompareExchange(const_cast<int32*>(&bTaskTimedOut), 1,1) < 1)  // 只有在超时未触发的情况下才执行回调
				{
					if(Result == 0)
					{
						char CacheClientId[256];
						const bool GetClientSuccess = TapClientBridge::GetClientID(CacheClientId);
						if (GetClientSuccess)
						{
							const FString CacheClientIdString = UTF8_TO_TCHAR(CacheClientId);
							if(!CacheClientIdString.IsEmpty() && CacheClientIdString != ClientID)
							{
								STapToast::Show(ETapToastType::Error,FText::FromString( TEXT("IsLaunchedFromTapTapPC : SDK 配置的 ClientID = " + ClientID + " 与 Tap 客户端 的 " + CacheClientIdString + " 不匹配" )));
								UE_LOG(LogTap, Error, TEXT("IsLaunchedFromTapTapPC : ClientID & ClientPublicKey : SDK 配置的 ClientID = %s 与 Tap 客户端 的 %s 不匹配 "), *ClientID, *CacheClientIdString);
								CallBack(false);
								return;
							}
						}
						char CacheOpenId[256];
						const bool GetOpenIdSuccess = TapClientBridge::GetOpenID(CacheOpenId);
						if (GetOpenIdSuccess)
						{
							const FString CacheOpenIdString = UTF8_TO_TCHAR(CacheOpenId);
							if(!CacheOpenIdString.IsEmpty() )
							{
								TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint =
									FMessageEndpoint::Builder("TapUserSyncBus");
								if(MessageEndpoint.IsValid())
								{
									MessageEndpoint->Publish<FTapCustomMessage>(new FTapCustomMessage("syncTapUser", CacheOpenIdString));
								}
							}
						}
						TickDelegateHandle = FWorldDelegates::OnWorldTickStart.AddStatic(&TUCommonPCImpl::OnWorldTickListener);
						CallBack(true);  // 任务成功完成
					}else
					{
						TSharedRef<STapClientVerifyTipWidget> Widget = SNew(STapClientVerifyTipWidget);
						Widget->SetInitErrorType(Result);
						UTapSubsystem::AddWidget(Widget);
						CallBack(false);
					}
				}
			});
		});

	// 设置超时（5 秒）
	AsyncTask(ENamedThreads::AnyThread, [CallBack]()
	{
		FPlatformProcess::Sleep(5.0f);  // 超过 5 秒即超时
		int32 CompleteValue = FPlatformAtomics::InterlockedCompareExchange(const_cast<int32*>(&bTaskCompleted),1,1);
		UE_LOG(LogTap, Log, TEXT("check InitSDK by TapClient  timeout = %d "),  CompleteValue);

		if ( CompleteValue < 1)  // 如果任务尚未完成，表示超时
			{
				FPlatformAtomics::InterlockedIncrement(&bTaskTimedOut);  // 标记任务已超时
				LastIsLaunchByTapPCResult = 1;
				// 在主线程回调
				AsyncTask(ENamedThreads::GameThread, [CallBack]()
				{
					TSharedRef<STapClientVerifyTipWidget> Widget = SNew(STapClientVerifyTipWidget);
					Widget->SetInitErrorType(TapClientBridge::TapSDKInitResult::Timeout);
					UTapSubsystem::AddWidget(Widget);
					UE_LOG(LogTemp, Error, TEXT("Task timed out after 5 seconds"));
					CallBack(false);  // 任务超时
				});
			}
		});
#else
	if(CallBack)
	{
		CallBack(true);
	}
#endif

}


bool TUCommonPCImpl::TapControllerStartup(ETapControllerType InControllerType)
{
	return FTapControllerManager::Get().Startup(InControllerType);
}

void TUCommonPCImpl::TapControllerUpdateConfig(const TSharedPtr<FTapControllerConfig>& InConfig)
{
	FTapControllerManager::Get().UpdateConfig(InConfig);
}

void TUCommonPCImpl::TapControllerShutdown()
{
	FTapControllerManager::Get().Shutdown();
}

bool TUCommonPCImpl::IsTapControllerAlreadyStarted() const
{
	return FTapControllerManager::Get().IsAlreadyStarted();
}

bool TUCommonPCImpl::IsTapControllerInputAwake() const
{
	return FTapControllerManager::Get().IsTapControllerRunning();
}

bool TUCommonPCImpl::IsSupportLoginByTapClient() 
{
	return HasCheckLaunchStateByTapClient;
}

void TUCommonPCImpl::TapLoginByTapPCClient(TArray<FString> Permissions, FString ResponseType,
		FString RedirectUri, FString CodeChallenge, FString State, FString CodeChallengeMethod,
		FString Version, FString SdkUa, FString Info, TFunction<void(const bool IsCancel, const bool IsFail, FString ErrMsg, FString RedirectUrl)> CallBack) 
{
	CurrentLoginCallBack = CallBack;
	AsyncTask(ENamedThreads::AnyThread, [Permissions, ResponseType,
		RedirectUri, CodeChallenge, State, CodeChallengeMethod,
		Version, SdkUa, Info]
	{
		TapClientBridge::RegisterCallback(2001,TapLoginDelegate);
		const FString PermissionString = FString::Join(Permissions, TEXT(","));
		int32 result =  TapClientBridge::AsyncAuthorize(TCHAR_TO_UTF8(*PermissionString),TCHAR_TO_UTF8(*ResponseType),
		TCHAR_TO_UTF8(*RedirectUri),TCHAR_TO_UTF8(*CodeChallenge), TCHAR_TO_UTF8(*State),TCHAR_TO_UTF8(*CodeChallengeMethod),
		TCHAR_TO_UTF8(*Version),TCHAR_TO_UTF8(*SdkUa),TCHAR_TO_UTF8(*Info));
		// 发起授权失败
		if(result != 1)
		{
			AsyncTask(ENamedThreads::GameThread, []
			{
				CurrentLoginCallBack(false, true, TEXT("发起授权失败，请确认 Tap 客户端是否正常运行"), "");
				CurrentLoginCallBack = nullptr;
			});
			TapClientBridge::UnRegisterCallback(2001,TapLoginDelegate);
		}
	});
}

void TUCommonPCImpl::TapLoginDelegate(int CallbackId, void* UserData)
{
	// 登录回调
	if (CallbackId == 2001)
	{
		UE_LOG(LogTap,Log,TEXT("TapLoginCallback dellgate recevied"));
		AsyncTask(ENamedThreads::GameThread, [UserData]
		{
			if(CurrentLoginCallBack != nullptr && UserData != nullptr)
			{
				const AuthorizeFinishedResponse* Response = static_cast<AuthorizeFinishedResponse*>(UserData);
				UE_LOG(LogTap,Log,TEXT("TapLoginCallback dellgate recevied %d %hs"), Response->is_cancel,  UTF8_TO_TCHAR(Response->callback_uri) );
				CurrentLoginCallBack(Response->is_cancel > 0, false, "", UTF8_TO_TCHAR(Response->callback_uri));
				CurrentLoginCallBack = nullptr;
			}
		});
		TapClientBridge::UnRegisterCallback(2001,TapLoginDelegate);
	}
	
}

void TUCommonPCImpl::OnWorldTickListener(UWorld* World, ELevelTick TickType, float DeltaTime)
{
	if(World)
	{
		TapClientBridge::RunCallbacks();
	}
}



