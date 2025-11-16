#pragma once
#include "TapLicense.h"

class FTapLicenseImpl {
public:
	
	virtual ~FTapLicenseImpl();

	static TSharedPtr<FTapLicenseImpl>& Get();
	
	virtual void QueryDLC(const TArray<FString>& DLCList);
	virtual void SetLicenseCallback(FSimpleDelegate OnSuccess, FSimpleDelegate OnFailed);
	virtual void SetDLCCallback(bool bCheckOnce, const FString& PublicKey, FTapLicense::FDLCQueryDelegate OnQuery, FTapLicense::FDLCOrderDelegate OnOrder);
	virtual void Check(bool bIsForce = false);
	virtual void PurchaseDLC(const FString& DLC);
	virtual void SetTestEnvironment(bool bIsTest = false);


private:
	static TSharedPtr<FTapLicenseImpl> Instance;
	
};
