#include "TUDBEvent.h"

#include "TapDB.h"
#include "TUDBDevice.h"
#include "TUDeviceInfo.h"
#include "TUDBImpl.h"
#include "TUHelper.h"
#include "TUDebuger.h"
#include "TUDBNet.h"
#include "TUDBStorage.h"


FString const TUDBEvent::Key::IPV6					= TEXT("ip_v6");
FString const TUDBEvent::Key::UserID				= TEXT("user_id");
FString const TUDBEvent::Key::DeviceID				= TEXT("device_id");
FString const TUDBEvent::Key::OpenID				= TEXT("open_id");
FString const TUDBEvent::Key::Name					= TEXT("name");
FString const TUDBEvent::Key::Type					= TEXT("type");
FString const TUDBEvent::Key::ClientID				= TEXT("client_id");
FString const TUDBEvent::Key::Properties			= TEXT("properties");
FString const TUDBEvent::Key::OS					= TEXT("os");
FString const TUDBEvent::Key::DeviceModel			= TEXT("device_model");
FString const TUDBEvent::Key::DeviceID1				= TEXT("device_id1");
FString const TUDBEvent::Key::DeviceID2				= TEXT("device_id2");
FString const TUDBEvent::Key::DeviceID3				= TEXT("device_id3");
FString const TUDBEvent::Key::DeviceID4				= TEXT("device_id4");
FString const TUDBEvent::Key::DeviceID5				= TEXT("device_id5");
FString const TUDBEvent::Key::InstallID				= TEXT("install_uuid");
FString const TUDBEvent::Key::PersistID				= TEXT("persist_uuid");
FString const TUDBEvent::Key::Channel				= TEXT("channel");
FString const TUDBEvent::Key::Width					= TEXT("width");
FString const TUDBEvent::Key::Height				= TEXT("height");
FString const TUDBEvent::Key::OSVersion				= TEXT("os_version");
FString const TUDBEvent::Key::LangSystem			= TEXT("lang_system");
FString const TUDBEvent::Key::Provider				= TEXT("provider");
FString const TUDBEvent::Key::Network				= TEXT("network");
FString const TUDBEvent::Key::AppVersion			= TEXT("app_version");
FString const TUDBEvent::Key::SDKVersion			= TEXT("sdk_version");
FString const TUDBEvent::Key::LoginType				= TEXT("login_type");
FString const TUDBEvent::Key::OrderID				= TEXT("order_id");
FString const TUDBEvent::Key::Product				= TEXT("product");
FString const TUDBEvent::Key::Amount				= TEXT("amount");
FString const TUDBEvent::Key::VirtualCurrencyAmount = TEXT("virtual_currency_amount");
FString const TUDBEvent::Key::CurrencyType			= TEXT("currency_type");
FString const TUDBEvent::Key::Payment				= TEXT("payment");
FString const TUDBEvent::Key::Duration				= TEXT("duration");


// FString TUDBEventUser::GetOpenId()
// {
// 	return TUHelper::InvokeFunction<FString>("TULoginReflection", "GetOpenID");
// }


TUDBEvent::TUDBEvent(const FString& _ClientID) {
	ClientID = _ClientID;
	if (ClientID.IsEmpty()) {
		TUDebuger::ErrorLog(TEXT("TapDB ClientID is Empty"));
	}
	DeviceID = TUDeviceInfo::GetLoginId();
	if (DeviceID.IsEmpty()) {
		TUDebuger::ErrorLog(TEXT("TapDB DeviceID is Empty"));
	}
	GenerateSysProperties();
	GenerateCommonProperties();
	// UserID = TUDataStorage<FTUDBStorage>::LoadString(FString::Printf(TEXT("%s_%s"), *FTUDBStorage::LoginUserIDKey, *ClientID));
	// if (!UserID.IsEmpty()) {
	// 	SysProperties->SetStringField(Key::UserID, UserID);
	// }
	// FString OpenID = TUDataStorage<FTUDBStorage>::LoadString(FString::Printf(TEXT("%s_%s"), *FTUDBStorage::LoginOpenIDKey, *ClientID));
	// if (!OpenID.IsEmpty()) {
	// 	SysProperties->SetStringField(Key::OpenID, OpenID);
	// }
}

