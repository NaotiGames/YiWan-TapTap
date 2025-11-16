#pragma once

#include "CoreMinimal.h"

typedef unsigned int uint32;
typedef char ErrMsg[1024];

// SDK 函数指针定义
typedef uint32 (*SDK_InitFunc)(ErrMsg *errMsg, const char *pubKey);
typedef bool (*ShutdownFunc)();
typedef bool (*RestartAppFunc)(const char *clientID);
typedef bool (*GetClientIDFunc)(char *buffer);
typedef bool (*GetOpenIDFunc)(char *buffer);
typedef void (*RegisterCallbackFunc)(int callbackID, void (*callback)(int, void*));
typedef void (*UnRegisterCallbackFunc)(int callbackID, void (*callback)(int, void*));

typedef void (*RunCallbacksFunc)();
typedef uint32 (*AsyncAuthorizeFunc)(const char* scopes, const char* response_type,
									 const char* redirect_uri, const char* code_challenge, const char* state,
									 const char* code_challenge_method, const char* version, const char* sdk_ua,
									 const char* info);
typedef bool (*TapAppIsOwnedFunc)();
typedef bool (*TapDLCShowStoreFunc)(const char *dlcId);
typedef bool (*TapDLCIsOwnedFunc)(const char *dlcId);

struct AuthorizeFinishedResponse
{
	uint32 is_cancel;          // 是否取消（0：否，1：是）
	char callback_uri[1024];   // 回调地址
};

struct GamePlayableStatusChangedResponse
{
	bool is_playable;
};

struct DLCPlayableStatusChangedResponse
{
	char dlc_id[32];
	bool is_playable;
};

class TapClientBridge
{
public:
	static bool LoadSDK();
	static void UnloadSDK();
	static uint32 InitSDK(ErrMsg& errMsg, const char* pubKey);
	static bool Shutdown();
	static bool RestartApp(const char* clientID);
	static bool GetClientID(char* buffer);
	static bool GetOpenID(char* buffer);
	static void RegisterCallback(int callbackID, void (*callback)(int, void*));
	static void UnRegisterCallback(int callbackID, void (*callback)(int, void*));

	static void RunCallbacks();
	static uint32 AsyncAuthorize(const char* scopes, const char* response_type,
								 const char* redirect_uri, const char* code_challenge, const char* state,
								 const char* code_challenge_method, const char* version, const char* sdk_ua,
							 	 const char* info);
	static bool TapAppIsOwned();

	static bool TapDLCShowStore(const char* dlcId);

	static bool TapDLCIsOwned(const char* dlcId);
	
	enum TapSDKInitResult
	{
		// 初始化成功
		OK = 0,
		// 其他错误
		FailedGeneric = 1,
		// 未找到 TapTap，用户可能未安装，请引导用户下载安装 TapTap
		NoPlatform = 2,
		// 已安装 TapTap，游戏未通过 TapTap 启动
		NotLaunchedByPlatform = 3,
		// 平台版本不匹配，请引导用户升级 TapTap 与游戏至最新版本，再重新运行游戏
		PlatformVersionMismatch = 4,

		// SDK 本地执行时未知错误
		Unknown = -1
	};

	enum TapEventID
	{
		// [1, 2000), reserved for TapTap platform events
		// 1 reserved for TapTap platform events
		// SystemStateChanged = 1,

		// [2001, 4000), reserved for TapTap user events
		AuthorizeFinished_internal = 2001,

		AuthorizeFinished = 2002,

		// [4001, 6000), reserved for TapTap ownership events
		GamePlayableStatusChanged = 4001,
		
		DLCPlayableStatusChanged = 4002,


	};
	
private:
	static void* SDKHandle;

	static SDK_InitFunc SDK_Init;
	static ShutdownFunc SDK_Shutdown;
	static RestartAppFunc SDK_RestartApp;
	static GetClientIDFunc SDK_GetClientID;
	static GetClientIDFunc SDK_GetOpenID;
	static RegisterCallbackFunc SDK_RegisterCallback;
	static RegisterCallbackFunc SDK_UnRegisterCallback;
	static RunCallbacksFunc SDK_RunCallbacks;
	static AsyncAuthorizeFunc SDK_AsyncAuthorize;
	static TapAppIsOwnedFunc SDK_TapAppIsOwned;
	static TapDLCShowStoreFunc SDK_TapDLCShowStore;
	static TapDLCIsOwnedFunc SDK_TapDLCIsOwned;
};