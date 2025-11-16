#include "AAUChinaAndroidImpl.h"
#include "AntiAddictionUE.h"
#include "TapJNI.h"
#include "TapJNICallbackHelper.h"
#include "TUHelper.h"
#include "Server/AAUHelper.h"

#define JavaAntiAddictionUE "com/tds/AntiAddictionUE"

TFunction<void(bool Status)> AAUChinaAndroidImpl::OnCheckPayLimitSuccess;
TFunction<void(bool Status)> AAUChinaAndroidImpl::OnSubmitPayResultSuccess;
TFunction<void(const FString& Msg)> AAUChinaAndroidImpl::OnCheckPayLimitFail;
TFunction<void(const FString& Msg)> AAUChinaAndroidImpl::OnSubmitPayResultFail;

AAUChinaAndroidImpl::~AAUChinaAndroidImpl() {
}

AAUChinaAndroidImpl::AAUChinaAndroidImpl() {
}

void AAUChinaAndroidImpl::StartupWithTapTap(const FString& UserId)
{
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(JavaAntiAddictionUE);
	JNI.CallStaticVoidMethod(ClassObject, "startupWithTapTap",
							 "(Landroid/app/Activity;Ljava/lang/String;)V", *JNI.GetActivity(),
							 *JNI.ToJavaString(UserId));
}

void AAUChinaAndroidImpl::SetTestEnv(bool Enable) {
	bTestEnvEnable = Enable;
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(JavaAntiAddictionUE);
	JNI.CallStaticVoidMethod(ClassObject, "setTestEnv",
							 "(Landroid/app/Activity;Z)V", *JNI.GetActivity(), Enable);
}

void AAUChinaAndroidImpl::Exit() {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(JavaAntiAddictionUE);
	JNI.CallStaticVoidMethod(ClassObject, "exit", "()V");
}

EAAUAgeLimit AAUChinaAndroidImpl::GetAgeRange() {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(JavaAntiAddictionUE);
	int Num = JNI.CallStaticIntMethod(ClassObject, "getAgeRange",
							 "()I");
	return AAUHelper::MakeAgeLimit(Num);
}

int AAUChinaAndroidImpl::GetRemainingTime() {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(JavaAntiAddictionUE);
	return JNI.CallStaticIntMethod(ClassObject, "getRemainingTime",
							 "()I");
}

void AAUChinaAndroidImpl::InitImpl(const FAAUConfig& _Config) {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(JavaAntiAddictionUE);
	JNI.CallStaticVoidMethod(ClassObject, "init",
							 "(Landroid/app/Activity;Ljava/lang/String;ZZ)V", *JNI.GetActivity(),
							 *JNI.ToJavaString(_Config.ClientID), _Config.ShowSwitchAccount, _Config.UseAgeRange);
}

void AAUChinaAndroidImpl::EnterGame() {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(JavaAntiAddictionUE);
	JNI.CallStaticVoidMethod(ClassObject, "enterGame", "()V");
}

void AAUChinaAndroidImpl::LeaveGame() {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(JavaAntiAddictionUE);
	JNI.CallStaticVoidMethod(ClassObject, "leaveGame", "()V");
}

void AAUChinaAndroidImpl::CheckPayLimit(int Amount, TFunction<void(bool Status)> CallBack,
                                        TFunction<void(const FString& Msg)> FailureHandler) {
	OnCheckPayLimitSuccess = CallBack;
	OnCheckPayLimitFail = FailureHandler;
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(JavaAntiAddictionUE);
	JNI.CallStaticVoidMethod(ClassObject, "checkPayLimit",
							 "(Landroid/app/Activity;I)V", *JNI.GetActivity(), Amount);
	
}

void AAUChinaAndroidImpl::SubmitPayResult(int Amount, TFunction<void(bool Success)> CallBack,
                                          TFunction<void(const FString& Msg)> FailureHandler) {
	OnSubmitPayResultSuccess = CallBack;
	OnSubmitPayResultFail = FailureHandler;
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(JavaAntiAddictionUE);
	JNI.CallStaticVoidMethod(ClassObject, "submitPayResult",
							 "(I)V", Amount);
}

FString AAUChinaAndroidImpl::CurrentToken() {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(JavaAntiAddictionUE);
	auto Result = JNI.CallStaticObjectMethod(ClassObject, "currentToken", "()Ljava/lang/String;");
	return JNI.GetFString(Result);
}



#ifdef __cplusplus
extern "C" {
#endif

	JNIEXPORT void Java_com_tds_AntiAddictionUE_onCheckPayLimitSuccess(JNIEnv* jenv, jclass thiz, bool status) {
		TUHelper::PerformOnGameThread([=]() {
			if (AAUChinaAndroidImpl::OnCheckPayLimitSuccess) {
				AAUChinaAndroidImpl::OnCheckPayLimitSuccess(status);
				AAUChinaAndroidImpl::OnCheckPayLimitSuccess = nullptr;
				AAUChinaAndroidImpl::OnCheckPayLimitFail = nullptr;
			}
		});
	}

	JNIEXPORT void Java_com_tds_AntiAddictionUE_onCheckPayLimitFail(JNIEnv* jenv, jclass thiz, jstring msg) {
		TapJNI::JNI JNI(jenv);
		FString MsgUE = JNI.GetFStringFromParam(msg);
		TUHelper::PerformOnGameThread([=]() {
			if (AAUChinaAndroidImpl::OnCheckPayLimitFail) {
				AAUChinaAndroidImpl::OnCheckPayLimitFail(MsgUE);
				AAUChinaAndroidImpl::OnCheckPayLimitSuccess = nullptr;
				AAUChinaAndroidImpl::OnCheckPayLimitFail = nullptr;
			}
		});
	}

	JNIEXPORT void Java_com_tds_AntiAddictionUE_onSubmitPayResultSuccess(JNIEnv* jenv, jclass thiz, bool status) {
		TUHelper::PerformOnGameThread([=]() {
			if (AAUChinaAndroidImpl::OnSubmitPayResultSuccess) {
				AAUChinaAndroidImpl::OnSubmitPayResultSuccess(status);
				AAUChinaAndroidImpl::OnSubmitPayResultSuccess = nullptr;
				AAUChinaAndroidImpl::OnSubmitPayResultFail = nullptr;
			}
		});
	}

	JNIEXPORT void Java_com_tds_AntiAddictionUE_onSubmitPayResultFail(JNIEnv* jenv, jclass thiz, jstring msg) {
		TapJNI::JNI JNI(jenv);
		FString MsgUE = JNI.GetFStringFromParam(msg);
		TUHelper::PerformOnGameThread([=]() {
			if (AAUChinaAndroidImpl::OnSubmitPayResultFail) {
				AAUChinaAndroidImpl::OnSubmitPayResultFail(MsgUE);
				AAUChinaAndroidImpl::OnSubmitPayResultSuccess = nullptr;
				AAUChinaAndroidImpl::OnSubmitPayResultFail = nullptr;
			}
		});
	}

	JNIEXPORT void Java_com_tds_AntiAddictionUE_onCallBack(JNIEnv* jenv, jclass thiz, int code, jstring msg) {
		TapJNI::JNI JNI(jenv);
		FString MsgUE = JNI.GetFStringFromParam(msg);
		TUHelper::PerformOnGameThread([=]() {
			AntiAddictionUE::OnCallBack.ExecuteIfBound((AntiAddictionUE::ResultHandlerCode)code, MsgUE);
		});
	}


#ifdef __cplusplus
}
#endif
