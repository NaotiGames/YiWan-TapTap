#pragma once
#include "CoreMinimal.h"
#include "AAUType.h"
#include "AAURealNameResultModel.generated.h"


USTRUCT()
struct FAAURealNameResultModel
{
	enum AuthState {
		Success = 0,                     // 认证成功
		Verifying = 1,                   // 认证中
		Fail = 2,                        // 认证失败
	};

	GENERATED_BODY()
	
	UPROPERTY()
	FString status ;

	UPROPERTY()
	FString anti_addiction_token;

	AuthState GetAuthState() {
		if (status.Equals("pass")) {
			return Success;
		} else if (status.Equals("waiting")) {
			return Verifying;
		} else if (status.Equals("failed")) {
			return Fail;
		} else {
			return Success;
		}
	}
	
};
