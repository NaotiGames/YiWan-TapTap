// Copyright Epic Games, Inc. All Rights Reserved.

#include "TapAchievementTypes.h"
#include "TUType.h"

#define LOCTEXT_NAMESPACE "TapAchievement"

FAchievementDescTap::FAchievementDescTap(const TSharedPtr<FJsonObject>& JsonObject)
{
	JsonObject->TryGetStringField(TEXT("achievement_id"), AchievementId);
	JsonObject->TryGetStringField(TEXT("achievement_open_id"), DisplayId);
	JsonObject->TryGetBoolField(TEXT("is_hide"), bIsHide);
	JsonObject->TryGetNumberField(TEXT("count_step"), CountStep);
	JsonObject->TryGetNumberField(TEXT("show_order"), ShowOrder);

	const TSharedPtr<FJsonObject>* ObjP = nullptr;
	if (JsonObject->TryGetObjectField(TEXT("achievement_config_out_dto"), ObjP))
	{
		(*ObjP)->TryGetStringField(TEXT("achievement_title"), AchievementTitle);
		(*ObjP)->TryGetStringField(TEXT("achievement_sub_title"), AchievementSubtitle);
		(*ObjP)->TryGetStringField(TEXT("achievement_icon"), AchievementIconUrl);
	}
	
	if (JsonObject->TryGetObjectField(TEXT("achievement_rarity"), ObjP))
	{
		(*ObjP)->TryGetNumberField(TEXT("rarity"), RarityD);
		Rarity = RarityD;
		
		(*ObjP)->TryGetNumberField(TEXT("level"), Level);
	}

	JsonObject->TryGetNumberField(TEXT("achievement_type"), Type);
}


FAchievementTap::FAchievementTap(const TSharedPtr<FJsonObject>& JsonObject)
{
	JsonObject->TryGetStringField(TEXT("achievement_id"), AchievementId);
	JsonObject->TryGetStringField(TEXT("achievement_open_id"), DisplayId);
	int64 Ticks;
	JsonObject->TryGetNumberField(TEXT("complete_time"), Ticks);
	CompleteTime = FDateTime::FromUnixTimestamp(Ticks / 1000);
	JsonObject->TryGetNumberField(TEXT("completed_step"), CompleteStep);
	JsonObject->TryGetNumberField(TEXT("completed_step"), CompleteStep);
	JsonObject->TryGetBoolField(TEXT("full_completed"), bFullCompleted);
}

FAchievementTap::FAchievementTap(const FAchievementDescTap& Desc)
{
	AchievementId = Desc.AchievementId;
	DisplayId = Desc.DisplayId;
}

void FAchievementTap::Write(const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>>& Writer) const
{
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("achievement_id"), AchievementId);
	Writer->WriteValue(TEXT("achievement_open_id"), DisplayId);
	Writer->WriteValue(TEXT("completed_step"), CompleteStep);
	const int64 Ticks = CompleteTime.ToUnixTimestamp() * 1000;
	Writer->WriteValue(TEXT("complete_time"), Ticks);
	Writer->WriteObjectEnd();
}

bool FAchievementTap::UpdateStep(const FAchievementDescTap& Description, int32 NewStep)
{
	int32 Step = FMath::Clamp(NewStep, CompleteStep, Description.CountStep);//目前不支持倒退
	if (CompleteStep != Step)
	{
		CompleteStep = Step;
		bFullCompleted = CompleteStep == Description.CountStep;
		CompleteTime = FDateTime::UtcNow();
		return true;
	}
	return false;
}

