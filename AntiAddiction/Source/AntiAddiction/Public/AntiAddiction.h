// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Slate/SlateGameResources.h"

class FAntiAddictionModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	TSharedPtr<FSlateStyleSet> Style;
};
