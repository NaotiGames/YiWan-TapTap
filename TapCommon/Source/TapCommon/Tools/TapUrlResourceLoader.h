#pragma once

DECLARE_DELEGATE_OneParam(FTapUrlResourceLoaderProgressDelegate, float Progress);
DECLARE_DELEGATE_TwoParams(FTapUrlResourceLoaderCompletedDelegate, const TArray<uint8>& Buffer, bool bIsLocal);
DECLARE_DELEGATE_OneParam(FTapUrlResourceLoaderTextureDelegate, UTexture2D *Texture);
DECLARE_DELEGATE_OneParam(FTapUrlResourceLoaderSuccessDelegate, bool bIsSuccess);

class TAPCOMMON_API FTapUrlResourceLoader {
public:
	// OnCompleted 可能会调用多次
	static void LoadImageTexture(const FString& Url, UTexture2D *Placeholder, FTapUrlResourceLoaderTextureDelegate OnCompleted);
	static void LoadImageTexture(const FString& Url, UTexture2D *Placeholder, FTapUrlResourceLoaderProgressDelegate OnProgress, FTapUrlResourceLoaderTextureDelegate OnCompleted);
	
	static void LoadFileData(const FString& Url, FTapUrlResourceLoaderProgressDelegate OnProgress, FTapUrlResourceLoaderCompletedDelegate OnCompleted);
	static void PreLoadFileData(const FString& Url, float Timeout, FTapUrlResourceLoaderSuccessDelegate OnCompleted);
private:
	static void DownloadFileData(const FString& Url, float Timeout, const FTapUrlResourceLoaderProgressDelegate& OnProgress, const FTapUrlResourceLoaderCompletedDelegate& OnCompleted);
	static const FString GetFileName(const FString& Url);
	static const FString GetLocalCachePath();
	static const bool LoadLocalCache(TArray<uint8>& Result, const FString& Url);
	static const bool SaveLocalCache(TArrayView<const uint8> Result, const FString& Url);
};
