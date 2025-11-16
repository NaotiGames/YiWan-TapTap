#include "TapBootstrap.h"
#include "TapBootstrapImpl.h"
#include "TapUECommon.h"
#include "TUHelper.h"
#include "TUMobileBridge.h"

#define BOOTSTRAP_SERVICE_CLZ "com.tapsdk.bootstrap.wrapper.TapBootstrapService"
#define BOOTSTRAP_SERVICE_IMPL "com.tapsdk.bootstrap.wrapper.TapBootstrapServiceImpl"


FTapBootstrapInit FTapBootstrap::OnBootstrapInit;
FSimpleMulticastDelegate FTapBootstrap::OnUserLogout;

void FTapBootstrap::Init(const FTUConfig& InConfig) {
#if ENGINE_MAJOR_VERSION > 4
	TapUECommon::setDurationStatisticsEnabled(false);
#endif
#if PLATFORM_IOS || PLATFORM_ANDROID	
	TUMobileBridge::Register(TEXT(BOOTSTRAP_SERVICE_CLZ),TEXT(BOOTSTRAP_SERVICE_IMPL));
#endif
	
	FTapBootstrapImpl::Get()->Init(InConfig);
	// 初始化防沉迷
	if (InConfig.TapAntiAddictionConfig.IsValid()) {
		TUHelper::InvokeNoReturnFunction("TapAntiAddictionReflection", "Init");
	}
	OnBootstrapInit.Broadcast(*FTUConfig::Get());
}

