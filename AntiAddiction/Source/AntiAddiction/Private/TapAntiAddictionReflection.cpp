#include "TapAntiAddictionReflection.h"
#include "TUType.h"
#include "AAUType.h"
#include "AntiAddictionUE.h"

void UTapAntiAddictionReflection::Init() {
	FTUConfig Config = *FTUConfig::Get().Get();
	if (!Config.TapAntiAddictionConfig.IsValid()) {
		return;
	}
	FAAUConfig aauConfig;
	aauConfig.ClientID = Config.ClientID;
	aauConfig.ShowSwitchAccount = Config.TapAntiAddictionConfig->bShowSwitchAccount;
	aauConfig.UseAgeRange = Config.TapAntiAddictionConfig->bUseAgeRange;
	AntiAddictionUE::Init(aauConfig);
}
