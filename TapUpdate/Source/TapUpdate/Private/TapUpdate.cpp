#include "TapUpdate.h"

#include "TapUpdateImpl.h"

void FTapUpdate::Init(const FString&  clientId, const FString&  clientToken) {
	FTapUpdateImpl::Get()->Init(clientId, clientToken);
}

void FTapUpdate::UpdateGame(FSimpleDelegate OnCancel) {
	FTapUpdateImpl::Get()->UpdateGame(MoveTemp(OnCancel));
}
