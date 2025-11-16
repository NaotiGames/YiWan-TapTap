#pragma once

#include "TapLicenseImpl.h"

class FTapLicenseWindows: public FTapLicenseImpl {
public:
	virtual ~FTapLicenseWindows() override;
	virtual void SetLicenseCallback(FSimpleDelegate OnSuccess, FSimpleDelegate OnFailed) override;
	virtual void SetDLCCallback(bool bCheckOnce, const FString& PublicKey, FTapLicense::FDLCQueryDelegate OnQuery,
		FTapLicense::FDLCOrderDelegate OnOrder) override;
	virtual void Check(bool bIsForce) override;
	virtual void QueryDLC(const TArray<FString>& DLCList) override;
	virtual void PurchaseDLC(const FString& DLC) override;
	virtual void SetTestEnvironment(bool bIsTest) override;

private:
	bool HasRegisterNativeDLCCallback = false;
	bool HasRegisterNativeLicenseCallback = false;
	FSimpleDelegate LicenseSuccessDelegate;
	FSimpleDelegate LicenseFailedDelegate;
	FTapLicense::FDLCQueryDelegate DLCQueryDelegate;
	FTapLicense::FDLCOrderDelegate DLCOrderDelegate;
};
