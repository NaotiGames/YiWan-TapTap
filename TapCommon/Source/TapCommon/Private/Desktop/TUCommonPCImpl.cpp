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
#include <stdexcept>


bool TUCommonPCImpl::HasCheckLaunchStateByTapClient = false;
bool TUCommonPCImpl::HasRegisterTapPCStateChangeListener = false;
TArray<TapTapPCStateChangeListener> TUCommonPCImpl::TapPCStateChangeListeners;
// 上次检查 launch 结果 0 成功 1 未知错误 2 未找到客户端  3 未通过客户端启动
int TUCommonPCImpl::LastIsLaunchByTapPCResult = -1;
TFunction<void(const bool IsCancel,  const bool IsFail, FString ErrMsg, FString RedirectUrl)> TUCommonPCImpl::CurrentLoginCallBack = nullptr;
TFunction<void(FString DLCId, const bool IsOwned)> TUCommonPCImpl::CurrentDLCCallBack = nullptr;
TFunction<void(const bool IsOwned)> TUCommonPCImpl::CurrentLicenseCallBack = nullptr;
FDelegateHandle TUCommonPCImpl::TickDelegateHandle;

void TUCommonPCImpl::SetLanguage(ELanguageType LanguageType) {
	TULanguage::SetCurrentType(LanguageType);
}

void TUCommonPCImpl::IsLaunchedFromTapTapPC(TFunction<void(const bool Pass)> CallBack)
{
#if PLATFORM_WINDOWS
	
	if (LastIsLaunchByTapPCResult > -1)
	{
		if(CallBack)
		{
			CallBack(LastIsLaunchByTapPCResult == 0);
		}
		return;
	}
	if(HasCheckLaunchStateByTapClient)
	{
		STapToast::Show(ETapToastType::Error,FText::FromString( TEXT("IsLaunchedFromTapTapPC 正在处理中，不能多次同时调用")));
		UE_LOG(LogTap, Error, TEXT("IsLaunchedFromTapTapPC IsLaunchedFromTapTapPC 正在处理中，不能多次同时调用"));
		if(CallBack)
		{
			CallBack(false);
		}
		return;
	}
	HasCheckLaunchStateByTapClient = true;
	const FString ClientID = FTUConfig::Get()->ClientID;
	const FString ClientPublicKey = FTUConfig::Get()->ClientPublicKey;
	if (ClientID.IsEmpty() || ClientPublicKey.IsEmpty() )
	{
		STapToast::Show(ETapToastType::Error,FText::FromString( TEXT("IsLaunchedFromTapTapPC : ClientID & ClientPublicKey 不能为空 ClientId = " + ClientID + " PublicKey = " + ClientPublicKey)));
		UE_LOG(LogTap, Error, TEXT("IsLaunchedFromTapTapPC : ClientID & ClientPublicKey 不能为空 ClientId = %s PublicKey = %s"), *ClientID, *ClientPublicKey);
		if(CallBack)
		{
			CallBack(false);
		}
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
			UE_LOG(LogTap, Log, TEXT("InitSDK by TapClient result = %d %s"), Result, *FString(UTF8_TO_TCHAR(ErrorMessage)));
		}
		// 在主线程中执行回调
		AsyncTask(ENamedThreads::GameThread, [CallBack, Result, ClientID]()
		{
			UE_LOG(LogTap, Log, TEXT("InitSDK by TapClient check = %d "), Result);
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
						if(CallBack)
						{
							CallBack(false);
						}
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
				if(CallBack)
				{
					CallBack(true);  // 任务成功完成
				}
				if(!HasRegisterTapPCStateChangeListener && TapPCStateChangeListeners.Num() > 0)
				{
					HasRegisterTapPCStateChangeListener = true;
					TapClientBridge::RegisterCallback(TapClientBridge::TapEventID::SystemStateChanged, TapClientStateChangedDelegate);
				}
			}else
			{
				TSharedRef<STapClientVerifyTipWidget> Widget = SNew(STapClientVerifyTipWidget);
				Widget->SetInitErrorType(Result);
				UTapSubsystem::AddWidget(Widget);
				if(CallBack)
				{
					CallBack(false);
				}
			}
			
		});
	});
