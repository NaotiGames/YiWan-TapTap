// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate/WidgetTransform.h"
#include "Widgets/SCompoundWidget.h"

class STapToastThrobber;

DECLARE_DELEGATE_OneParam(FOnTapToastThrobberRemoveSelf, const TSharedRef<STapToastThrobber>&);

/**
 * 活动等待控件
 */
class TAPCOMMON_API STapToastThrobber : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STapToastThrobber)
	: _HasThrobber(true)
	, _Content(FText::GetEmpty())
	, _TimeInterval(0.f)
	, _bBlock(false)
	{}

	/** 是否有旋转等待指示器 */
	SLATE_ARGUMENT( bool, HasThrobber )

	/** 显示文本内容 */
	SLATE_ARGUMENT( FText, Content )

	/** 自动销毁时间 */
	SLATE_ARGUMENT( float, TimeInterval )

	/** 阻挡点击 */
	SLATE_ARGUMENT( bool, bBlock)
	
	SLATE_EVENT(FOnTapToastThrobberRemoveSelf, OnRemoveSelf)
	
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void ShowThrobber(bool bNewShow);

	void UpdateContent(const FText& NewContent);
	
	void UpdateTimeInterval(float NewTime);

	FORCEINLINE void UpdateRemoveSelfDelegate(const FOnTapToastThrobberRemoveSelf& InDelegate)
	{
		OnRemoveSelf = InDelegate;
	}

protected:
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	
	EActiveTimerReturnType TimerRemoveTapToastThrobber( double InCurrentTime, float InDeltaTime );
	
	TSharedPtr<SImage> Throbber;

	TSharedPtr<STextBlock> ContentText;

	TSharedPtr<FActiveTimerHandle> RemoveTimerHandle;

	FOnTapToastThrobberRemoveSelf OnRemoveSelf;

	FWidgetTransform Transform;

	float SavedRotationRate = 0.f;
};

