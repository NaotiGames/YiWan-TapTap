#include "TUHttpRequest.h"
#include "Http.h"
#include "TUDeviceInfo.h"


TUHttpRequest::TUHttpRequest()
{
	Parameters = MakeShareable(new FJsonObject);
	PostUrlParameters = MakeShareable(new FJsonObject);
}

FString TUHttpRequest::GetFinalUrl()
{
	return FinalURL;
}

TMap<FString, FString> TUHttpRequest::CommonHeaders()
{
	TMap<FString, FString> CommonHeaders;
	if (Form == Default)
	{
		CommonHeaders.Add("Content-Type", "application/x-www-form-urlencoded");
	} else if (Form == Json)
	{
		CommonHeaders.Add("Content-Type", "application/json");
	}
	CommonHeaders.Add("User-Agent",  FString::Printf(TEXT("TapSDK_UE/%s/%s"), TapCommon_UE_VERSION, *TUDeviceInfo::GetPlatform()));
	return CommonHeaders;
}

TSharedPtr<FJsonObject> TUHttpRequest::CommonParameters()
{
	return MakeShareable(new FJsonObject);
}

// TDUHttpRequest::~TDUHttpRequest()
// {
// 	UE_LOG(LogTemp, Display, TEXT("TDUHttpRequest销毁了----"));
// }




