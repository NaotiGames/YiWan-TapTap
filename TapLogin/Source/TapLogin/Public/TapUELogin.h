#pragma once
#include "TUError.h"
#include "TULoginProfileModel.h"
#include "TUAccessToken.h"
#include "TUAuthResult.h"
#include "TULoginFriendResult.h"
#include "TULoginType.h"

class TAPLOGIN_API TapUELogin {
public:

	// 只能初始化一次
	static void Init(FTULoginConfig Config);

	// 未登录的话为nullptr
	static TSharedPtr<FTULoginProfileModel> GetProfile();

	// 获取失败，ModelPtr为空
	static void FetchProfile(TFunction<void(TSharedPtr<FTULoginProfileModel> ModelPtr, const FTUError& Error)> CallBack);

	static TSharedPtr<FTUAccessToken> GetAccessToken();

	static void Login(TFunction<void(const TUAuthResult& Result)> CallBack);
	
	static void Login(TArray<FString> Permissions, TFunction<void(const TUAuthResult& Result)> CallBack);

	static void Logout();

	// 仅支持移动端
	// static void QueryMutualList(FString Cursor, int Size, TFunction<void(TSharedPtr<FTULoginFriendResult> ModelPtr, const FTUError& Error)> CallBack);

private:
	friend class AAUChinaImpl;
	// 获取测试资格
	static void GetAntiAddictionCode(const FTUAccessToken& Token, TFunction<void(TSharedPtr<FString> CodePtr, const FTUError& Error)> CallBack);

	static void AppendPermission(const FString& Permission);
};
