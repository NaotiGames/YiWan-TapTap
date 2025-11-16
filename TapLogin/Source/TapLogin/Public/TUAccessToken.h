#pragma once
#include "CoreMinimal.h"
#include "TUAccessToken.generated.h"

USTRUCT(BlueprintType)
struct TAPLOGIN_API FTUAccessToken
{
	GENERATED_BODY()

	UPROPERTY()
	FString kid;

	UPROPERTY()
	FString access_token;

	UPROPERTY()
	FString token_type;

	UPROPERTY()
	FString mac_key;

	UPROPERTY()
	FString mac_algorithm;

	UPROPERTY()
	FString scope;
	
	FORCEINLINE int32 ParseScope(TArray<FString>& OutScopes) const
	{
		return scope.ParseIntoArray(OutScopes, TEXT(" "), false);
	}

	bool ContainsScope(const TCHAR* InScope) const;

#if PLATFORM_MAC || PLATFORM_WINDOWS

	void SaveToLocal() const;
	
	static TSharedPtr<FTUAccessToken> GetLocalModel();

	static void ClearLocalModel();

#endif


	
};
