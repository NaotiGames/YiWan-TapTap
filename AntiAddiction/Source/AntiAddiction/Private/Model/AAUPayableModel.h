#pragma once
#include "CoreMinimal.h"
#include "AAUPayableModel.generated.h"


USTRUCT()
struct FAAUPayableModel
{
	GENERATED_BODY()
	
	UPROPERTY()
	bool allow = false; 
	
	UPROPERTY()
	FString title; 

	UPROPERTY()
	FString description_plain;
	
	
};


USTRUCT()
struct FAAUPaymentModel
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString message;
};