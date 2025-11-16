// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/Input/DPadAxisInputConverter.h"
#include "Misc/EngineVersionComparison.h"


bool FDPadAxisInputConverter::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	if (Axis.IsValid() && InAnalogInputEvent.GetKey() == Axis)
	{
		float Val = InAnalogInputEvent.GetAnalogValue();

		FGamepadKeyNames::Type NewButton;
		if (!FMath::IsNearlyEqual(Val, Idle, IdleTolerance))
		{
			float Dis = FLT_MAX;
			static float Values[4] = {Left, Down, Right, Up};
			static FGamepadKeyNames::Type Buttons[4] = {FGamepadKeyNames::DPadLeft, FGamepadKeyNames::DPadDown, FGamepadKeyNames::DPadRight, FGamepadKeyNames::DPadUp};
		
			for (int32 i = 0; i < 4; ++i)
			{
				float NewDis = FMath::Abs(Values[i] - Val);
				if (NewDis < Dis)
				{
					Dis = NewDis;
					NewButton = Buttons[i];
				}
			}
		}

		if (NewButton != LastButton)
		{
#if UE_VERSION_NEWER_THAN(5, 1, 0)
			FPlatformUserId UserId = IPlatformInputDeviceMapper::Get().GetPrimaryPlatformUser();
			FInputDeviceId DeviceId = IPlatformInputDeviceMapper::Get().GetDefaultInputDevice();
			if (FKey(LastButton).IsValid())
			{
				SlateApp.OnControllerButtonReleased(LastButton, UserId, DeviceId, false);
			}
			LastButton = NewButton;
			if (FKey(NewButton).IsValid())
			{
				SlateApp.OnControllerButtonPressed(NewButton, UserId, DeviceId, false);
			}
#else
			if (FKey(LastButton).IsValid())
			{
				SlateApp.OnControllerButtonReleased(LastButton, ControllerId, false);
			}
			LastButton = NewButton;
			if (FKey(NewButton).IsValid())
			{
				SlateApp.OnControllerButtonPressed(NewButton, ControllerId, false);
			}
#endif
		}
		return true;
	}
	return false;
}
