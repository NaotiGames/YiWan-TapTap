#include "AAURegionConfig.h"

#include "AAUImpl.h"
#include "TUDebuger.h"


class AAURegionConfigCN: public AAURegionConfig
{
public:
	virtual FString BaseHost() {
		return "https://tds-tapsdk.cn.tapapis.com";
	}
};


TSharedPtr<AAURegionConfig> AAURegionConfig::Instance = nullptr;

FString AAURegionConfig::RealNameUrl()
{
	return BaseHost() + "/real-name/v2/";
}

FString AAURegionConfig::AntiAddictionUrl()
{
	return BaseHost() + "/anti-addiction/v2/";
}


TSharedPtr<AAURegionConfig>& AAURegionConfig::Get() {
	if (!Instance.IsValid()) {
		Instance = MakeShareable(new AAURegionConfigCN);
	}
	return Instance;
}