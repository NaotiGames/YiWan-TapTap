#include "TapLicenseWindows.h"
#include "TapUECommon.h"
#include "TUHelper.h"


FTapLicenseWindows::~FTapLicenseWindows() {
}

void FTapLicenseWindows::SetLicenseCallback(FSimpleDelegate OnSuccess, FSimpleDelegate OnFailed)
{
	LicenseSuccessDelegate = OnSuccess;
	LicenseFailedDelegate = OnFailed;
	if(!HasRegisterNativeLicenseCallback)
	{
		TapUECommon::RegisterLicenseCallback([this](const bool isOwned)
		{
			if (isOwned)
			{
				LicenseSuccessDelegate.ExecuteIfBound();
			}else
			{
				LicenseFailedDelegate.ExecuteIfBound();
			}
		});
	}
}
void FTapLicenseWindows::Check(bool bIsForce)
{
	const bool isOwned =  TapUECommon::HasTapLicense();
	if (isOwned)
	{
		LicenseSuccessDelegate.ExecuteIfBound();
	}else
	{
		LicenseFailedDelegate.ExecuteIfBound();
	}
}
void FTapLicenseWindows::PurchaseDLC(const FString& DLC)
{
	 TapUECommon::ShowStoreWithDLC(DLC);
}
void FTapLicenseWindows::SetDLCCallback(bool bCheckOnce, const FString& PublicKey, FTapLicense::FDLCQueryDelegate OnQuery, FTapLicense::FDLCOrderDelegate OnOrder)
{
	DLCQueryDelegate = OnQuery;
	DLCOrderDelegate = OnOrder;
	if (!HasRegisterNativeDLCCallback)
	{
		HasRegisterNativeDLCCallback = true;
		TapUECommon::RegisterDLCOwnedCallback([this](FString DLCId, const bool IsOwned)
		{
			DLCOrderDelegate.ExecuteIfBound(DLCId, IsOwned ? FTapLicense::EOrderStatus::DLCPurchased : FTapLicense::EOrderStatus::DLCNotPurchased);
		});
	}
}
void FTapLicenseWindows::QueryDLC(const TArray<FString>& DLCList)
{
	if(DLCList.Num() > 0)
	{
		FTapLicense::Map Map;
		for (auto DLC : DLCList)
		{
			Map.Add(DLC, TapUECommon::QueryTapDLC(DLC));
		}
		DLCQueryDelegate.ExecuteIfBound(FTapLicense::EQueryResult::OK,Map);
	}
}

void FTapLicenseWindows::SetTestEnvironment(bool bIsTest)
{
	
}

