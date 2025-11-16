#include "TapUpdateAndroidImpl.h"
#include "TapJNI.h"
#include "TapJNICallbackHelper.h"
#include "TUHelper.h"

#define TapUpdateUE "com/tds/TapUpdateUE"


FTapUpdateAndroidImpl::~FTapUpdateAndroidImpl() {
}

void FTapUpdateAndroidImpl::Init(const FString&  clientId, const FString&  clientToken){
	TapJNI::JNI JNI;
		auto ClassObject = JNI.FindClass(TapUpdateUE);
		JNI.CallStaticVoidMethod(ClassObject, "init",
								 "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V",
								 *JNI.GetActivity(), *JNI.ToJavaString(clientId), *JNI.ToJavaString(clientToken));
}

void FTapUpdateAndroidImpl::UpdateGame(FSimpleDelegate OnCancel) {
		TapJNI::JNI JNI;
		auto ClassObject = JNI.FindClass(TapUpdateUE);
		JNI.CallStaticVoidMethod(ClassObject, "updateGame",
								 "(Landroid/app/Activity;I)V",
								 *JNI.GetActivity(), FTapJNICallbackHelper::AddCallBack(OnCancel));
}

#ifdef __cplusplus
extern "C" {
#endif

	__attribute__((visibility("default"))) void Java_com_tds_TapUpdateUE_onUpdateCancel(
		JNIEnv* jenv, jclass thiz, int callBackID) {
		auto CallBackPtr = FTapJNICallbackHelper::FindCallBack<FSimpleDelegate>(callBackID);
		if (CallBackPtr == nullptr) {
			return;
		}
		TUHelper::PerformOnGameThread([=]() {
			CallBackPtr->ExecuteIfBound();
			FTapJNICallbackHelper::RemoveCallBack(callBackID);
		});
	}


#ifdef __cplusplus
}
#endif

