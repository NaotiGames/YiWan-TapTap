// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/TapControllerTip.h"

#include "SlateOptMacros.h"
#include "TapCommon.h"
#include "TULanguage.h"
#include "Controller/TapControllerManager.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SUniformGridPanel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

STapControllerTip::~STapControllerTip()
{
	FTapControllerManager::Get().OnTapControllerTypeChanged().RemoveAll(this);
}

void STapControllerTip::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FTapCommonModule::Get().Style->GetBrush("/Controller/ControllerTipBG"))
		.Padding(FMargin(32.f, 8.f))
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(FTapCommonModule::Get().Style->GetBrush("/Controller/controller_tip_logo"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.f)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				SAssignNew(Panel, SUniformGridPanel)
				.MinDesiredSlotWidth(104.f)
			]
		]
	];

	FTapControllerManager::Get().OnTapControllerTypeChanged().AddSP(this, &STapControllerTip::UpdateControllerType);
	UpdateControllerType(FTapControllerManager::Get().GetControllerType());
}

void STapControllerTip::UpdateControllerType(TOptional<ETapControllerType> InControllerType)
{
	ControllerType = InControllerType;
	UpdateIcons();
}

void STapControllerTip::UpdateTipIconTypes(ETapControllerTipIconType NewTypes)
{
	IconTypes = NewTypes;
	UpdateIcons();
}

void STapControllerTip::UpdateIcons()
{
	if (ControllerType)
	{
		SetVisibility(EVisibility::HitTestInvisible);

		ELanguageType Language = TULanguage::GetCurrentType();
		const FTextBlockStyle& TextStyle = FTapCommonModule::Get().Style->GetWidgetStyle<FTextBlockStyle>("/Controller/ControllerTipText");
		
		Panel->ClearChildren();
		UEnum* Enum = StaticEnum<ETapControllerTipIconType>();
		int32 Column = 0;
		for (int32 i = 1; i < Enum->NumEnums() - 1; ++i)
		{
			ETapControllerTipIconType Type = static_cast<ETapControllerTipIconType>(Enum->GetValueByIndex(i));
			if (EnumHasAnyFlags(IconTypes, Type))
			{
				if (const FSlateBrush* Brush = FindIconBrush(Type, *ControllerType))
				{
					Panel->AddSlot(Column++, 0)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.Padding(0.f, 0.f, 6.f, 0.f)
						.AutoWidth()
						[
							SNew(SImage)
							.Image(Brush)
						]
						+SHorizontalBox::Slot()
						.FillWidth(1.f)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Text(GetIconText(Type, Language))
							.TextStyle(&TextStyle)
						]
					];
				}
				else
				{
					ensure(false);
				}
			}
		}
	}
	else
	{
		SetVisibility(EVisibility::Hidden);
	}
}

#define ENUM_SHORT_NAME(EnumType, TypeValue) (StaticEnum<EnumType>()->GetNameStringByIndex(StaticEnum<EnumType>()->GetIndexByValue(static_cast<int64>(TypeValue))))

const FSlateBrush* STapControllerTip::FindIconBrush(ETapControllerTipIconType InType, ETapControllerType InControllerType)
{
	
	FString PropertyNameStr = FString::Printf(TEXT("/Controller/controller_tip_%s_%s"), *ENUM_SHORT_NAME(ETapControllerTipIconType, InType), *ENUM_SHORT_NAME(ETapControllerType, InControllerType));
	return FTapCommonModule::Get().Style->GetBrush(FName(PropertyNameStr));
}

