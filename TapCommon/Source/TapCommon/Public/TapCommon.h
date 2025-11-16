// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Slate/SlateGameResources.h"
#include "TapCommon.generated.h"

DECLARE_DELEGATE_OneParam(FAsyncDownloadImage, UTexture2D*);
DECLARE_DELEGATE_OneParam(FAsyncDownloadBrush, TSharedPtr<FSlateDynamicImageBrush>);

USTRUCT()
struct FTapCustomMessage
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString MessageName;

	UPROPERTY()
	FString MessageContent;

	FTapCustomMessage(){}
	FTapCustomMessage(FString Name, FString Content)
	{
		MessageName = Name;
		MessageContent = Content;
	}
};

class TAPCOMMON_API FTapCommonModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FSlateStyleSet> Style;
	
	static inline FTapCommonModule &Get()
	{
		return FModuleManager::LoadModuleChecked<FTapCommonModule>("TapCommon");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("TapCommon");
	}

	DECLARE_MULTICAST_DELEGATE_OneParam(FBridgeCallback, const FString&);

	DECLARE_MULTICAST_DELEGATE_OneParam(FGetRegionCode, const bool);

	UPROPERTY(BlueprintAssignable, Category = "TapCommon")
	static FBridgeCallback OnBridgeCallback;

	UPROPERTY(BlueprintAssignable, Category= "TapCommon")
	static FGetRegionCode OnRegionCode;

	/** 是否安装 TapTap */
	DECLARE_MULTICAST_DELEGATE_OneParam(FIsTapTapInstalled,const bool);
	
	UPROPERTY(BlueprintAssignable,Category = "TapCommon")
	static FIsTapTapInstalled OnTapTapInstalled;

	/** 是否安装 Tap IO */
	DECLARE_MULTICAST_DELEGATE_OneParam(FIsTapGlobalInstalled,const bool);

	UPROPERTY(BlueprintAssignable,Category = "TapCommon")
	static FIsTapGlobalInstalled OnTapGlobalInstalled;

	/** 在 TapTap 更新 Game */
	DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateGameInTapTap,const bool);

	UPROPERTY(BlueprintAssignable,Category = "TapCommon")
	static FUpdateGameInTapTap OnUpdateGameInTapTap;

	/** 在 TapTap iO 更新 Game*/
	DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateGameInTapGlobal,const bool);

	UPROPERTY(BlueprintAssignable,Category = "TapCommon")
	static FUpdateGameInTapGlobal OnUpdateGameInTapGlobal;

	/** 在 TapTap 打开 Game */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOpenReviewInTapTap,const bool);

	UPROPERTY(BlueprintAssignable,Category = "TapCommon")
	static FOpenReviewInTapTap OnOpenReviewInTapTap;

	/** 在 Tap IO 打开 Game */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOpenReviewInTapGlobal,const bool);
	
	UPROPERTY(BlueprintAssignable,Category = "TapCommon")
	static FOpenReviewInTapGlobal OnOpenReviewInTapGlobal;

	static void AsyncDownloadImage(const FString& Url, const FAsyncDownloadImage& Callback);

	static void AsyncDownloadImage(const FString& Url, const FAsyncDownloadBrush& Callback);
	
	static void TapThrobberShowWait();
 
	static void TapThrobberShowWaitAndToast(const FString& Toast);
	 
	static void TapThrobberDismiss();
 
	static void TapThrobberShowToast(const FString& Toast, float TimeInterval = 3.f);

	static void TapToastThrobberShowToast(const FString& Toast, float TimeInterval = 3.f);

	static void OnTapThrobberRemoveSelf(const TSharedRef<class STapThrobber>& Throbber);

	static void OnTapToastThrobberRemoveSelf(const TSharedRef<class STapToastThrobber>& Throbber);
	TSharedPtr<class STapThrobber> TapThrobber;
	TSharedPtr<class STapToastThrobber> TapToastThrobber;
	
};
