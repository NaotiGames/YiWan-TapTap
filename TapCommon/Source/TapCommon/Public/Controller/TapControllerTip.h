// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TUType.h"
#include "Widgets/SCompoundWidget.h"

class SUniformGridPanel;
enum class ETapControllerType : uint8;

UENUM()
enum class ETapControllerTipIconType : uint8
{
	None		= 0,
	Scroll		= 1 << 0,
	Back		= 1 << 1,
	Accept		= 1 << 2,
};
ENUM_CLASS_FLAGS(ETapControllerTipIconType);

/**
 * 
 */
class TAPCOMMON_API STapControllerTip : public SCompoundWidget
{
public:
	virtual ~STapControllerTip() override;
	
	SLATE_BEGIN_ARGS(STapControllerTip)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void UpdateControllerType(TOptional<ETapControllerType> InControllerType);

	void UpdateTipIconTypes(ETapControllerTipIconType NewTypes);

	FORCEINLINE ETapControllerTipIconType GetTipIconTypes() const { return IconTypes; }
	
	FORCEINLINE void AddTipIconTypes(ETapControllerTipIconType InTypes) { UpdateTipIconTypes(GetTipIconTypes() | InTypes); }

	FORCEINLINE void RemoveTipIconTypes(ETapControllerTipIconType InTypes) { UpdateTipIconTypes(GetTipIconTypes() & ~InTypes); }

	FORCEINLINE void SetForceLanguageType(const TOptional<ELanguageType>& InType) { LanguageType = InType; }
	
protected:
	void UpdateIcons();
	
	static const FSlateBrush* FindIconBrush(ETapControllerTipIconType InType, ETapControllerType InControllerType);

	FText GetIconText(ETapControllerTipIconType InType, ELanguageType Language);
	
	TOptional<ETapControllerType> ControllerType;
	
	ETapControllerTipIconType IconTypes = ETapControllerTipIconType::None;

	TSharedPtr<SUniformGridPanel> Panel;
	
	TOptional<ELanguageType> LanguageType;
};
