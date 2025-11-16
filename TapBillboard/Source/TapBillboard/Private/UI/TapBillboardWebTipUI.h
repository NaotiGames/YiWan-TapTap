// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "TapBillboardWebTipUI.generated.h"

/**
 * 
 */
UCLASS()
class TAPBILLBOARD_API UTapBillboardWebTipUI : public UUserWidget {
	GENERATED_BODY()

public:

	void ShowWait();
	void ShowLoadFail();
	void ShowLoadSuccess();

	FSimpleDelegate OnRefreshClick;
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void OnTipButtonClicked();

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TipLabel;

	UPROPERTY(Meta = (BindWidget))
	UImage* TipImage;
	
	UPROPERTY(Meta = (BindWidget))
	UButton* TipButton;

private:
	float AccumulatedTime = 0;
	float RotationSpeed = 0;

	FDateTime LastShowWaitTime;

	FTimerHandle TimerHandle;

	void InternalShowLoadFail();
};