#else
	if(CallBack)
	{
		CallBack(false);
	}
#endif

}

void TUCommonPCImpl::RegisterTapTapPCStateChangeListener(TapTapPCStateChangeListener Listener)
{
#if PLATFORM_WINDOWS
	AsyncTask(ENamedThreads::GameThread, [Listener]()
	{
		if(LastIsLaunchByTapPCResult == 0 && !HasRegisterTapPCStateChangeListener)
		{
			HasRegisterTapPCStateChangeListener = true;
			TapClientBridge::RegisterCallback(TapClientBridge::TapEventID::SystemStateChanged, TapClientStateChangedDelegate);
		}
		if(Listener != nullptr && !TapPCStateChangeListeners.Contains(Listener))
		{
			TapPCStateChangeListeners.Add(Listener);
		}
	});
	
#endif
	
}

void TUCommonPCImpl::UnRegisterTapTapPCStateChangeListener(TapTapPCStateChangeListener Listener)
{
#if PLATFORM_WINDOWS
	AsyncTask(ENamedThreads::GameThread, [Listener]()
	{
		TapPCStateChangeListeners.Remove(Listener);
	});
	
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
	AsyncTask(ENamedThreads::AnyThread, [CallBack,Permissions, ResponseType,
		RedirectUri, CodeChallenge, State, CodeChallengeMethod,
		Version, SdkUa, Info]
	{
		TapClientBridge::RegisterCallback(TapClientBridge::TapEventID::AuthorizeFinished_internal,TapLoginDelegate);
		const FString PermissionString = FString::Join(Permissions, TEXT(","));
		int32 result =  TapClientBridge::AsyncAuthorize(TCHAR_TO_UTF8(*PermissionString),TCHAR_TO_UTF8(*ResponseType),
		TCHAR_TO_UTF8(*RedirectUri),TCHAR_TO_UTF8(*CodeChallenge), TCHAR_TO_UTF8(*State),TCHAR_TO_UTF8(*CodeChallengeMethod),
		TCHAR_TO_UTF8(*Version),TCHAR_TO_UTF8(*SdkUa),TCHAR_TO_UTF8(*Info));
		// 发起授权失败
		if(result != 1)
		{
			AsyncTask(ENamedThreads::GameThread, [CallBack]
			{
				CurrentLoginCallBack = nullptr;
				CallBack(false, true, TEXT("发起授权失败，请确认 Tap 客户端是否正常运行"), "");
			});
			
			TapClientBridge::UnRegisterCallback(TapClientBridge::TapEventID::AuthorizeFinished_internal,TapLoginDelegate);
		}
	});
}

void TUCommonPCImpl::TapLoginDelegate(int CallbackId, void* UserData)
{
	// 登录回调
	if (CallbackId == TapClientBridge::TapEventID::AuthorizeFinished_internal)
	{
		if(CurrentLoginCallBack != nullptr && UserData != nullptr)
		{
			TFunction<void(const bool IsCancel,  const bool IsFail, FString ErrMsg, FString RedirectUrl)> TempCopy = CurrentLoginCallBack;
			const AuthorizeFinishedResponse* Response = static_cast<AuthorizeFinishedResponse*>(UserData);
			UE_LOG(LogTap,Log,TEXT("TapLoginCallback dellgate recevied %d %s"), Response->is_cancel,  UTF8_TO_TCHAR(Response->callback_uri) );

			CurrentLoginCallBack = nullptr;
			TapClientBridge::UnRegisterCallback(TapClientBridge::TapEventID::AuthorizeFinished_internal,TapLoginDelegate);

			TempCopy(Response->is_cancel > 0, false, "", UTF8_TO_TCHAR(Response->callback_uri));
			TempCopy = nullptr;	

		}
	}
}

