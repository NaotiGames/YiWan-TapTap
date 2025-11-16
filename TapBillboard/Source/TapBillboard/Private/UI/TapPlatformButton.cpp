// Fill out your copyright notice in the Description page of Project Settings.


#include "TapPlatformButton.h"
#include "Components/Button.h"
#include "TapUrlResourceLoader.h"

void UTapPlatformButton::UpdateButtonTexture(const FString& Url)
{
	FTapUrlResourceLoader::LoadImageTexture(Url, nullptr, FTapUrlResourceLoaderTextureDelegate::CreateUObject(this, &UTapPlatformButton::OnDownloadFinished));
}

FTimerHandle TimerHandle;

void UTapPlatformButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button->SetVisibility(ESlateVisibility::Hidden);
	Button->OnClicked.AddDynamic(this, &UTapPlatformButton::OnPCButtonClicked);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTapPlatformButton::ShowButton, 0.2);
}


void UTapPlatformButton::OnPCButtonClicked()
{
	OnClicked.ExecuteIfBound();
}

void UTapPlatformButton::OnDownloadFinished(UTexture2D* Texture) {
	if (IsValid(Texture)) {
		Button->SetVisibility(ESlateVisibility::Visible);
		Button->WidgetStyle.Normal.SetResourceObject(Texture);
		Button->WidgetStyle.Hovered.SetResourceObject(Texture);
		Button->WidgetStyle.Pressed.SetResourceObject(Texture);
		Button->SetStyle(Button->WidgetStyle);
	}
}

void UTapPlatformButton::ShowButton() {
	Button->SetVisibility(ESlateVisibility::Visible);
}

