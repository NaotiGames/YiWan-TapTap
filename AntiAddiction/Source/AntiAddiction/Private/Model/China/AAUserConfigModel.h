#pragma once
#include "CoreMinimal.h"
#include "AAUserConfigModel.generated.h"


USTRUCT()
struct FAAUStrictTipWord
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString title;

	UPROPERTY()
	FString description_plain;
	
};

USTRUCT()
struct FAAUUITipModel
{
	GENERATED_BODY()

	UPROPERTY()
	FAAUStrictTipWord allow;

	UPROPERTY()
	FAAUStrictTipWord reject;
	
};

USTRUCT()
struct FAACheckAgeModel
{
	GENERATED_BODY()

	UPROPERTY()
	bool allow = false;
};

USTRUCT()
struct FAARealNameStateModel
{
	GENERATED_BODY()

	UPROPERTY()
	bool is_adult = false;

	UPROPERTY()
	int age_limit = -1;
};

USTRUCT()
struct FAAPolicyModel
{
	GENERATED_BODY()

	FAAPolicyModel()
		: policy_active(TEXT(""))
		, policy_model(TEXT(""))
		, policy_heartbeat_interval(0)
	{

	}

	UPROPERTY()
	FString policy_active;

	UPROPERTY()
	FString policy_model;

	UPROPERTY()
	int policy_heartbeat_interval;
};

USTRUCT()
struct FAATimeRangeModel
{
	GENERATED_BODY()

	UPROPERTY()
	FAAUUITipModel text ;

	UPROPERTY()
	TArray<FString> holidays;

	UPROPERTY()
	FString time_start;

	UPROPERTY()
	FString time_end;
};

USTRUCT()
struct FAALocalModel
{
	GENERATED_BODY()

	UPROPERTY()
	FAATimeRangeModel time_range;
};


USTRUCT()
struct FAAUserConfigModel //FAAUChinaConfigModel
{
	GENERATED_BODY()
	
	UPROPERTY()
	FAACheckAgeModel content_rating_check ;

	UPROPERTY()
	FAARealNameStateModel real_name;
	
	UPROPERTY()
	FAAPolicyModel anti_addiction;

	UPROPERTY()
	FAALocalModel local;
	
	static void SaveToLocal(const FString& UserId,TSharedPtr<FAAUserConfigModel> Model);
	static TSharedPtr<FAAUserConfigModel> GetLocalModel();
	static void ResetModel();

public:
	static TSharedPtr<FAAUserConfigModel> CurrentModel;
};

