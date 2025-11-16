#pragma once
#include "AAUType.h"
#include "AAUUser.generated.h"



USTRUCT()
struct FAAUUser {
	
	GENERATED_BODY()
	
	UPROPERTY()
	FString UserID;

	UPROPERTY()
	FString AccessTokenV2;

	UPROPERTY()
	FString AccessToken;

	EAAUAgeLimit AgeLimit;

	FAAUUser() = default;
	FAAUUser(const FString& _UserID, const FString& _AccessToken)
		: UserID(_UserID), AccessTokenV2(_AccessToken) {}

	void ResetAgeLimit(const int ageLimit, bool isAdult);

private:
	
};

