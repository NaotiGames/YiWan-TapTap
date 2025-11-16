#pragma once
#include "CoreMinimal.h"
#include "AAURealNameConfigModel.generated.h"


enum AAURealNameWordType {
	AAURealNameWordTypeQuickVerify = 0,
	AAURealNameWordTypeVerify, // 手动实名
	AAURealNameWordTypeVerifying, // 实名中
	AAURealNameWordTypeVerifyFail, //实名失败
};
 



USTRUCT()
struct FAAUAuthIdentifyWord
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString title;

	UPROPERTY()
	FString description_plain;

	UPROPERTY()
	FString negative_button;

	UPROPERTY()
	FString positive_button;
	
};

USTRUCT()
struct FAAUUIConfigModel
{
	GENERATED_BODY()

	UPROPERTY()
	FAAUAuthIdentifyWord taptap_auth;

	UPROPERTY()
	FAAUAuthIdentifyWord manual_auth;

	UPROPERTY()
	FAAUAuthIdentifyWord auth_waiting;
	
	UPROPERTY()
	FAAUAuthIdentifyWord auth_failed;  // 见AAURealNameWordType 
	
};

USTRUCT()
struct FAAURealNameConfigModel //FAAUChinaConfigModel
{
	GENERATED_BODY()
	
	UPROPERTY()
	bool manual_auth_enable = false;

	UPROPERTY()
	FAAUUIConfigModel real_name_text;
	
	static void SaveToLocal(TSharedPtr<FAAURealNameConfigModel> Model);
	static TSharedPtr<FAAURealNameConfigModel> GetLocalModel();
	static void ResetModel();

public:
	static TSharedPtr<FAAURealNameConfigModel> CurrentModel;
};
