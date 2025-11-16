#include "TapUrlResourceLoader.h"

#include "HttpModule.h"
#include "ImageUtils.h"
#include "TUDebuger.h"
#include "TUHelper.h"
#include "Interfaces/IHttpResponse.h"

const FString FTapUrlResourceLoader::GetFileName(const FString& Url) {
	return FMD5::HashAnsiString(*Url);
}

const FString FTapUrlResourceLoader::GetLocalCachePath() {
	return FPaths::ProjectSavedDir() / TEXT("tds_cache/");
}

static FRWLock CriticalSection;

const bool FTapUrlResourceLoader::LoadLocalCache(TArray<uint8>& Result, const FString& Url) {
	FReadScopeLock ScopeLock(CriticalSection);
	FString FilePath = GetLocalCachePath() + GetFileName(Url);
	return FFileHelper::LoadFileToArray(Result, *FilePath);
}

const bool FTapUrlResourceLoader::SaveLocalCache(TArrayView<const uint8> Result, const FString& Url) {
	FWriteScopeLock ScopeLock(CriticalSection);
	FString FilePath = GetLocalCachePath() + GetFileName(Url);
	return FFileHelper::SaveArrayToFile(Result, *FilePath);
}

void FTapUrlResourceLoader::LoadImageTexture(const FString& Url, UTexture2D* Placeholder,
                                             FTapUrlResourceLoaderTextureDelegate OnCompleted) {
	LoadImageTexture(Url, Placeholder, FTapUrlResourceLoaderProgressDelegate(), OnCompleted);
}

void FTapUrlResourceLoader::LoadImageTexture(const FString& Url, UTexture2D* Placeholder,
                                             FTapUrlResourceLoaderProgressDelegate OnProgress,
                                             FTapUrlResourceLoaderTextureDelegate OnCompleted) {
	if (IsValid(Placeholder)) {
		Placeholder->AddToRoot();
	}
	AsyncTask(ENamedThreads::AnyThread, [=]() {
		TArray<uint8> Result;
		if (LoadLocalCache(Result, Url)) {
			OnProgress.ExecuteIfBound(1);
			UTexture2D* Tex = FImageUtils::ImportBufferAsTexture2D(Result);
			if (IsValid(Tex)) {
				Tex->AddToRoot();
			}
			TUHelper::PerformOnGameThread([=]() {
				if (IsValid(Tex)) {
					OnCompleted.ExecuteIfBound(Tex);
					Tex->RemoveFromRoot();
					if (IsValid(Placeholder)) {
						Placeholder->RemoveFromRoot();
					}
				}
				else {
					if (IsValid(Placeholder)) {
						OnCompleted.ExecuteIfBound(Placeholder);
						Placeholder->RemoveFromRoot();
					}
				}
			});
		}
		else {
			if (IsValid(Placeholder)) {
				TUHelper::PerformOnGameThread([=]() {
					OnCompleted.ExecuteIfBound(Placeholder);
					Placeholder->RemoveFromRoot();
				});
			}

			DownloadFileData(Url, 3, OnProgress, FTapUrlResourceLoaderCompletedDelegate::CreateLambda(
				                 [=](const TArray<uint8>& Buffer, bool bIsLocal) {
					                 if (Buffer.Num() > 0) {
						                 AsyncTask(ENamedThreads::AnyThread, [=]() {
							                 UTexture2D* Tex = FImageUtils::ImportBufferAsTexture2D(Buffer);
							                 if (IsValid(Tex)) {
								                 Tex->AddToRoot();
								                 TUHelper::PerformOnGameThread([=]() {
									                 OnCompleted.ExecuteIfBound(Tex);
									                 Tex->RemoveFromRoot();
								                 });
							                 }
						                 });
					                 }
				                 }));
		}
	});
}

void FTapUrlResourceLoader::LoadFileData(const FString& Url, FTapUrlResourceLoaderProgressDelegate OnProgress,
                                         FTapUrlResourceLoaderCompletedDelegate OnCompleted) {
	TArray<uint8> Result;
	if (LoadLocalCache(Result, Url)) {
		OnProgress.ExecuteIfBound(1);
		OnCompleted.ExecuteIfBound(Result, true);
	}
	else {
		DownloadFileData(Url, 60, OnProgress, OnCompleted);
	}
}

void FTapUrlResourceLoader::PreLoadFileData(const FString& Url, float Timeout, FTapUrlResourceLoaderSuccessDelegate OnCompleted) {
	FString FilePath = GetLocalCachePath() + GetFileName(Url);
	if (IFileManager::Get().FileExists(*FilePath)) {
		OnCompleted.ExecuteIfBound(true);
		return;
	}
	DownloadFileData(Url, Timeout, FTapUrlResourceLoaderProgressDelegate(), FTapUrlResourceLoaderCompletedDelegate::CreateLambda(
		                 [=](const TArray<uint8>& Buffer, bool bIsLocal) {
			                 OnCompleted.ExecuteIfBound(Buffer.Num() > 0);
		                 }));
}

void FTapUrlResourceLoader::DownloadFileData(const FString& Url, float Timeout,
                                             const FTapUrlResourceLoaderProgressDelegate& OnProgress,
                                             const FTapUrlResourceLoaderCompletedDelegate& OnCompleted) {
	if (Url.IsEmpty()) {
		OnCompleted.ExecuteIfBound(TArray<uint8>(), false);
		return;
	}
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetTimeout(Timeout);
	if (OnProgress.IsBound()) {
		HttpRequest->OnRequestProgress().BindLambda([=](FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived) {
			TUHelper::PerformOnGameThread([=]() {
				if (!Request.IsValid() || !Request->GetResponse().IsValid()) {
					OnProgress.ExecuteIfBound(0);
					TUDebuger::WarningLog(TEXT("未获取到下载进度对象"));
					return;
				}
				int32 TotalSize = 0;
				// TUDebuger::DisplayLog(FString::Printf(TEXT("下载进度内容长度：%d"), BytesReceived));
				LexFromString(TotalSize, *Request->GetResponse()->GetHeader("content-length"));
				// TUDebuger::DisplayLog(FString::Printf(TEXT("下载进度内容总长度：%d"), TotalSize));

				if (TotalSize == 0) {
					OnProgress.ExecuteIfBound(0);
					TUDebuger::WarningLog(TEXT("未获取到下载内容长度"));
				}
				else {
					OnProgress.ExecuteIfBound(BytesReceived / (float)TotalSize);
				}
			});
		});
	}
	if (OnCompleted.IsBound()) {
		HttpRequest->OnProcessRequestComplete().BindLambda(
			[=](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully) {
				if (bConnectedSuccessfully && Response.IsValid() &&
					EHttpResponseCodes::IsOk(Response->GetResponseCode())) {
					auto Buffer = Response->GetContent();
					if (Buffer.Num() > 0) {
						SaveLocalCache(Buffer, Url);
					}
					TUHelper::PerformOnGameThread([=]() {
						OnCompleted.ExecuteIfBound(Buffer, false);
					});
				}
				else {
					TUHelper::PerformOnGameThread([=]() {
						OnCompleted.ExecuteIfBound(TArray<uint8>(), false);
					});
				}
			});
	}
	if (!HttpRequest->ProcessRequest()) {
		OnCompleted.ExecuteIfBound(TArray<uint8>(), false);
	}
}
