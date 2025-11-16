
#include "TapLicense.h"

#include "TapLicenseImpl.h"

void FTapLicense::QueryDLC(const TArray<FString>& DLCList) {
	FTapLicenseImpl::Get()->QueryDLC(DLCList);
}

void FTapLicense::SetDLCCallback(FDLCQueryDelegate OnQuery, FDLCOrderDelegate OnOrder) {
	SetDLCCallback(false, "", OnQuery, OnOrder);
}

void FTapLicense::SetLicenseCallback(FSimpleDelegate OnSuccess, FSimpleDelegate OnFailed) {
	FTapLicenseImpl::Get()->SetLicenseCallback(OnSuccess, OnFailed);
}

void FTapLicense::SetDLCCallback(bool bCheckOnce, const FString& PublicKey, FDLCQueryDelegate OnQuery,
	FDLCOrderDelegate OnOrder) {
	FTapLicenseImpl::Get()->SetDLCCallback(bCheckOnce, PublicKey, OnQuery, OnOrder);
}

void FTapLicense::Check(bool bIsForce) {
	FTapLicenseImpl::Get()->Check(bIsForce);
}

void FTapLicense::PurchaseDLC(const FString& DLC) {
	FTapLicenseImpl::Get()->PurchaseDLC(DLC);
}

void FTapLicense::SetTestEnvironment(bool bIsTest){
	FTapLicenseImpl::Get()->SetTestEnvironment(bIsTest);
}







