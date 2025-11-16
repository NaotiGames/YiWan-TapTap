#include "TapClientBridge.hpp"

void* TapClientBridge::SDKHandle = nullptr;

SDK_InitFunc TapClientBridge::SDK_Init = nullptr;
ShutdownFunc TapClientBridge::SDK_Shutdown = nullptr;
RestartAppFunc TapClientBridge::SDK_RestartApp = nullptr;
GetClientIDFunc TapClientBridge::SDK_GetClientID = nullptr;
GetOpenIDFunc TapClientBridge::SDK_GetOpenID = nullptr;
RegisterCallbackFunc TapClientBridge::SDK_RegisterCallback = nullptr;
UnRegisterCallbackFunc TapClientBridge::SDK_UnRegisterCallback = nullptr;
RunCallbacksFunc TapClientBridge::SDK_RunCallbacks = nullptr;
AsyncAuthorizeFunc TapClientBridge::SDK_AsyncAuthorize = nullptr;
TapAppIsOwnedFunc TapClientBridge::SDK_TapAppIsOwned = nullptr;
TapDLCShowStoreFunc TapClientBridge::SDK_TapDLCShowStore = nullptr;
TapDLCIsOwnedFunc TapClientBridge::SDK_TapDLCIsOwned = nullptr;

bool TapClientBridge::LoadSDK()
{
    FString DllPath = FPaths::ProjectDir() / TEXT("Plugins/OAuthLogin/ThirdPlugins/TapTap/TapCommon/Source/ThirdParty/Windows/taptap_api.dll");

    SDKHandle = FPlatformProcess::GetDllHandle(*DllPath);
    if (!SDKHandle)
    {
        UE_LOG(LogTemp, Error, TEXT("无法加载 SDK DLL: %s"), *DllPath);
        return false;
    }

    // 获取函数指针
    SDK_Init = reinterpret_cast<SDK_InitFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapSDK_Init")));
    SDK_Shutdown = reinterpret_cast<ShutdownFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapSDK_Shutdown")));
    SDK_RestartApp = reinterpret_cast<RestartAppFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapSDK_RestartAppIfNecessary")));
    SDK_GetClientID = reinterpret_cast<GetClientIDFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapSDK_GetClientID")));
    SDK_RegisterCallback = reinterpret_cast<RegisterCallbackFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapSDK_RegisterCallback")));
    SDK_UnRegisterCallback = reinterpret_cast<UnRegisterCallbackFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapSDK_UnregisterCallback")));

    SDK_RunCallbacks = reinterpret_cast<RunCallbacksFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapSDK_RunCallbacks")));
    SDK_AsyncAuthorize = reinterpret_cast<AsyncAuthorizeFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapUser_AsyncAuthorize_internal")));
    SDK_GetOpenID = reinterpret_cast<GetClientIDFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapUser_GetOpenID")));
    SDK_TapAppIsOwned = reinterpret_cast<TapAppIsOwnedFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapApps_IsOwned")));
    SDK_TapDLCShowStore = reinterpret_cast<TapDLCShowStoreFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapDLC_ShowStore")));
    SDK_TapDLCIsOwned = reinterpret_cast<TapDLCIsOwnedFunc>(FPlatformProcess::GetDllExport(SDKHandle, TEXT("TapDLC_IsOwned")));

    if (!SDK_Init || !SDK_Shutdown || !SDK_RestartApp || !SDK_GetClientID || 
        !SDK_RegisterCallback || !SDK_RunCallbacks || !SDK_AsyncAuthorize || !SDK_GetOpenID ||
        !SDK_TapAppIsOwned || !SDK_TapDLCShowStore || !SDK_TapDLCIsOwned)
    {
        UE_LOG(LogTemp, Error, TEXT("部分函数加载失败"));
        UnloadSDK();
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("SDK DLL 加载成功！"));
    return true;
}

void TapClientBridge::UnloadSDK()
{
    if (SDKHandle)
    {
        FPlatformProcess::FreeDllHandle(SDKHandle);
        SDKHandle = nullptr;
    }
}

uint32 TapClientBridge::InitSDK(ErrMsg& errMsg, const char* pubKey)
{
    return SDK_Init ? SDK_Init(&errMsg, pubKey) : 0;
}

bool TapClientBridge::Shutdown()
{
    return SDK_Shutdown ? SDK_Shutdown() : false;
}

bool TapClientBridge::RestartApp(const char* clientID)
{
    return SDK_RestartApp ? SDK_RestartApp(clientID) : false;
}

bool TapClientBridge::GetClientID(char* buffer)
{
    return SDK_GetClientID ? SDK_GetClientID(buffer) : false;
}

bool TapClientBridge::GetOpenID(char* buffer)
{
    return SDK_GetOpenID ? SDK_GetOpenID(buffer) : false;
}

void TapClientBridge::RegisterCallback(int callbackID, void (*callback)(int, void*))
{
    if (SDK_RegisterCallback)
    {
        SDK_RegisterCallback(callbackID, callback);
    }
}

void TapClientBridge::UnRegisterCallback(int callbackID, void (*callback)(int, void*))
{
    if (SDK_UnRegisterCallback)
    {
        SDK_UnRegisterCallback(callbackID, callback);
    }
}

void TapClientBridge::RunCallbacks()
{
    if (SDK_RunCallbacks)
    {
        SDK_RunCallbacks();
    }
}

uint32 TapClientBridge::AsyncAuthorize(const char* scopes, const char* response_type,
                                    const char* redirect_uri, const char* code_challenge, const char* state,
                                    const char* code_challenge_method, const char* version, const char* sdk_ua,
                                    const char* info)
{
    return SDK_AsyncAuthorize ? SDK_AsyncAuthorize(scopes, response_type, redirect_uri, code_challenge, state, code_challenge_method, version, sdk_ua,
                                    info) : 0;
}

bool TapClientBridge::TapAppIsOwned()
{
    return SDK_TapAppIsOwned ? SDK_TapAppIsOwned() : false;
}

bool TapClientBridge::TapDLCShowStore(const char* dlcId)
{
    return SDK_TapDLCShowStore ? SDK_TapDLCShowStore(dlcId) : false;
}

bool TapClientBridge::TapDLCIsOwned(const char* dlcId)
{
    return SDK_TapDLCIsOwned ? SDK_TapDLCIsOwned(dlcId) : false;
}
