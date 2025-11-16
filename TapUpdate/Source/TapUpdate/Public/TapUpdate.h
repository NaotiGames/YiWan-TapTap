#pragma once

class TAPUPDATE_API FTapUpdate {
public:
	static void Init(const FString&  clientId, const FString&  clientToken);
	static void UpdateGame(FSimpleDelegate OnCancel);
};