void TUDBEvent::SetUser(const FString& _UserID) {
	if (_UserID.IsEmpty()) {
		TUDebuger::ErrorLog(FString::Printf(TEXT("%s Error"), ANSI_TO_TCHAR(__FUNCTION__)));
		return;
	}
	UserID = _UserID;
	SysProperties->SetStringField(Key::UserID, _UserID);
	// TUDataStorage<FTUDBStorage>::SaveString(FString::Printf(TEXT("%s_%s"), *FTUDBStorage::LoginUserIDKey, *ClientID), _UserID);
}

void TUDBEvent::SetTapTapID(const FString& TapTapID) {
	if (TapTapID.IsEmpty()) {
		TUDebuger::ErrorLog(FString::Printf(TEXT("%s Error"), ANSI_TO_TCHAR(__FUNCTION__)));
		return;
	}
	SysProperties->SetStringField(Key::OpenID, TapTapID);
	// TUDataStorage<FTUDBStorage>::SaveString(FString::Printf(TEXT("%s_%s"), *FTUDBStorage::LoginOpenIDKey, *ClientID), TapTapID);
}

void TUDBEvent::ClearUser() {
	UserID = "";
	SysProperties->RemoveField(Key::UserID);
	SysProperties->RemoveField(Key::OpenID);
	// TUDataStorage<FTUDBStorage>::Remove(FString::Printf(TEXT("%s_%s"), *FTUDBStorage::LoginUserIDKey, *ClientID));
	// TUDataStorage<FTUDBStorage>::Remove(FString::Printf(TEXT("%s_%s"), *FTUDBStorage::LoginOpenIDKey, *ClientID));
}

void TUDBEvent::TrackEvent(const FString& EventName, TSharedPtr<FJsonObject> Properties, TFunction<void()> SuccessBlock) {
	TSharedPtr<FJsonObject> AllProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(AllProperties, SysProperties);
	AllProperties->SetStringField(Key::Type, TEXT("track"));
	AllProperties->SetStringField(Key::Name, EventName);

	TSharedPtr<FJsonObject> SubProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(SubProperties, CommonProperties);
	TUHelper::JsonObjectAppend(SubProperties, CustomStaticProperties);
	if (CustomDynamicPropertiesCaculator) {
		TUHelper::JsonObjectAppend(SubProperties, CustomDynamicPropertiesCaculator());
	}
	TUHelper::JsonObjectAppend(SubProperties, Properties);

	AllProperties->SetObjectField(Key::Properties, SubProperties);

	TUDBNet::SendEvent(AllProperties, SuccessBlock);
}

void TUDBEvent::DeviceInitialize(TSharedPtr<FJsonObject> Properties) {
	TSharedPtr<FJsonObject> AllProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(AllProperties, SysProperties);
	AllProperties->RemoveField(Key::UserID);
	AllProperties->SetStringField(Key::Type, TEXT("initialise"));

	TSharedPtr<FJsonObject> SubProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(SubProperties, Properties);

	AllProperties->SetObjectField(Key::Properties, SubProperties);

	TUDBNet::SendEvent(AllProperties);
}

void TUDBEvent::DeviceUpdate(TSharedPtr<FJsonObject> Properties) {
	TSharedPtr<FJsonObject> AllProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(AllProperties, SysProperties);
	AllProperties->RemoveField(Key::UserID);
	AllProperties->SetStringField(Key::Type, TEXT("update"));

	TSharedPtr<FJsonObject> SubProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(SubProperties, Properties);

	AllProperties->SetObjectField(Key::Properties, SubProperties);

	TUDBNet::SendEvent(AllProperties);
}

void TUDBEvent::DeviceAdd(TSharedPtr<FJsonObject> Properties) {
	TSharedPtr<FJsonObject> AllProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(AllProperties, SysProperties);
	AllProperties->RemoveField(Key::UserID);
	AllProperties->SetStringField(Key::Type, TEXT("add"));

	TSharedPtr<FJsonObject> SubProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(SubProperties, Properties);

	AllProperties->SetObjectField(Key::Properties, SubProperties);

	TUDBNet::SendEvent(AllProperties);
}

