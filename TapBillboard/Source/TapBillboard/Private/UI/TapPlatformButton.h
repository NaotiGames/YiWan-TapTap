// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TapPlatformButton.generated.h"

class UButton;

UCLASS(Abstract)
class TAPBILLBOARD_API UTapPlatformButton : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateButtonTexture(const FString& Url);
	
	FSimpleDelegate OnClicked;

protected:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void OnPCButtonClicked();

	void OnDownloadFinished(UTexture2D* Texture);

	void ShowButton();
	
	UPROPERTY(Meta = (BindWidget))
	UButton* Button;
	friend class FPCButtonHandle;
};

