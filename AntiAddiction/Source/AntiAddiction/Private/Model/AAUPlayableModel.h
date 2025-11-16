#pragma once
#include "CoreMinimal.h"
#include "AAUPlayableModel.generated.h"

USTRUCT()
struct FAAUPlayableModel
{

	GENERATED_BODY()

	UPROPERTY()
	int remain_time = 0;  //   防沉迷剩余时间，单位秒

	UPROPERTY()
	FString title; // 实名的文案

	UPROPERTY()
	FString description_plain; // 距离健康保护时间还剩余14分钟，请注意适当休息。
	
};
