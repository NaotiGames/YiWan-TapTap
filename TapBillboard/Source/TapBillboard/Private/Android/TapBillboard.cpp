// Fill out your copyright notice in the Description page of Project Settings.


#include "TapBillboard.h"

#include "StringHelpers.h"
#include "TapJNI.h"
#include "TapJNICallbackHelper.h"
#include "TUHelper.h"
#include "TUJsonHelper.h"

#define TapBillboardUE "com/tds/TapBillboardUE"

FTapBillboard *TapBillboardPtr = nullptr;

FTapBillboard::FTapBillboard()
{
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBillboardUE);
	JNI.CallStaticVoidMethod(ClassObject, "registerListeners", "()V");
	TapBillboardPtr = this;
}

FTapBillboard::~FTapBillboard() {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBillboardUE);
	JNI.CallStaticVoidMethod(ClassObject, "unregisterListeners", "()V");
	if (TapBillboardPtr == this) {
		TapBillboardPtr = nullptr;
	}
}

void FTapBillboard::Init(const FTUConfig& InConfig)
{
	if (!InConfig.BillboardConfig.IsValid()) {
		return;
	}

	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBillboardUE);
	auto ClientID = JNI.ToJavaString(InConfig.ClientID);
	auto ClientToken = JNI.ToJavaString(InConfig.ClientToken);
	auto ServerURL = JNI.ToJavaString(InConfig.ServerURL);
	auto BillboardUrl = JNI.ToJavaString(InConfig.BillboardConfig->BillboardUrl);
	TArray<FString> DimensionsUE;
	for (TTuple<FString, FString>& T : InConfig.BillboardConfig->Dimensions)
	{
		DimensionsUE.Add(T.Key);
		DimensionsUE.Add(T.Value);
	}
	auto Dimensions = JNI.GetStringArray(DimensionsUE);
	JNI.CallStaticVoidMethod(ClassObject, "init",
							 "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;I)V",
							 *JNI.GetActivity(), *ClientID, *ClientToken, *ServerURL, *BillboardUrl, *Dimensions, (InConfig.RegionType == ERegionType::CN ? 0 : 1));

}

void FTapBillboard::OpenPanel(const FSimpleDelegate& OnSuccess, const FTapFailed& OnFailed, const FSimpleDelegate& OnClose)
{
	auto OnSuccessCopy = OnSuccess;
	auto OnFailedCopy = OnFailed;
	auto OnCloseCopy = OnClose;
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBillboardUE);
	JNI.CallStaticVoidMethod(ClassObject, "openPanel", "(Landroid/app/Activity;I)V",
		*JNI.GetActivity(), FTapJNICallbackHelper::AddCallBack(OnSuccessCopy, OnFailedCopy, OnCloseCopy));
}

void FTapBillboard::OpenSplashPanel(const FSimpleDelegate& OnSuccess, const FTapFailed& OnFailed, const FSimpleDelegate& OnClose)
{
	auto OnSuccessCopy = OnSuccess;
	auto OnFailedCopy = OnFailed;
	auto OnCloseCopy = OnClose;
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBillboardUE);
	JNI.CallStaticVoidMethod(ClassObject, "openSplashPanel", "(Landroid/app/Activity;I)V",
		*JNI.GetActivity(), FTapJNICallbackHelper::AddCallBack(OnSuccessCopy, OnFailedCopy, OnCloseCopy));
}

void FTapBillboard::ClosePanel() {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBillboardUE);
	JNI.CallStaticVoidMethod(ClassObject, "closePanel", "()V");
}

void FTapBillboard::CloseSplashPanel()
{
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBillboardUE);
	JNI.CallStaticVoidMethod(ClassObject, "closeSplashPanel", "()V");
}

void FTapBillboard::StartFetchMarqueeData()
{
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBillboardUE);
	JNI.CallStaticVoidMethod(ClassObject, "startFetchMarqueeData", "(Landroid/app/Activity;)V",
		*JNI.GetActivity());
}

void FTapBillboard::StopFetchMarqueeData(bool bCloseNow)
{
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBillboardUE);
	JNI.CallStaticVoidMethod(ClassObject, "stopFetchMarqueeData", "(Z)V", bCloseNow);
}

void FTapBillboard::HandleCustomLinkClickedEvent(const FString& Url) {
	OnCustomLinkClicked.Broadcast(Url);
}

void FTapBillboard::HandleAudioStatusChangedEvent(bool bNewPlaying) {
	OnAudioOutputStateChanged.ExecuteIfBound(bNewPlaying);
}

void FTapBillboard::HandleMarqueeShowEvent(const FString& MarqueeStr, const FString& ConfigStr) {
	OnBillboardMarqueeShow.Broadcast(FTapBillboardMarquee(TUJsonHelper::GetJsonObject(MarqueeStr)), FTapBillboardMarqueeConfig(TUJsonHelper::GetJsonObject(ConfigStr)));
}

void FTapBillboard::HandleMarqueeCloseEvent() {
	OnBillboardMarqueeClose.Broadcast();
}

void FTapBillboard::GetBadgeDetails(const FTapBadgeDetailsResult& OnSuccess, const FTapFailed& OnFailed) {
	auto OnSuccessCopy = OnSuccess;
	auto OnFailedCopy = OnFailed;
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBillboardUE);
	JNI.CallStaticVoidMethod(ClassObject, "getBadgeDetails", "(I)V",
		FTapJNICallbackHelper::AddCallBack(OnSuccessCopy, OnFailedCopy));
}

