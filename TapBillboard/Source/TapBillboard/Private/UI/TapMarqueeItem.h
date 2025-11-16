// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"
#include "TapMarqueeItem.generated.h"

class UImage;
class UTextBlock;
struct FAnnouncementStyleData;
DECLARE_DELEGATE_OneParam(FMarqueeCallback, UTapMarqueeItem*);

#define NULL_ID (-1)

UCLASS(Abstract)
class TAPBILLBOARD_API UTapMarqueeItem : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateItem(const FText& Content, float InStartupOffset, int64 Id, bool bMarkReadWhenFinished = false);
	
	void UpdateStyle(const FAnnouncementStyleData& StyleData);

	// void UpdateFont(const FAnnouncementStyleData& StyleData);
	
	FMarqueeCallback OutsideOfParent;

	UPROPERTY()
	int64 MarkReadID = NULL_ID;

	void UpdateIconTexture(UTexture2D *Texture);
	void UpdateFont(UFont *Font);
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeOnInitialized() override;
	
	UPROPERTY(Meta = (BindWidget))
	UWidget* MarginWidget;
	
	UPROPERTY(Meta = (BindWidget))
	UImage* Icon;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ContentLabel;

	FTimerHandle FindIconTimer;

	UPROPERTY()
	bool bMarkRead;

};
