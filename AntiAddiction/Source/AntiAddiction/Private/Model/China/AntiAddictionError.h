// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TUHttpResponse.h"

#include "AntiAddictionError.generated.h"

/**
 * 
 */
USTRUCT()
struct ANTIADDICTION_API FAntiAddictionError
{
	GENERATED_BODY()

	UPROPERTY()
	int32 code = 0;

	UPROPERTY()
	FString error;

	UPROPERTY()
	FString error_description;

	UPROPERTY()
	FString msg;

	TUHttpResponse::State httpState;
	  bool IsTokenExpired() const;
};
