// Copyright Epic Games, Inc. All Rights Reserved.

#include "TapBillboardModule.h"

#include "TapBootstrap.h"
#include "TapBootstrapModule.h"
#include "TapBillboardCommon.h"
#if PLATFORM_WINDOWS || PLATFORM_MAC
#include "PC/TapBillboardPC.h"
#elif PLATFORM_IOS || PLATFORM_ANDROID
#include "TapBillboard.h"
#endif

FTapBillboardPtr FTapBillboardModule::GetTapBillboardInterface()
{
	if (FTapBillboardModule* Module = FModuleManager::GetModulePtr<FTapBillboardModule>("TapBillboard"))
	{
		return Module->TapBillboard;
	}
	return nullptr;
}

void FTapBillboardModule::StartupModule()
{
#if PLATFORM_WINDOWS || PLATFORM_MAC
	auto TapBillboardPCPtr = MakeShared<FTapBillboardPC, ESPMode::ThreadSafe>();
	TapBillboardPCPtr->LoadBrowserClass();
	TapBillboard = TapBillboardPCPtr;
#elif PLATFORM_IOS || PLATFORM_ANDROID
	TapBillboard = MakeShared<FTapBillboard, ESPMode::ThreadSafe>();
#endif
	check(FModuleManager::GetModulePtr<FTapBootstrapModule>("TapBootstrap") != nullptr);
	BootstrapInitHandle = FTapBootstrap::OnBootstrapInit.AddRaw(this, &FTapBillboardModule::OnBootstrapInit);
}

void FTapBillboardModule::ShutdownModule()
{
	FTapBootstrap::OnBootstrapInit.Remove(BootstrapInitHandle);
	TapBillboard.Reset();
}

void FTapBillboardModule::OnBootstrapInit(const FTUConfig& Config)
{
	if (Config.BillboardConfig.IsValid())
	{
		if (TapBillboard)
		{
			TapBillboard->Init(Config);
		}
	}
}

IMPLEMENT_MODULE(FTapBillboardModule, TapBillboard)
