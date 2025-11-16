// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/TapStyle.h"

#include "TapCommon.h"
#include "TUType.h"

TSharedPtr<FSlateBrush> FTapStyle::MakeRoundCornersBrush(bool bHalfHeightRadius, float CornerRadius, UTexture2D* IconTexture)
{
	TSharedRef<FSlateStyleSet> CommonStyleSet = FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style.ToSharedRef();
	if (const FSlateBrush* Brush = CommonStyleSet->GetBrush("/Common/RoundCorners"))
	{
		if (UMaterialInterface* Mat = Cast<UMaterialInterface>(Brush->GetResourceObject()))
		{
			if (UMaterialInstanceDynamic* DMat = UMaterialInstanceDynamic::Create(Mat, nullptr))
			{
				TSharedPtr<FSlateBrush> DynamicBrush = MakeShared<FSlateBrush>(*Brush);
				DynamicBrush->SetResourceObject(DMat);
				if (bHalfHeightRadius)
				{
					DynamicBrush->DrawAs = ESlateBrushDrawType::Image;
				}
				else
				{
					DynamicBrush->DrawAs = ESlateBrushDrawType::Box;
					DynamicBrush->Margin = 0.5f;
					DynamicBrush->ImageSize = FVector2D(CornerRadius * 2.f);
				}

				if (IconTexture)
				{
					DMat->SetScalarParameterValue("Texture", 1.f);
					DMat->SetTextureParameterValue("Texture", IconTexture);
				}
				else
				{
					DMat->SetScalarParameterValue("Texture", 0.f);
				}
				
				return DynamicBrush;
			}
		}
	}

	UE_LOG(LogTap, Warning, TEXT("Round corners image construct failed."));
	return nullptr;
}

const FSlateBrush* FTapStyle::GetUnitBrush()
{
	TSharedRef<FSlateStyleSet> CommonStyleSet = FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style.ToSharedRef();
	return CommonStyleSet->GetBrush("/Common/UnitBrush");
}

