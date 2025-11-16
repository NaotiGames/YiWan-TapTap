#pragma once
#include "TapUpdateImpl.h"

class FTapUpdateAndroidImpl: public FTapUpdateImpl {
public:
	virtual ~FTapUpdateAndroidImpl() override;
	virtual void Init(const FString&  clientId, const FString&  clientToken) override;
	virtual void UpdateGame(FSimpleDelegate OnCancel) override;
};