// Fill out your copyright notice in the Description page of Project Settings.


#include "TapBillboardCommon.h"


FDelegateHandle FTapBillboardCommon::RegisterCustomLinkListener(
	const FOnCustomLinkClicked::FDelegate& LinkClickedCallback) {
	return OnCustomLinkClicked.Add(LinkClickedCallback);
}

void FTapBillboardCommon::UnregisterCustomLinkListener(FDelegateHandle Handle) {
	OnCustomLinkClicked.Remove(Handle);
}

void FTapBillboardCommon::RegisterOutputStateListener(const FAudioOutputStateChanged& OnAudioStateChanged) {
	OnAudioOutputStateChanged = OnAudioStateChanged;	
}

void FTapBillboardCommon::UnregisterOutputStateListener() {
	OnAudioOutputStateChanged.Unbind();
}
