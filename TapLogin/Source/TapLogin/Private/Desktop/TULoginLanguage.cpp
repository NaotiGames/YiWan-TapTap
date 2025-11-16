#include "TULoginLanguage.h"

#include "TULanguage.h"


TSharedPtr<ILoginLang> TULoginLanguage::CurrentLang = nullptr;
ELanguageType TULoginLanguage::LanguageType = ELanguageType::AUTO;

TSharedPtr<ILoginLang> TULoginLanguage::GetCurrentLang()
{
	auto CurrentType = TULanguage::GetCurrentType();
	if (CurrentLang.IsValid() && CurrentType == LanguageType) {
		return CurrentLang;
	}
	switch (CurrentType) {
	case ELanguageType::ZH:
		CurrentLang = MakeShareable(new LoginLangCN);
		break;
	case ELanguageType::EN:
		CurrentLang = MakeShareable(new LoginLangIO);
		break;
	case ELanguageType::ID:
		CurrentLang = MakeShareable(new LoginLangID);
		break;
	case ELanguageType::JA:
		CurrentLang = MakeShareable(new LoginLangJA);
		break;
	case ELanguageType::KO:
		CurrentLang = MakeShareable(new LoginLangKO);
		break;
	case ELanguageType::TH:
		CurrentLang = MakeShareable(new LoginLangTH);
		break;
	case ELanguageType::ZHTW:
		CurrentLang = MakeShareable(new LoginLangZHTW);
		break;
	case ELanguageType::DE: 
		CurrentLang = MakeShared<LoginLangDE>(); 
		break;
	case ELanguageType::ES: 
		CurrentLang = MakeShared<LoginLangES>(); 
		break;
	case ELanguageType::FR: 
		CurrentLang = MakeShared<LoginLangFR>(); 
		break;
	case ELanguageType::PT: 
		CurrentLang = MakeShared<LoginLangPT>(); 
		break;
	case ELanguageType::RU: 
		CurrentLang = MakeShared<LoginLangRU>(); 
		break;
	case ELanguageType::TR: 
		CurrentLang = MakeShared<LoginLangTR>(); 
		break;
	case ELanguageType::VI: 
		CurrentLang = MakeShared<LoginLangVI>(); 
		break;
	default:
		if (FTUConfig::Get()->RegionType == ERegionType::CN) {
			CurrentLang = MakeShareable(new LoginLangCN);
		} else {
			CurrentLang = MakeShareable(new LoginLangIO);
		}
		break;
	}
	return CurrentLang;
}

// 	default:
// 		CurrentLang = MakeShareable(new LoginLangIO);
// 		break;
// 	}
// }


