// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"

/**
 * Convert DPad Axis input to controller button input
 */
class TAPCOMMON_API FDPadAxisInputConverter : public IInputProcessor
{
public:
	FKey Axis;
	float Left = 0.f;
	float Down = 0.f;
	float Right = 0.f;
	float Up = 0.f;
	float Idle = 0.f;
	float IdleTolerance = 0.1f;

	int32 ControllerId = 0;
protected:
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {}

	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;

	FGamepadKeyNames::Type LastButton;
};
