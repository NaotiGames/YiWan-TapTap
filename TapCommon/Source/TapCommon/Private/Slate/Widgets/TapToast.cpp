// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Widgets/TapToast.h"

#include "SlateOptMacros.h"
#include "TapCommon.h"
#include "TapSubsystem.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SConstraintCanvas.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

TWeakPtr<STapToast> STapToast::WhiteToast;
TWeakPtr<STapToast> STapToast::BlackToast;

// 和Curve同步
#define FADE_OUT_TIME 0.3f

void STapToast::Construct(const FArguments& InArgs,  const FTapToastStyle* InStyle, float Offset)
{
	check(InStyle);

	// setup toast brush instance. 
	Brush = InStyle->IconBrushTemplate;
	if (UMaterialInterface* Mat = Cast<UMaterialInterface>(Brush.GetResourceObject()))
	{
		GCMat = UMaterialInstanceDynamic::Create(Mat, nullptr);
	}
	Brush.SetResourceObject(GCMat);

	// init anim
	Anim.AddCurve(0.f, FADE_OUT_TIME, ECurveEaseFunction::QuadOut);
	
	ChildSlot
	[
		SNew(SConstraintCanvas)
		+ SConstraintCanvas::Slot()
		  .Anchors(FAnchors(0.5f, 0.15f))
		  .Offset(FMargin(0.f, Offset))
		  .Alignment(0.5f)
		  .AutoSize(true)
		[
			SNew(SBox)
					.HeightOverride(InStyle->Height)
					.MaxDesiredWidth(1256.f)
			[
				SNew(SBorder)
						.RenderTransform(this, &STapToast::GetToastRenderTransform)
						.Padding(FMargin(12.f, 0.f))
						.VAlign(VAlign_Center)
						.BorderImage(&InStyle->Background)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					  .VAlign(VAlign_Center)
					  .AutoWidth()
					  .Padding(0.f, 0.f, 8.f, 0.f)
					[
						SAssignNew(IconImage, SImage).Image(&Brush)
					]
					+ SHorizontalBox::Slot()
					  .VAlign(VAlign_Center)
					  .AutoWidth()
					[
						SAssignNew(MessageTextBlock, STextBlock)
								.TextStyle(&InStyle->ContentStyle)
					]
				]
			]
		]
	];

	SetVisibility(EVisibility::SelfHitTestInvisible);
}

TSharedPtr<STapToast> STapToast::Show(ETapToastType Type, const FText& Message, const FString& WebUrl)
{
	enum class EToastStyle
	{
		White,
		Black,
		Achievement, // not support yet
	};

	// determine style-type
	EToastStyle StyleType = EToastStyle::White;
	switch (Type)
	{
	case ETapToastType::Info:
	case ETapToastType::Success:
	case ETapToastType::Error:
	case ETapToastType::Warning:
		StyleType = EToastStyle::White;
		break;
	case ETapToastType::TapTap:
		StyleType = EToastStyle::Black;
		break;
	default:
		return nullptr;
	}

	// find toast ptr
	TWeakPtr<STapToast>* ToastPtr = nullptr;
	switch (StyleType)
	{
	case EToastStyle::White:
		ToastPtr = &WhiteToast;
		break;
	case EToastStyle::Black:
		ToastPtr = &BlackToast;
		break;
	default:
		ensure(false);
		return nullptr;
	}

	TSharedPtr<STapToast> Toast = ToastPtr->Pin();
	TSharedRef<FSlateStyleSet> StyleSet = FModuleManager::GetModuleChecked<FTapCommonModule>("TapCommon").Style.ToSharedRef();
	if (!Toast) // create toast
	{
		const FTapToastStyle* Style = nullptr;
		float Offset = 0.f;
		switch (StyleType)
		{
		case EToastStyle::White:
			Style = &StyleSet->GetWidgetStyle<FTapToastStyle>("/Toast/MessageStyle");
			Offset = -45.f;
			break;
		case EToastStyle::Black:
			Style = &StyleSet->GetWidgetStyle<FTapToastStyle>("/Toast/TapTapStyle");
			break;
		default:
			ensure(false);
			return nullptr;
		}
		Toast = SNew(STapToast, Style, Offset);
	}

	check(Toast);
	
	// find icon brush
	const FSlateBrush* IconBrushP = nullptr;
	switch (Type)
	{
	case ETapToastType::Info:
		IconBrushP = StyleSet->GetBrush("/Toast/Info");
		break;
	case ETapToastType::Success:
		IconBrushP = StyleSet->GetBrush("/Toast/Success");
		break;
	case ETapToastType::Error:
		IconBrushP = StyleSet->GetBrush("/Toast/Error");
		break;
	case ETapToastType::Warning:
		IconBrushP = StyleSet->GetBrush("/Toast/Warning");
		break;
	case ETapToastType::TapTap:
		IconBrushP = StyleSet->GetBrush("/Toast/TapTap");
		break;
	default: ;
	}

	Toast->UpdateToast(IconBrushP, Message, 3.f, WebUrl);

	if (!ToastPtr->IsValid())
	{
		UTapSubsystem::AddWidget(Toast.ToSharedRef());
		*ToastPtr = Toast; 
	}
	return Toast;
}

