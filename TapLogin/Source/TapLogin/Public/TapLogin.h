// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

class FSlateStyleSet;

class FTapLoginModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FSlateStyleSet> Style;
};
