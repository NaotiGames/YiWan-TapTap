// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TAPCOMMON_API FTapStyle
{
public:
	static TSharedPtr<FSlateBrush> MakeRoundCornersBrush(bool bHalfHeightRadius = false, float CornerRadius = 8.f, UTexture2D* IconTexture = nullptr);

	static const FSlateBrush* GetUnitBrush();
};
