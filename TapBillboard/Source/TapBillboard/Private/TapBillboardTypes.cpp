// Fill out your copyright notice in the Description page of Project Settings.


#include "TapBillboardTypes.h"

#include "StringHelpers.h"
#include "TapBillboardModels.h"

ETapBillboardUrlType GetBillboardUrlType(const FString& Url)
{
	int32 Index = INDEX_NONE;
	Url.FindChar(TEXT('#'), Index);
	if (Index != INDEX_NONE)
	{
		const FString& SubStr = Url.Right(Url.Len() - Index - 1);
		if (SubStr == TEXT("com_taptap_tdsbillboard_audible"))
		{
			return ETapBillboardUrlType::AudioOn;
		}
		else if(SubStr == TEXT("com_taptap_tdsbillboard_mute"))
		{
			return ETapBillboardUrlType::AudioOff;
		}
	}

	if(Url.Left(11) != TEXT("javascript:") && Url.Left(7) != TEXT("http://") && Url.Left(8) != TEXT("https://"))
	{
		return ETapBillboardUrlType::Custom;
	}
	
	if (Url.Right(17) == TEXT("tdsTarget=browser"))
	{
		return ETapBillboardUrlType::LaunchSystemBrowser;
	}
	return ETapBillboardUrlType::LaunchInnerBrowser;
}

ETapBillboardMarqueeOrientationScrollWidthType GetTapBillboardMarqueeOrientationScrollWidthType(const FString& Name) {
	if (Name == "FILL") {
		return ETapBillboardMarqueeOrientationScrollWidthType::Fill;
	}  else if (Name == "FIXED") {
		return ETapBillboardMarqueeOrientationScrollWidthType::Fixed;
	}
	return ETapBillboardMarqueeOrientationScrollWidthType::Fill;
}

ETapBillboardMarqueeOrientationPosition GetTapBillboardMarqueeOrientationPosition(const FString& Name) {
	 if (Name == "TOP_LEFT" || Name == "topLeft") {
		return ETapBillboardMarqueeOrientationPosition::TopLeft;
	}  else if (Name == "TOP_RIGHT" || Name == "topRight") {
		return ETapBillboardMarqueeOrientationPosition::TopRight;
	}  else if (Name == "BOTTOM" || Name == "bottom") {
		return ETapBillboardMarqueeOrientationPosition::Bottom;
	}  else if (Name == "BOTTOM_LEFT" || Name == "bottomLeft") {
		return ETapBillboardMarqueeOrientationPosition::BottomLeft;
	}  else if (Name == "BOTTOM_RIGHT" || Name == "bottomRight") {
		return ETapBillboardMarqueeOrientationPosition::BottomLeft;
	}  
	return ETapBillboardMarqueeOrientationPosition::Top;
}

FString GetTapBillboardMarqueeOrientationScrollWidthTypeName(ETapBillboardMarqueeOrientationScrollWidthType Type) {
	switch (Type) {
	case ETapBillboardMarqueeOrientationScrollWidthType::Fill:
		return "FILL";
		break;
	case ETapBillboardMarqueeOrientationScrollWidthType::Fixed:
		return "FIXED";
		break;
	default: return "FILL";
	}
}

FString GetTapBillboardMarqueeOrientationPositionName(ETapBillboardMarqueeOrientationPosition Position) {
	switch (Position) {
	case ETapBillboardMarqueeOrientationPosition::Top:
		return "TOP";
		break;
	case ETapBillboardMarqueeOrientationPosition::TopLeft:
		return "TOP_LEFT";
		break;
	case ETapBillboardMarqueeOrientationPosition::TopRight:
		return "TOP_RIGHT";
		break;
	case ETapBillboardMarqueeOrientationPosition::Bottom:
		return "BOTTOM";
		break;
	case ETapBillboardMarqueeOrientationPosition::BottomLeft:
		return "BOTTOM_LEFT";
		break;
	case ETapBillboardMarqueeOrientationPosition::BottomRight:
		return "BOTTOM_RIGHT";
		break;
	default: return "TOP";
	}
}

FTapBillboardMarqueeOrientationConfig::
FTapBillboardMarqueeOrientationConfig(const TSharedPtr<FJsonObject>& JsonObject) {
	if (!JsonObject.IsValid()) {
		return;
	}
	JsonObject->TryGetNumberField(TEXT("scroll_width"), ScrollWidth);
	JsonObject->TryGetNumberField(TEXT("position_offset_x"), HorizontalOffset);
	JsonObject->TryGetNumberField(TEXT("position_offset_y"), VerticalOffset);
	JsonObject->TryGetNumberField(TEXT("scroll_margin_x"), HorizontalScrollMargin);
	JsonObject->TryGetNumberField(TEXT("scroll_margin_y"), VerticalScrollMargin);

	JsonObject->TryGetNumberField(TEXT("scrollWidth"), ScrollWidth);
	JsonObject->TryGetNumberField(TEXT("horizontalOffset"), HorizontalOffset);
	JsonObject->TryGetNumberField(TEXT("verticalOffset"), VerticalOffset);
	JsonObject->TryGetNumberField(TEXT("horizontalScrollMargin"), HorizontalScrollMargin);
	JsonObject->TryGetNumberField(TEXT("verticalScrollMargin"), VerticalScrollMargin);
	FString Temp;
	if (JsonObject->TryGetStringField(TEXT("scroll_width_type"), Temp) ||
		JsonObject->TryGetStringField(TEXT("scrollWidthType"), Temp)) {
		ScrollWidthType = GetTapBillboardMarqueeOrientationScrollWidthType(Temp);
	}
	if (JsonObject->TryGetStringField(TEXT("position"), Temp)) {
		Position = GetTapBillboardMarqueeOrientationPosition(Temp);
	}
}

