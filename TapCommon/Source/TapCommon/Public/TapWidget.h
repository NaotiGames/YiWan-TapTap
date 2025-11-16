// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TapWidget.generated.h"


UCLASS()
class TAPCOMMON_API UTapWidget : public UUserWidget
{
	GENERATED_BODY()
public:
#if ENGINE_MAJOR_VERSION == 4
	virtual void AddToScreen(ULocalPlayer* LocalPlayer, int32 ZOrder) override;

	virtual void RemoveFromParent() override;
#endif
};