void STapToast::UpdateToast(const FSlateBrush* IconBrush, const FText& Message, float Duration, const FString& Url)
{
	if (IconBrush)
	{
		UpdateIcon(IconBrush);
	}
	
	if (!Url.IsEmpty())
	{
		FTapCommonModule::AsyncDownloadImage(Url, FAsyncDownloadImage::CreateSP(this, &STapToast::UpdateIcon));
	}

	MessageTextBlock->SetText(Message);
		
	Fadein();

	RefreshDurationTimer(Duration);
}

void STapToast::UpdateIcon(const FSlateBrush* IconBrush)
{
	UTexture2D* Tex = Cast<UTexture2D>(IconBrush->GetResourceObject());
	if (Tex && GCMat)
	{
		UpdateIcon(Tex);
	}
	else
	{
		IconImage->SetImage(IconBrush);
	}
}

void STapToast::UpdateIcon(UTexture2D* Texture)
{
	if (Texture && ensureMsgf(GCMat, TEXT("Toast style's icon-brush resource-object must not be material, if update with texture.")))
	{
		GCMat->SetTextureParameterValue("Texture", Texture);
		IconImage->SetImage(&Brush);
		ensure(Brush.GetResourceObject() == GCMat);
	}
}

void STapToast::Fadein()
{
	if (Anim.IsPlaying())
	{
		Anim.Play(AsShared(), false, Anim.GetSequenceTime());
	}
	else
	{
		if (Anim.IsAtStart())
		{
			Anim.Play(AsShared());
		}
	}
}

void STapToast::Fadeout()
{
	Anim.Reverse();
}

TOptional<FSlateRenderTransform> STapToast::GetToastRenderTransform() const
{
	float Offset = (1 - Anim.GetLerp()) * -500.f;
	return FSlateRenderTransform(FVector2D(0.f, Offset));
}

EActiveTimerReturnType STapToast::OnToastTimer(double InCurrentTime, float InDeltaTime)
{
	UTapSubsystem::RemoveWidget(AsShared());
	ToastTimer.Reset();
	return EActiveTimerReturnType::Stop;
}

EActiveTimerReturnType STapToast::OnFadeoutTimer(double InCurrentTime, float InDeltaTime)
{
	Fadeout();
	FadeoutTimer.Reset();
	return EActiveTimerReturnType::Stop;
}

void STapToast::RefreshDurationTimer(float Duration)
{
	if (ToastTimer)
	{
		UnRegisterActiveTimer(ToastTimer.ToSharedRef());
	}
	ToastTimer = RegisterActiveTimer(Duration + FADE_OUT_TIME, FWidgetActiveTimerDelegate::CreateSP(this, &STapToast::OnToastTimer));
	if (FadeoutTimer)
	{
		UnRegisterActiveTimer(FadeoutTimer.ToSharedRef());
	}
	FadeoutTimer = RegisterActiveTimer(Duration, FWidgetActiveTimerDelegate::CreateSP(this, &STapToast::OnFadeoutTimer));
}

void STapToast::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(GCMat);
}

FString STapToast::GetReferencerName() const
{
	return TEXT("STapToast");
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