void FAchievementLanguage::UpdateLanguage(ELanguageType NewType)
{
	switch (NewType)
	{
	case ELanguageType::ZH:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "已解锁{0}项成就，共{1}项成就");
		DialogPercent = LOCTEXT("RarityFormat", "“有{0}%的玩家达成此项成就”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“只有不到0.1%的玩家达成此项成就”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- {0}.{1}.{2} 获得 -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "隐藏");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "继续玩下去就会知道了");
		Unlocked = LOCTEXT("AchievementUnlocked", "已解锁");
		Locked = LOCTEXT("AchievementLocked", "未解锁");
		RarityCommon = LOCTEXT("AchievementCommon", "普通");
		RarityUncommon = LOCTEXT("AchievementUncommon", "稀有");
		RarityRare = LOCTEXT("AchievementRare", "珍贵");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "极为珍贵");
		PopTitle = LOCTEXT("AchievementPopTitle", "恭喜解锁新成就！");
		break;
	case ELanguageType::EN:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} Achievements");
		DialogPercent = LOCTEXT("RarityFormat", "'{0}% of players have this'");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "'Fewer than 0.1% of players have achieved this'");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- Accomplished on {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "Hidden");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "Keep playing to find out");
		Unlocked = LOCTEXT("AchievementUnlocked", "UNLOCKED");
		Locked = LOCTEXT("AchievementLocked", "LOCKED");
		RarityCommon = LOCTEXT("AchievementCommon", "Common");
		RarityUncommon = LOCTEXT("AchievementUncommon", "Rare");
		RarityRare = LOCTEXT("AchievementRare", "Super Rare");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "Ultra Rare");
		PopTitle = LOCTEXT("AchievementPopTitle", "Achievement unlocked!");
		break;
	case ELanguageType::ZHTW:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "已解鎖{0}項成就，共{1}項成就");
		DialogPercent = LOCTEXT("RarityFormat", "“有{0}%的玩家達成此項成就”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“只有不到0.1%的玩家達成此項成就”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- {0}.{1}.{2} 獲得 -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "隱藏");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "繼續玩下去就會知道囉");
		Unlocked = LOCTEXT("AchievementUnlocked", "已解鎖");
		Locked = LOCTEXT("AchievementLocked", "未解鎖");
		RarityCommon = LOCTEXT("AchievementCommon", "普通");
		RarityUncommon = LOCTEXT("AchievementUncommon", "稀有");
		RarityRare = LOCTEXT("AchievementRare", "珍貴");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "極為珍貴");
		PopTitle = LOCTEXT("AchievementPopTitle", "恭喜解鎖新成就！");
		break;
	case ELanguageType::JA:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} 達成項目");
		DialogPercent = LOCTEXT("RarityFormat", "“{0}%のプレイヤーがこの項目を達成しました”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“この項目を達成したプレイヤーは0.1％未満です”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- 獲得：{0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "非表示");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "もう少しプレイしたら明らかになりますよ");
		Unlocked = LOCTEXT("AchievementUnlocked", "完了");
		Locked = LOCTEXT("AchievementLocked", "ロック中");
		RarityCommon = LOCTEXT("AchievementCommon", "ノーマル");
		RarityUncommon = LOCTEXT("AchievementUncommon", "ハード");
		RarityRare = LOCTEXT("AchievementRare", "スーパーハード");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "エクストリーム");
		PopTitle = LOCTEXT("AchievementPopTitle", "項目達成おめでとう！");
		break;
	case ELanguageType::KO:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}업적중 {1}개 의 업적을 달성 하였습니다");
		DialogPercent = LOCTEXT("RarityFormat", "“{0}%유저가 이 업적을 달성 하였습니다”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“0.1% 미만의 유저가 이 성과를 달성 하였습니다”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- 획득: {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "숨김");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "계속해서 플레이를 하면 알 수 있습니다");
		Unlocked = LOCTEXT("AchievementUnlocked", "해제");
		Locked = LOCTEXT("AchievementLocked", "미해제");
		RarityCommon = LOCTEXT("AchievementCommon", "노멀");
		RarityUncommon = LOCTEXT("AchievementUncommon", "희귀");
		RarityRare = LOCTEXT("AchievementRare", "유니크");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "울트라 유니크");
		PopTitle = LOCTEXT("AchievementPopTitle", "축하합니다. 새로운 업적을 달성 하였습니다.");
		break;
	case ELanguageType::TH:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} ความสำเร็จ");
		DialogPercent = LOCTEXT("RarityFormat", "“{0}% ของผู้เล่นได้รับความสำเร็จนี”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“ผู้เล่นน้อยกว่า 0.1% ได้รับความสำเร็จนี้”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- สำเร็จเมื่อ {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "ความสำเร็จลับ");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "เล่นเพื่อปลดล็อกความสำเร็จลับ");
		Unlocked = LOCTEXT("AchievementUnlocked", "ปลดล็อก");
		Locked = LOCTEXT("AchievementLocked", "ล็อก");
		RarityCommon = LOCTEXT("AchievementCommon", "คอมมอน");
		RarityUncommon = LOCTEXT("AchievementUncommon", "แรร์");
		RarityRare = LOCTEXT("AchievementRare", "ซุปเปอร์แรร์");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "อัลตราแรร์");
		PopTitle = LOCTEXT("AchievementPopTitle", "ปลดล็อกความสำเร็จ!");
		break;
	case ELanguageType::ID:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} Achievement");
		DialogPercent = LOCTEXT("RarityFormat", "“{0}% pemain mendapat Achievement ini”");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "“Kurang dari 0.1% pemain memiliki achievement ini”");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- Mendapat achievement pada {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "Tersembunyi");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "Tetaplah bermain untuk membukanya");
		Unlocked = LOCTEXT("AchievementUnlocked", "UNLOCKED");
		Locked = LOCTEXT("AchievementLocked", "LOCKED");
		RarityCommon = LOCTEXT("AchievementCommon", "Common");
		RarityUncommon = LOCTEXT("AchievementUncommon", "Rare");
		RarityRare = LOCTEXT("AchievementRare", "Super Rare");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "Ultra Rare");
		PopTitle = LOCTEXT("AchievementPopTitle", "Mendapatkan achievement!");
		break;
	case ELanguageType::DE:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} Leistungen");
		DialogPercent = LOCTEXT("RarityFormat", "'{0}% der Spieler haben dies erreicht'");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "'Weniger als 0.1% Spieler haben das erreicht'");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- Abgeschlossen am {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "Verborgen");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "Spiele weiter, um es herauszufinden");
		Unlocked = LOCTEXT("AchievementUnlocked", "ENTSPERRT");
		Locked = LOCTEXT("AchievementLocked", "GESPERRT");
		RarityCommon = LOCTEXT("AchievementCommon", "Normal");
		RarityUncommon = LOCTEXT("AchievementUncommon", "Selten");
		RarityRare = LOCTEXT("AchievementRare", "Sehr selten");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "Ultra Selten");
		PopTitle = LOCTEXT("AchievementPopTitle", "Leistung freigeschaltet!");
		break;
	case ELanguageType::ES:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} Logros");
		DialogPercent = LOCTEXT("RarityFormat", "'{0}% de jugadores consiguieron esto'");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "'Menos del 0,1% de los jugadores lo consiguieron'");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- Conseguido en {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "Oculto");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "Sigue jugando para descubrirlo");
		Unlocked = LOCTEXT("AchievementUnlocked", "DESBLOQUEADO");
		Locked = LOCTEXT("AchievementLocked", "BLOQUEADO");
		RarityCommon = LOCTEXT("AchievementCommon", "Común");
		RarityUncommon = LOCTEXT("AchievementUncommon", "Raro");
		RarityRare = LOCTEXT("AchievementRare", "Súper raro");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "Ultra raro");
		PopTitle = LOCTEXT("AchievementPopTitle", "¡Logro desbloqueado!");
		break;
	case ELanguageType::FR:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} Réalisations");
		DialogPercent = LOCTEXT("RarityFormat", "'{0}% des joueurs ont ceci'");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "'Moins de 0,1 % des joueurs y sont parvenus'");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- Accompli le {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "Caché");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "Continuez à jouer pour le découvrir");
		Unlocked = LOCTEXT("AchievementUnlocked", "DÉBLOQUÉ");
		Locked = LOCTEXT("AchievementLocked", "VERROUILLÉ");
		RarityCommon = LOCTEXT("AchievementCommon", "Commun");
		RarityUncommon = LOCTEXT("AchievementUncommon", "Rare");
		RarityRare = LOCTEXT("AchievementRare", "Super rare");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "Ultra rare");
		PopTitle = LOCTEXT("AchievementPopTitle", "Réalisation débloquée !");
		break;
	case ELanguageType::PT:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} Conquistas");
		DialogPercent = LOCTEXT("RarityFormat", "'{0}% dos jogadores possui isso'");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "'Menos de 0.1% dos jogadores realizaram isso'");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- Realizado em {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "Oculto");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "Continue jogando para descobrir");
		Unlocked = LOCTEXT("AchievementUnlocked", "DESBLOQUEADO");
		Locked = LOCTEXT("AchievementLocked", "BLOQUEADO");
		RarityCommon = LOCTEXT("AchievementCommon", "Comum");
		RarityUncommon = LOCTEXT("AchievementUncommon", "Raro");
		RarityRare = LOCTEXT("AchievementRare", "Super Raro");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "Ultra Raro");
		PopTitle = LOCTEXT("AchievementPopTitle", "Conquista desbloqueada!");
		break;
	case ELanguageType::RU:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} достижения");
		DialogPercent = LOCTEXT("RarityFormat", "'Менее {0}% игроков приобрели это'");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "'Менее 0,1% игроков достигли этого'");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- Достигнуто {0}.{1}.{2} -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "Скрыто");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "Продолжайте играть, чтобы узнать");
		Unlocked = LOCTEXT("AchievementUnlocked", "ОТКРЫТО");
		Locked = LOCTEXT("AchievementLocked", "ЗАКРЫТО");
		RarityCommon = LOCTEXT("AchievementCommon", "Обыч");
		RarityUncommon = LOCTEXT("AchievementUncommon", "Редк");
		RarityRare = LOCTEXT("AchievementRare", "Очень редк.");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "Ультра редк.");
		PopTitle = LOCTEXT("AchievementPopTitle", "Достижение открыто!");
		break;
	case ELanguageType::TR:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} Başarı");
		DialogPercent = LOCTEXT("RarityFormat", "'Buna sahip olan oyuncuların oranı: {0}%'");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "'Oyuncuların %0,1'inden daha azı bunu başardı'");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- {0}.{1}.{2} tarihinde başarıldı -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "Gizli");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "Öğrenmek için oynamaya devam et");
		Unlocked = LOCTEXT("AchievementUnlocked", "KİLİDİ AÇILDI");
		Locked = LOCTEXT("AchievementLocked", "KİLİTLİ");
		RarityCommon = LOCTEXT("AchievementCommon", "Yaygın");
		RarityUncommon = LOCTEXT("AchievementUncommon", "Seyrek");
		RarityRare = LOCTEXT("AchievementRare", "Nadir");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "İstisnai");
		PopTitle = LOCTEXT("AchievementPopTitle", "Başarının kilidi açıldı!");
		break;
	case ELanguageType::VI:
		DialogStatus = LOCTEXT("AchievementProgressFormat", "{0}/{1} Thành tựu");
		DialogPercent = LOCTEXT("RarityFormat", "'{0}% người có cái này'");
		DialogPercentLow = LOCTEXT("RarityFormatLow", "'Ít hơn 0.1% người chơi đạt được thành tựu này'");
		DialogEarnedDate = LOCTEXT("CompleteTimeFormat", "- {0}.{1}.{2} đã nhận được -");
		InvisibleTitle = LOCTEXT("AchievementHiddenTitle", "Ẩn");
		InvisibleSubtitle = LOCTEXT("AchievementHiddenSubtitle", "Chơi tiếp để tìm hiểu");
		Unlocked = LOCTEXT("AchievementUnlocked", "MỞ");
		Locked = LOCTEXT("AchievementLocked", "KHÓA");
		RarityCommon = LOCTEXT("AchievementCommon", "Thường");
		RarityUncommon = LOCTEXT("AchievementUncommon", "Hiếm");
		RarityRare = LOCTEXT("AchievementRare", "Siêu Hiếm");
		RarityUltraRare = LOCTEXT("AchievementUltraRare", "Cực Hiếm");
		PopTitle = LOCTEXT("AchievementPopTitle", "Mở Thành tựu!");
		break;
	default: ;
	}
}

FAchievementLanguage* GAchievementLanguage = nullptr;

#undef LOCTEXT_NAMESPACE