bool TUCommonPCImpl::QueryTapDLC(FString dlcId)
{
	// if (LastIsLaunchByTapPCResult != 1)
	// {
	// 	throw std::runtime_error("queryDLC must be invoked after IsLaunchedFromTapTapPC success");
	// }
	checkf(LastIsLaunchByTapPCResult == 0, TEXT("queryDLC must be invoked after IsLaunchedFromTapTapPC success"));
	return TapClientBridge::TapDLCIsOwned(TCHAR_TO_UTF8(* dlcId));
}
bool TUCommonPCImpl::ShowStoreWithDLC(FString dlcId)
{
	checkf(LastIsLaunchByTapPCResult == 0, TEXT("PurchaseDLC must be invoked after IsLaunchedFromTapTapPC success"));
	return TapClientBridge::TapDLCShowStore(TCHAR_TO_UTF8(* dlcId));
}
bool TUCommonPCImpl::HasTapLicense()
{
	checkf(LastIsLaunchByTapPCResult == 0, TEXT("License Check must be invoked after IsLaunchedFromTapTapPC success"));
	return TapClientBridge::TapAppIsOwned();
}

void TUCommonPCImpl::RegisterDLCOwnedCallback(TFunction<void(FString DLCId, const bool IsOwned)> Callback)
{
	CurrentDLCCallBack = Callback;
	TapClientBridge::RegisterCallback(TapClientBridge::TapEventID::DLCPlayableStatusChanged, TapDLCDelegate);
}
void TUCommonPCImpl::RegisterLicenseCallback(TFunction<void(const bool IsOwned)> Callback)
{
	CurrentLicenseCallBack = Callback;
	TapClientBridge::RegisterCallback(TapClientBridge::TapEventID::GamePlayableStatusChanged, TapLicenseDelegate);
}

void TUCommonPCImpl::TapLicenseDelegate(int CallbackId, void* UserData)
{
	if (CallbackId == TapClientBridge::TapEventID::GamePlayableStatusChanged)
	{
		if(CurrentLicenseCallBack != nullptr && UserData != nullptr)
		{
			const GamePlayableStatusChangedResponse* Response = static_cast<GamePlayableStatusChangedResponse*>(UserData);
			UE_LOG(LogTap,Log,TEXT("TapLicenseDelegate dellgate recevied Raw: 0x%02X, Bool: %s"), Response->is_playable, Response->is_playable != 0 ? TEXT("true") : TEXT("false"));
			CurrentLicenseCallBack(Response->is_playable);
		}
	}
}

void TUCommonPCImpl::TapDLCDelegate(int CallbackId, void* UserData)
{
	if (CallbackId == TapClientBridge::TapEventID::DLCPlayableStatusChanged)
	{
		if(CurrentDLCCallBack != nullptr && UserData != nullptr)
		{
			const DLCPlayableStatusChangedResponse* Response = static_cast<DLCPlayableStatusChangedResponse*>(UserData);
			UE_LOG(LogTap,Log,TEXT("TapDLCDelegate dellgate recevied  %s %s "), UTF8_TO_TCHAR(Response->dlc_id), Response->is_playable ? TEXT("true") : TEXT("false"));
			CurrentDLCCallBack(UTF8_TO_TCHAR(Response->dlc_id), Response->is_playable);
		}
	}
}

void TUCommonPCImpl::TapClientStateChangedDelegate(int CallbackId, void* UserData)
{
	if (CallbackId == TapClientBridge::TapEventID::SystemStateChanged)
	{
		if(UserData != nullptr)
		{
			const TapPCStateChangedResponse* Response = static_cast<TapPCStateChangedResponse*>(UserData);
			UE_LOG(LogTap,Log,TEXT("TapPCState dellgate recevied  %d "), Response->state);
			for (auto Listener : TapPCStateChangeListeners)
			{
				Listener(Response->state);
			}
		}
	}
}



void TUCommonPCImpl::OnWorldTickListener(UWorld* World, ELevelTick TickType, float DeltaTime)
{
	if(World)
	{
		TapClientBridge::RunCallbacks();
	}
}



