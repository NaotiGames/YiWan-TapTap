// Copyright Epic Games, Inc. All Rights Reserved.

#include "AntiAddiction.h"

#include "Styling/SlateStyleRegistry.h"
#define TAP_STYLE_NAME_ANTI_ADDICTION "Tap.AntiAddiction"

void FAntiAddictionModule::StartupModule()
{
#if PLATFORM_DESKTOP
	Style = FSlateGameResources::New(TAP_STYLE_NAME_ANTI_ADDICTION, "/AntiAddiction/Desktop2/Styles", "/AntiAddiction/Desktop2/Styles");
#else
	Style = FSlateGameResources::New(TAP_STYLE_NAME_ANTI_ADDICTION, "/AntiAddiction/Mobile2/Styles", "/AntiAddiction/Mobile2/Styles");
#endif
	FSlateStyleRegistry::RegisterSlateStyle(*Style);
}

void FAntiAddictionModule::ShutdownModule()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(TAP_STYLE_NAME_ANTI_ADDICTION);
}
	
IMPLEMENT_MODULE(FAntiAddictionModule, AntiAddiction)