FTapBillboardMarqueeOrientationConfig::FTapBillboardMarqueeOrientationConfig(
	const FAnnouncementLayout& AnnouncementLayout) {
	ScrollWidth = AnnouncementLayout.scroll_width;
	HorizontalOffset = AnnouncementLayout.position_offset_x;
	VerticalOffset = AnnouncementLayout.position_offset_y;
	HorizontalScrollMargin = AnnouncementLayout.scroll_margin_x;
	VerticalScrollMargin = AnnouncementLayout.scroll_margin_y;
	ScrollWidthType = GetTapBillboardMarqueeOrientationScrollWidthType(AnnouncementLayout.scroll_width_type);
	Position = GetTapBillboardMarqueeOrientationPosition(AnnouncementLayout.position);
}

TSharedPtr<FJsonObject> FTapBillboardMarqueeOrientationConfig::ToJsonObject() const {
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("scroll_width"), ScrollWidth);
	JsonObject->SetNumberField(TEXT("position_offset_x"), HorizontalOffset);
	JsonObject->SetNumberField(TEXT("position_offset_y"), VerticalOffset);
	JsonObject->SetNumberField(TEXT("scroll_margin_x"), HorizontalScrollMargin);
	JsonObject->SetNumberField(TEXT("scroll_margin_y"), VerticalScrollMargin);
	JsonObject->SetStringField(TEXT("scroll_width_type"), GetTapBillboardMarqueeOrientationScrollWidthTypeName(ScrollWidthType));
	JsonObject->SetStringField(TEXT("position"), GetTapBillboardMarqueeOrientationPositionName(Position));
	return JsonObject;
}

FTapBillboardMarqueeIconConfig::FTapBillboardMarqueeIconConfig(const TSharedPtr<FJsonObject>& JsonObject) {
	if (!JsonObject.IsValid()) {
		return;
	}
	JsonObject->TryGetStringField(TEXT("url"), Url);
}

TSharedPtr<FJsonObject> FTapBillboardMarqueeIconConfig::ToJsonObject() const {
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("url"), Url);
	return JsonObject;
}

FTapBillboardMarqueeConfig::FTapBillboardMarqueeConfig(const TSharedPtr<FJsonObject>& JsonObject) {
	if (!JsonObject.IsValid()) {
		return;
	}
	JsonObject->TryGetBoolField(TEXT("useSystemFont"), bUseSystemFont);
	JsonObject->TryGetStringField(TEXT("customFont"), CustomFont);
	JsonObject->TryGetNumberField(TEXT("repeatCount"), RepeatCount);

	JsonObject->TryGetBoolField(TEXT("use_system_font"), bUseSystemFont);
	JsonObject->TryGetStringField(TEXT("custom_font"), CustomFont);
	JsonObject->TryGetNumberField(TEXT("repeat_count"), RepeatCount);
	FString ColorHex;
	if (JsonObject->TryGetStringField(TEXT("defaultTextColor"), ColorHex) ||
		JsonObject->TryGetStringField(TEXT("default_text_color"), ColorHex)) {
		DefaultTextColor = HexToColor(ColorHex);
	}
	if (JsonObject->TryGetStringField(TEXT("contentBackgroundColor"), ColorHex) ||
		JsonObject->TryGetStringField(TEXT("content_background_color"), ColorHex)) {
		ContentBackgroundColor = HexToColor(ColorHex);
	}
	const TSharedPtr<FJsonObject>* JsonObjectPtr = nullptr;
	if (JsonObject->TryGetObjectField(TEXT("icon"), JsonObjectPtr)) {
		Icon = FTapBillboardMarqueeIconConfig(*JsonObjectPtr);
	}
	if (JsonObject->TryGetObjectField(TEXT("horizontal"), JsonObjectPtr)) {
		Horizontal = FTapBillboardMarqueeOrientationConfig(*JsonObjectPtr);
	}
	if (JsonObject->TryGetObjectField(TEXT("vertical"), JsonObjectPtr)) {
		Vertical = FTapBillboardMarqueeOrientationConfig(*JsonObjectPtr);
	}
}

FTapBillboardMarqueeConfig::FTapBillboardMarqueeConfig(const FAnnouncementStyleData& StyleData) {
	bUseSystemFont = StyleData.use_system_font;
	CustomFont = StyleData.custom_font;
	RepeatCount = StyleData.repeat_count;
	DefaultTextColor = HexToColor(StyleData.default_text_color);
	ContentBackgroundColor = HexToColor(StyleData.content_background_color);
	Icon.Url = StyleData.icon.url;
	Horizontal = FTapBillboardMarqueeOrientationConfig(StyleData.horizontal);
	Vertical = FTapBillboardMarqueeOrientationConfig(StyleData.vertical);
}

TSharedPtr<FJsonObject> FTapBillboardMarqueeConfig::ToJsonObject() const {
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("default_text_color"), ColorToHex(DefaultTextColor));
	JsonObject->SetStringField(TEXT("content_background_color"), ColorToHex(ContentBackgroundColor));
	JsonObject->SetBoolField(TEXT("use_system_font"), bUseSystemFont);
	JsonObject->SetStringField(TEXT("custom_font"), CustomFont);
	JsonObject->SetNumberField(TEXT("repeat_count"), RepeatCount);
	JsonObject->SetObjectField(TEXT("icon"), Icon.ToJsonObject());
	JsonObject->SetObjectField(TEXT("horizontal"), Horizontal.ToJsonObject());
	JsonObject->SetObjectField(TEXT("vertical"), Vertical.ToJsonObject());

	return JsonObject;
}