#define LOCTEXT_NAMESPACE "TapTap"
FText STapControllerTip::GetIconText(ETapControllerTipIconType InType, ELanguageType Language)
{
	if (LanguageType.IsSet())
	{
		Language = *LanguageType;
	}
	switch (InType)
	{
	case ETapControllerTipIconType::Accept:
		switch (Language)
		{
		case ELanguageType::ZH: return LOCTEXT("ControllerTipAccept", "选择");
		case ELanguageType::EN: return LOCTEXT("ControllerTipAccept", "Select");
		case ELanguageType::ZHTW: return LOCTEXT("ControllerTipAccept", "選擇");
		case ELanguageType::JA: return LOCTEXT("ControllerTipAccept", "選ぶ");
		case ELanguageType::KO: return LOCTEXT("ControllerTipAccept", "선택하다");
		case ELanguageType::TH: return LOCTEXT("ControllerTipAccept", "เลือก");
		case ELanguageType::ID: return LOCTEXT("ControllerTipAccept", "memilih");
		case ELanguageType::DE: return LOCTEXT("ControllerTipAccept", "wählen");
		case ELanguageType::ES: return LOCTEXT("ControllerTipAccept", "elegir");
		case ELanguageType::FR: return LOCTEXT("ControllerTipAccept", "choisir");
		case ELanguageType::PT: return LOCTEXT("ControllerTipAccept", "escolher");
		case ELanguageType::RU: return LOCTEXT("ControllerTipAccept", "выбирать");
		case ELanguageType::TR: return LOCTEXT("ControllerTipAccept", "seçmek");
		case ELanguageType::VI: return LOCTEXT("ControllerTipAccept", "chọn");
		default: ;
		}
		break;
	case ETapControllerTipIconType::Back:
		switch (Language)
		{
		case ELanguageType::ZH: return LOCTEXT("ControllerTipBack", "关闭");
		case ELanguageType::EN: return LOCTEXT("ControllerTipBack", "Close");
		case ELanguageType::ZHTW: return LOCTEXT("ControllerTipBack", "關閉");
		case ELanguageType::JA: return LOCTEXT("ControllerTipBack", "近い");
		case ELanguageType::KO: return LOCTEXT("ControllerTipBack", "닫다");
		case ELanguageType::TH: return LOCTEXT("ControllerTipBack", "ปิด");
		case ELanguageType::ID: return LOCTEXT("ControllerTipBack", "Menutup");
		case ELanguageType::DE: return LOCTEXT("ControllerTipBack", "Schließen");
		case ELanguageType::ES: return LOCTEXT("ControllerTipBack", "Cerca");
		case ELanguageType::FR: return LOCTEXT("ControllerTipBack", "Fermer");
		case ELanguageType::PT: return LOCTEXT("ControllerTipBack", "Fechar");
		case ELanguageType::RU: return LOCTEXT("ControllerTipBack", "Закрывать");
		case ELanguageType::TR: return LOCTEXT("ControllerTipBack", "Kapalı");
		case ELanguageType::VI: return LOCTEXT("ControllerTipBack", "Đóng");
		default: ;
		}
		break;
	case ETapControllerTipIconType::Scroll:
		switch (Language)
		{
		case ELanguageType::ZH: return LOCTEXT("ControllerTipScroll", "滚动页面");
		case ELanguageType::EN: return LOCTEXT("ControllerTipScroll", "Scroll page");
		case ELanguageType::ZHTW: return LOCTEXT("ControllerTipScroll", "捲動頁面");
		case ELanguageType::JA: return LOCTEXT("ControllerTipScroll", "スクロール");
		case ELanguageType::KO: return LOCTEXT("ControllerTipScroll", "스크롤");
		case ELanguageType::TH: return LOCTEXT("ControllerTipScroll", "เลื่อน");
		case ELanguageType::ID: return LOCTEXT("ControllerTipScroll", "Menggulir");
		case ELanguageType::DE: return LOCTEXT("ControllerTipScroll", "Scrollen");
		case ELanguageType::ES: return LOCTEXT("ControllerTipScroll", "Desplazarse");
		case ELanguageType::FR: return LOCTEXT("ControllerTipScroll", "Faire défiler");
		case ELanguageType::PT: return LOCTEXT("ControllerTipScroll", "Rolagem");
		case ELanguageType::RU: return LOCTEXT("ControllerTipScroll", "Прокрутка");
		case ELanguageType::TR: return LOCTEXT("ControllerTipScroll", "Sayfayı kaydır");
		case ELanguageType::VI: return LOCTEXT("ControllerTipScroll", "Cuộn");
		default: ;
		}
		break;
	default: ;
	}
	return FText::GetEmpty();
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
