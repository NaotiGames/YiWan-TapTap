// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PC/TapBillboardPC.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/ScrollBox.h"
#include "Components/Spacer.h"
#include "TapMarqueeItem.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"
#include "Components/CanvasPanelSlot.h"
#include "TapBillboardMarqueeUI.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TAPBILLBOARD_API UTapBillboardMarquee : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateStyle(const FAnnouncementStyleData& StyleData);

	void StartAnnouncementLoop();
	
	void StopAnnouncementLoop();

	FORCEINLINE bool IsLoopRunning() const
	{
		return CurrentLoop != nullptr;
	}

	TSubclassOf<UTapMarqueeItem> GetItemClass() const
	{
		return ItemClass;
	}

	void UpdateScrollSpeed(float NewSpeed);
	
protected:
	bool NextAnnouncement();
	
	void ShowMarqueeItem(const FAnnouncementDetailData& DetailData, bool bMarkReadWhenFinished);
	
	void OnItemClipped(UTapMarqueeItem* Item);

	void OnOrientationChanged(int32 Orientation);
	
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY()
	UFont* DFont;
	
	void UpdateFont(UFont* Font);
	void DownloadFont(const TArray<uint8>& Buffer, bool bIsLocal);
	
	TSharedPtr<FAnnouncementDetailData> CurrentLoop;

	UPROPERTY()
	TArray<int64> ScrollingIds;

	UPROPERTY()
	int32 LeftLoopCount;

	UPROPERTY(Meta = (BindWidget))
	UImage* BGLeft;
	UPROPERTY(Meta = (BindWidget))
	UImage* BGWidget;
	UPROPERTY(Meta = (BindWidget))
	UImage* BGRight;
	
	UPROPERTY(Meta = (BindWidget))
	UScrollBox* ItemPanel;

	UPROPERTY(Meta = (BindWidget))
	UOverlay* LayoutOverlay;

	UPROPERTY(Meta = (BindWidget))
	USpacer* PlaceHolder;
	UPROPERTY(Meta = (BindWidget))
	USpacer* PlaceHolderRight;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTapMarqueeItem> ItemClass;
	
	UPROPERTY(EditDefaultsOnly)
	float ScrollSpeed = 60.f;
	
	UPROPERTY()
	float Offset;
	
	FTimerHandle DelayTimer;
	
	TSharedPtr<class FTapBillboardPC, ESPMode::ThreadSafe>  BillboardInterface;
};

 



