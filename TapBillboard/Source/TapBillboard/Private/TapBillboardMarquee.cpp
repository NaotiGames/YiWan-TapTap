#include "TapBillboardMarquee.h"
#include "TapBillboardModels.h"

FTapBillboardMarquee::FTapBillboardMarquee(const TSharedPtr<FJsonObject>& JsonObject) {
	if (!JsonObject.IsValid()) {
		return;
	}
	JsonObject->TryGetNumberField(TEXT("id"), Id);
	JsonObject->TryGetStringField(TEXT("content"), Content);
	JsonObject->TryGetStringField(TEXT("type"), Type);
	JsonObject->TryGetStringField(TEXT("template"), Template);
	JsonObject->TryGetStringField(TEXT("short_title"), ShortTitle);
	JsonObject->TryGetStringField(TEXT("long_title"), LongTitle);
	JsonObject->TryGetStringField(TEXT("jump_location"), JumpLocation);
	JsonObject->TryGetStringField(TEXT("jump_link"), JumpLink);
	JsonObject->TryGetNumberField(TEXT("publish_time"), PublishTime);
	JsonObject->TryGetNumberField(TEXT("expire_time"), ExpireTime);

	JsonObject->TryGetStringField(TEXT("shortTitle"), ShortTitle);
	JsonObject->TryGetStringField(TEXT("longTitle"), LongTitle);
	JsonObject->TryGetStringField(TEXT("jumpLocation"), JumpLocation);
	JsonObject->TryGetStringField(TEXT("jumpLink"), JumpLink);
	JsonObject->TryGetNumberField(TEXT("publishTime"), PublishTime);
	JsonObject->TryGetNumberField(TEXT("expireTime"), ExpireTime);
}

FTapBillboardMarquee::FTapBillboardMarquee(const FAnnouncementDetailData& DetailData) {
	Id = DetailData.id;
	Content = DetailData.content;
	Type = DetailData.type;
	Template = DetailData.Template;
	ShortTitle = DetailData.short_title;
	LongTitle = DetailData.long_title;
	JumpLocation = DetailData.jump_location;
	JumpLink = DetailData.jump_link;
	PublishTime = DetailData.publish_time;
	ExpireTime = DetailData.expire_time;
}

TSharedPtr<FJsonObject> FTapBillboardMarquee::ToJsonObject() const {
	// 创建一个新的 FJsonObject
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	// 设置对象属性
	JsonObject->SetNumberField(TEXT("id"), Id);
	JsonObject->SetStringField(TEXT("content"), Content);
	JsonObject->SetStringField(TEXT("type"), Type);
	JsonObject->SetStringField(TEXT("template"), Template);
	JsonObject->SetStringField(TEXT("short_title"), ShortTitle);
	JsonObject->SetStringField(TEXT("long_title"), LongTitle);
	JsonObject->SetStringField(TEXT("jump_location"), JumpLocation);
	JsonObject->SetStringField(TEXT("jump_link"), JumpLink);
	JsonObject->SetNumberField(TEXT("publish_time"), PublishTime);
	JsonObject->SetNumberField(TEXT("expire_time"), ExpireTime);

	return JsonObject;
}
