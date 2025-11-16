// Copyright Epic Games, Inc. All Rights Reserved.

#include "TapLogin.h"

#include "Slate/SlateGameResources.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FTapLoginModule"

#define TAP_STYLE_NAME_LOGIN "Tap.Login"

void FTapLoginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if PLATFORM_DESKTOP
	Style = FSlateGameResources::New(TAP_STYLE_NAME_LOGIN, "/TapLogin/Desktop2", "/TapLogin/Desktop2");
	Style->Set("taptap", FInlineTextImageStyle().SetImage( *Style->GetBrush("taptap_brush")).SetBaseline( 0 ));
#else
	Style = FSlateGameResources::New(TAP_STYLE_NAME_LOGIN, "/TapLogin/Mobile", "/TapLogin/Mobile");
#endif
	FSlateStyleRegistry::RegisterSlateStyle(*Style);
}

void FTapLoginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FSlateStyleRegistry::UnRegisterSlateStyle(TAP_STYLE_NAME_LOGIN);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTapLoginModule, TapLogin)