void TUDBEvent::UserInitialize(TSharedPtr<FJsonObject> Properties) {
	if (UserID.IsEmpty()) {
		TUDebuger::ErrorLog(FString::Printf(TEXT("Please Set User ID Before Call %s"), ANSI_TO_TCHAR(__FUNCTION__)));
		return;
	}
	TSharedPtr<FJsonObject> AllProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(AllProperties, SysProperties);
	AllProperties->RemoveField(Key::DeviceID);
	AllProperties->SetStringField(Key::Type, TEXT("initialise"));

	TSharedPtr<FJsonObject> SubProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(SubProperties, Properties);

	AllProperties->SetObjectField(Key::Properties, SubProperties);

	TUDBNet::SendEvent(AllProperties);
}

void TUDBEvent::UserUpdate(TSharedPtr<FJsonObject> Properties) {
	if (UserID.IsEmpty()) {
		TUDebuger::ErrorLog(FString::Printf(TEXT("Please Set User ID Before Call %s"), ANSI_TO_TCHAR(__FUNCTION__)));
		return;
	}
	TSharedPtr<FJsonObject> AllProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(AllProperties, SysProperties);
	AllProperties->RemoveField(Key::DeviceID);
	AllProperties->SetStringField(Key::Type, TEXT("update"));

	TSharedPtr<FJsonObject> SubProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(SubProperties, Properties);

	AllProperties->SetObjectField(Key::Properties, SubProperties);

	TUDBNet::SendEvent(AllProperties);
}

void TUDBEvent::UserAdd(TSharedPtr<FJsonObject> Properties) {
	if (UserID.IsEmpty()) {
		TUDebuger::ErrorLog(FString::Printf(TEXT("Please Set User ID Before Call %s"), ANSI_TO_TCHAR(__FUNCTION__)));
		return;
	}
	TSharedPtr<FJsonObject> AllProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(AllProperties, SysProperties);
	AllProperties->RemoveField(Key::DeviceID);
	AllProperties->SetStringField(Key::Type, TEXT("add"));

	TSharedPtr<FJsonObject> SubProperties = MakeShareable(new FJsonObject);
	TUHelper::JsonObjectAppend(SubProperties, Properties);

	AllProperties->SetObjectField(Key::Properties, SubProperties);

	TUDBNet::SendEvent(AllProperties);
}

void TUDBEvent::GenerateSysProperties() {
	SysProperties = MakeShareable(new FJsonObject);
	SysProperties->SetStringField(Key::ClientID, ClientID);
	SysProperties->SetStringField(Key::DeviceID, DeviceID);
	FString IPV6 = TUDeviceInfo::GetIpv6();
	if (!IPV6.IsEmpty()) {
		SysProperties->SetStringField(Key::IPV6, IPV6);
	}
}

void TUDBEvent::GenerateCommonProperties() {
	CommonProperties = MakeShareable(new FJsonObject);
	FString OS;
#if PLATFORM_IOS
	OS = TEXT("iOS");
#elif PLATFORM_ANDROID
	OS = TEXT("Android");
#elif PLATFORM_MAC
	OS = TEXT("Mac");
#elif PLATFORM_WINDOWS
	OS = TEXT("Windows");
#endif
	CommonProperties->SetStringField(Key::OS, OS);
	CommonProperties->SetNumberField(Key::Width, TUDeviceInfo::GetScreenWidth());
	CommonProperties->SetNumberField(Key::Height, TUDeviceInfo::GetScreenHeight());
	CommonProperties->SetStringField(Key::OSVersion, TUDeviceInfo::GetOSVersion());
	CommonProperties->SetStringField(Key::LangSystem, FPlatformMisc::GetDefaultLanguage());
	CommonProperties->SetStringField(Key::InstallID, TUDeviceInfo::GetInstallId());
	CommonProperties->SetStringField(Key::PersistID, DeviceID);
	CommonProperties->SetStringField(Key::DeviceID1, DeviceID);
	CommonProperties->SetStringField(Key::DeviceID5, TUDBDevice::GetDeviceId5());
	CommonProperties->SetStringField(Key::SDKVersion, TapDB_UE_VERSION);
	FString GameVersion = TUDBImpl::Get()->Config.GameVersion;
	if (!GameVersion.IsEmpty()) {
		CommonProperties->SetStringField(Key::AppVersion, GameVersion);
	}
	FString Channel = TUDBImpl::Get()->Config.Channel;
	if (!Channel.IsEmpty()) {
		CommonProperties->SetStringField(Key::Channel, Channel);
	}
}