#ifdef __cplusplus
extern "C" {
#endif

	__attribute__((visibility("default"))) void Java_com_tds_TapBillboardUE_NotifyNavigateOpenFailed (JNIEnv* jenv, jclass thiz, int callBackID, int code, jstring message) {
		auto CallBackPtr = FTapJNICallbackHelper::FindCallBack<FTapFailed>(callBackID, 1);
		if (CallBackPtr == nullptr) {
			return;
		}
		TapJNI::JNI JNI(jenv);
		FTUError Error(code, JNI.GetFStringFromParam(message));
		TUHelper::PerformOnGameThread([=]() {
			CallBackPtr->ExecuteIfBound(Error);
			FTapJNICallbackHelper::RemoveCallBack(callBackID);
		});
	}

	__attribute__((visibility("default"))) void Java_com_tds_TapBillboardUE_NotifyNavigateOpenSuccess (JNIEnv* jenv, jclass thiz, int callBackID) {
		auto CallBackPtr = FTapJNICallbackHelper::FindCallBack<FSimpleDelegate>(callBackID, 0);
		if (CallBackPtr == nullptr) {
			return;
		}
		TUHelper::PerformOnGameThread([=]() {
			CallBackPtr->ExecuteIfBound();
		});
	}

	__attribute__((visibility("default"))) void Java_com_tds_TapBillboardUE_NotifyNavigateClosed (JNIEnv* jenv, jclass thiz, int callBackID) {
		auto CallBackPtr = FTapJNICallbackHelper::FindCallBack<FSimpleDelegate>(callBackID, 2);
		if (CallBackPtr == nullptr) {
			return;
		}
		TUHelper::PerformOnGameThread([=]() {
			CallBackPtr->ExecuteIfBound();
			FTapJNICallbackHelper::RemoveCallBack(callBackID);
		});
	}

	__attribute__((visibility("default"))) void Java_com_tds_TapBillboardUE_GetBadgeDetailsFailed (JNIEnv* jenv, jclass thiz, int callBackID, int code, jstring message) {
		auto CallBackPtr = FTapJNICallbackHelper::FindCallBack<FTapFailed>(callBackID, 1);
		if (CallBackPtr == nullptr) {
			return;
		}
		TapJNI::JNI JNI(jenv);
		FTUError Error(code, JNI.GetFStringFromParam(message));
		TUHelper::PerformOnGameThread([=]() {
			CallBackPtr->ExecuteIfBound(Error);
			FTapJNICallbackHelper::RemoveCallBack(callBackID);
		});
	}

	__attribute__((visibility("default"))) void Java_com_tds_TapBillboardUE_GetBadgeDetailsSuccess (JNIEnv* jenv, jclass thiz, int callBackID, int showRedDot, jstring closeButtonImg) {
		auto CallBackPtr = FTapJNICallbackHelper::FindCallBack<FTapBadgeDetailsResult>(callBackID, 1);
		if (CallBackPtr == nullptr) {
			return;
		}
		TapJNI::JNI JNI(jenv);
		FBadgeDetails BadgeDetails;
		BadgeDetails.show_red_dot = showRedDot;
		BadgeDetails.close_button_img = JNI.GetFStringFromParam(closeButtonImg);
		TUHelper::PerformOnGameThread([=]() {
			CallBackPtr->ExecuteIfBound(BadgeDetails);
			FTapJNICallbackHelper::RemoveCallBack(callBackID);
		});
	}

	__attribute__((visibility("default"))) void Java_com_tds_TapBillboardUE_NotifyCustomUrl (JNIEnv* jenv, jclass thiz, jstring Url) {
		TapJNI::JNI JNI(jenv);
		FString UrlUE = JNI.GetFStringFromParam(Url);
		TUHelper::PerformOnGameThread([=](){ TapBillboardPtr->HandleCustomLinkClickedEvent(UrlUE); });
	}

	__attribute__((visibility("default"))) void Java_com_tds_TapBillboardUE_NotifyAudioStatusChanged (JNIEnv* jenv, jclass thiz, bool newPlaying) {
		TapJNI::JNI JNI(jenv);
		TUHelper::PerformOnGameThread([=](){ TapBillboardPtr->HandleAudioStatusChangedEvent(newPlaying); });
	}

	__attribute__((visibility("default"))) void Java_com_tds_TapBillboardUE_NotifyMarqueeShow (JNIEnv* jenv, jclass thiz, jstring marquee, jstring config) {
		TapJNI::JNI JNI(jenv);
		FString Marquee = JNI.GetFStringFromParam(marquee);
		FString Config = JNI.GetFStringFromParam(config);
		TUHelper::PerformOnGameThread([=](){ TapBillboardPtr->HandleMarqueeShowEvent(Marquee, Config); });
	}

	__attribute__((visibility("default"))) void Java_com_tds_TapBillboardUE_NotifyMarqueeClose (JNIEnv* jenv, jclass thiz) {
		TapJNI::JNI JNI(jenv);
		TUHelper::PerformOnGameThread([=](){ TapBillboardPtr->HandleMarqueeCloseEvent(); });
	}


#ifdef __cplusplus
}
#endif