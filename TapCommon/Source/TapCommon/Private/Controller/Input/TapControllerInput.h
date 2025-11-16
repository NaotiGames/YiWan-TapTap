// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/AnalogCursor.h"

/**
 * 
 */
class TAPCOMMON_API FTapControllerInput : public FAnalogCursor
{
public:
	FTapControllerInput();
	float ScrollSpeed = 80.f;
protected:
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;

	virtual void UpdateCursorPosition(FSlateApplication& SlateApp, TSharedRef<FSlateUser> SlateUser, const FVector2D& NewPosition, bool bForce) override;
	bool TapUINavigationImpl(EUINavigation Direction, TSharedRef<FSlateUser> SlateUser);
	
	FVector2D AnalogValue = FVector2D(ForceInitToZero);
};
