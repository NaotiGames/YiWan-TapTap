#include "TapBillboardLang.h"
#include "TULanguage.h"
#include "TapSingleton.h"

TSharedPtr<IBillboardLang> TUBillboardLanguage::CurrentLang = nullptr;
ELanguageType TUBillboardLanguage::LanguageType = ELanguageType::AUTO;

TSharedPtr<IBillboardLang> TUBillboardLanguage::GetCurrentLang()
{
	auto CurrentType = TULanguage::GetCurrentType();
	if (CurrentLang.IsValid() && CurrentType == LanguageType) {
		return CurrentLang;
	}
	switch (CurrentType) {
	case ELanguageType::ZH:
		CurrentLang = MakeShareable(new BillboardLangCN);
		break;
	case ELanguageType::EN:
		CurrentLang = MakeShareable(new BillboardLangIO);
		break;
	case ELanguageType::ID:
		CurrentLang = MakeShareable(new BillboardLangID);
		break;
	case ELanguageType::JA:
		CurrentLang = MakeShareable(new BillboardLangJA);
		break;
	case ELanguageType::KO:
		CurrentLang = MakeShareable(new BillboardLangKO);
		break;
	case ELanguageType::TH:
		CurrentLang = MakeShareable(new BillboardLangTH);
		break;
	case ELanguageType::ZHTW:
		CurrentLang = MakeShareable(new BillboardLangZHTW);
		break;
	case ELanguageType::DE: 
		CurrentLang = MakeShared<BillboardLangDE>(); 
		break;
	case ELanguageType::ES: 
		CurrentLang = MakeShared<BillboardLangES>(); 
		break;
	case ELanguageType::FR: 
		CurrentLang = MakeShared<BillboardLangFR>(); 
		break;
	case ELanguageType::PT: 
		CurrentLang = MakeShared<BillboardLangPT>(); 
		break;
	case ELanguageType::RU: 
		CurrentLang = MakeShared<BillboardLangRU>(); 
		break;
	case ELanguageType::TR: 
		CurrentLang = MakeShared<BillboardLangTR>(); 
		break;
	case ELanguageType::VI: 
		CurrentLang = MakeShared<BillboardLangVI>(); 
		break;
	default:
		if (FTUConfig::Get()->RegionType == ERegionType::CN) {
			CurrentLang = MakeShareable(new BillboardLangCN);
		} else {
			CurrentLang = MakeShareable(new BillboardLangIO);
		}
		break;
	}
	return CurrentLang;
}
