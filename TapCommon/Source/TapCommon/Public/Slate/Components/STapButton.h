// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Widgets/Input/SButton.h"
#include "STapButton.generated.h"

UENUM()
enum class ETapButtonStyleType : uint8
{
	Blue,//半圆角纯色蓝
	White,//半圆角带灰边白
	Black,//半圆角纯色黑
	RefreshBlue,//12px圆角边框蓝
	RefreshBlack,//12px圆角边框黑
	
	RoundBack,//圆圈返回
	RoundClose,//圆圈关闭
	SmallBack,//小箭头的返回
	SmallClose,//小方块关闭
	BoldBack,//粗的返回
	BoldClose,//粗的关闭
	LargeCopy,//带扩展边缘拷贝按钮
};

/**
 * 
 */
class TAPCOMMON_API STapButton : public SButton
{
public:
	SLATE_BEGIN_ARGS(STapButton)
		{
		}

	SLATE_ARGUMENT_DEFAULT( ETapButtonStyleType, StyleType ) { ETapButtonStyleType::Blue};

	SLATE_DEFAULT_SLOT( FArguments, Content )

	SLATE_ATTRIBUTE( FText, Text )

	SLATE_ARGUMENT_DEFAULT( EHorizontalAlignment, HAlign ) { HAlign_Center };

	SLATE_ARGUMENT_DEFAULT( EVerticalAlignment, VAlign ){ VAlign_Center };
	
	SLATE_ATTRIBUTE( FMargin, ContentPadding )
	
	SLATE_EVENT( FOnClicked, OnClicked )
	
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
	
	virtual const FSlateBrush* GetFocusBrush() const override;

	const FButtonStyle* GetButtonStyle() const;

	const FTextBlockStyle* GetTextStyle() const;

	ETapButtonStyleType StyleType = ETapButtonStyleType::Blue;
	
	TWeakPtr<FSlateStyleSet> SavedStyleSet;

	TSharedPtr<SWidget> ContentWidget;
};
