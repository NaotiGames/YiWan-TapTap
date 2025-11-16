// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "TapBillboardTypes.generated.h"

UENUM()
enum class ETapBillboardUrlType : uint8
{
	Custom,
	AudioOn,
	AudioOff,
	LaunchSystemBrowser,
	LaunchInnerBrowser
};

UENUM()
enum class ETapBillboardMarqueeOrientationScrollWidthType : uint8
{
	Fill,
	Fixed,
};

UENUM()
enum class ETapBillboardMarqueeOrientationPosition : uint8
{
	Top,
	TopLeft,
	TopRight,
	Bottom,
	BottomLeft,
	BottomRight,
};

ETapBillboardUrlType GetBillboardUrlType(const FString& Url);
ETapBillboardMarqueeOrientationScrollWidthType GetTapBillboardMarqueeOrientationScrollWidthType(const FString& Name);
ETapBillboardMarqueeOrientationPosition GetTapBillboardMarqueeOrientationPosition(const FString& Name);
FString GetTapBillboardMarqueeOrientationScrollWidthTypeName(ETapBillboardMarqueeOrientationScrollWidthType Type);
FString GetTapBillboardMarqueeOrientationPositionName(ETapBillboardMarqueeOrientationPosition Position);


USTRUCT(BlueprintType)
struct FBadgeDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool show_red_dot = false;

	UPROPERTY(BlueprintReadOnly)
	FString close_button_img;
};

struct FAnnouncementLayout;
class TAPBILLBOARD_API FTapBillboardMarqueeOrientationConfig {
public:
	
	ETapBillboardMarqueeOrientationScrollWidthType ScrollWidthType = ETapBillboardMarqueeOrientationScrollWidthType::Fill;
	ETapBillboardMarqueeOrientationPosition Position = ETapBillboardMarqueeOrientationPosition::Top;
	int ScrollWidth = 300;
	int HorizontalOffset = 0;
	int VerticalOffset = 0;
	int HorizontalScrollMargin = 10;
	int VerticalScrollMargin = 10;
	
	FTapBillboardMarqueeOrientationConfig() = default;
	FTapBillboardMarqueeOrientationConfig(const TSharedPtr<FJsonObject>& JsonObject);
	FTapBillboardMarqueeOrientationConfig(const FAnnouncementLayout& AnnouncementLayout);
	TSharedPtr<FJsonObject> ToJsonObject() const;
};


class TAPBILLBOARD_API FTapBillboardMarqueeIconConfig {
public:
	
	FString Url;
	
	FTapBillboardMarqueeIconConfig() = default;
	FTapBillboardMarqueeIconConfig(const TSharedPtr<FJsonObject>& JsonObject);
	TSharedPtr<FJsonObject> ToJsonObject() const;
};

struct FAnnouncementStyleData;
class TAPBILLBOARD_API FTapBillboardMarqueeConfig {
public:
	
	FColor DefaultTextColor;
	FColor ContentBackgroundColor;
	bool bUseSystemFont = true;
	FString CustomFont;
	int RepeatCount = 0;
	FTapBillboardMarqueeIconConfig Icon;
	FTapBillboardMarqueeOrientationConfig Horizontal;
	FTapBillboardMarqueeOrientationConfig Vertical;
	
	FTapBillboardMarqueeConfig() = default;
	FTapBillboardMarqueeConfig(const TSharedPtr<FJsonObject>& JsonObject);
	FTapBillboardMarqueeConfig(const FAnnouncementStyleData& StyleData);
	
	TSharedPtr<FJsonObject> ToJsonObject() const;
};


