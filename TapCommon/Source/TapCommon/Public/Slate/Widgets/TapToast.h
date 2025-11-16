// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate/Styles/TapToastWidgetStyle.h"
#include "Widgets/SCompoundWidget.h"

struct FAnchors;
/**
 * Relevant to STapToast::Show only
 */
enum class ETapToastType : uint8
{
	Info,
	Success,
	Error,
	Warning,
	
	TapTap,

	Achievement,
};
class SImage;

class TAPCOMMON_API STapToast : public SCompoundWidget, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(STapToast)
	{}
	
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const FTapToastStyle* InStyle, float Offset);

	static TSharedPtr<STapToast> Show(ETapToastType Type, const FText& Message, const FString& WebUrl = FString());
	
	void UpdateToast(const FSlateBrush* IconBrush, const FText& Message, float Duration = 3.f, const FString& Url = FString());

	void UpdateIcon(const FSlateBrush* IconBrush);

	void UpdateIcon(UTexture2D* Texture);
	
	void Fadein();

	void Fadeout();
	
protected:
	TOptional<FSlateRenderTransform> GetToastRenderTransform() const;
	
	EActiveTimerReturnType OnToastTimer(double InCurrentTime, float InDeltaTime);

	EActiveTimerReturnType OnFadeoutTimer(double InCurrentTime, float InDeltaTime);

	void RefreshDurationTimer(float Duration);

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual FString GetReferencerName() const override;

	TSharedPtr<SImage> IconImage;
	TSharedPtr<STextBlock> MessageTextBlock;
	
	FSlateBrush Brush;
	TObjectPtr<UMaterialInstanceDynamic> GCMat = nullptr;

	FCurveSequence Anim;
	TSharedPtr<FActiveTimerHandle> ToastTimer;
	TSharedPtr<FActiveTimerHandle> FadeoutTimer;
	
	static TWeakPtr<STapToast> WhiteToast;
	static TWeakPtr<STapToast> BlackToast;
};
