// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TapControllerTypes.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTapControllerRunningStateChanged, bool bRunning);

UENUM()
enum class ETapControllerType : uint8
{
	PS,
	XBox,
	Switch,
};

USTRUCT()
struct FTapControllerConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float DeadZone = 0.1f;
	UPROPERTY(EditAnywhere)
	float CursorMaxSpeed = 1500.f;
	UPROPERTY(EditAnywhere)
	float ScrollMaxSpeed = 80.f;

	const static FTapControllerConfig DefaultConfig;
};
