#pragma once

struct FAnnouncementDetailData;
class TAPBILLBOARD_API FTapBillboardMarquee {
public:
	int Id;
	FString Content;
	FString Type;
	FString Template;
	FString ShortTitle;
	FString LongTitle;
	FString JumpLocation;
	FString JumpLink;
	int64 PublishTime;
	int64 ExpireTime;

	FTapBillboardMarquee() = default;
	FTapBillboardMarquee(const TSharedPtr<FJsonObject>& JsonObject);
	FTapBillboardMarquee(const FAnnouncementDetailData& DetailData);
	
	TSharedPtr<FJsonObject> ToJsonObject() const;
};
