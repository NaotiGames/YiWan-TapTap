// Fill out your copyright notice in the Description page of Project Settings.


#include "TapControllerInput.h"

#include "TUType.h"
#include "Controller/TapControllerManager.h"
#include "Framework/Application/SlateUser.h"
#include "Input/HittestGrid.h"
#include "Widgets/SViewport.h"


FTapControllerInput::FTapControllerInput()
{
	SetMode(AnalogCursorMode::Direct);
}

void FTapControllerInput::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
	FAnalogCursor::Tick(DeltaTime, SlateApp, Cursor);
	
	const float AnalogValsSize = AnalogValue.Size();
	if (AnalogValsSize > 0.0f)
	{
		const float TargetSize = FMath::Max(AnalogValsSize - DeadZone, 0.0f) / (1.0f - DeadZone);
		AnalogValue /= AnalogValsSize;
		AnalogValue *= TargetSize;
		
		TSharedPtr<SWidget> ScrollWidget = FTapControllerManager::Get().GetCanScrollWidget();
		if (ScrollWidget)
		{
			AnalogValue = DeltaTime * ScrollSpeed * AnalogValue;
				
			const FGeometry& Geometry = ScrollWidget->GetCachedGeometry();
			FVector2D Pos = Geometry.GetAbsolutePosition() + Geometry.GetAbsoluteSize() / 2.f;
			SlateApp.OnMouseWheel(AnalogValue.Y, Pos);
		}
	}
}

bool FTapControllerInput::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	UE_LOG(LogTap, Log, TEXT("KeyDown: %s, InputMode: %s, IsRepeat: %s"),
		*InKeyEvent.GetKey().ToString(),
		*UEnum::GetValueAsString(FTapControllerManager::Get().GetInputMode()),
		InKeyEvent.IsRepeat() ? TEXT("True") : TEXT("False"));

	if (InKeyEvent.IsRepeat())
	{
		return false;
	}

	TSharedPtr<FSlateUser> SlateUser = SlateApp.GetUser(InKeyEvent);
	if (!SlateUser)
	{
		return false;
	}

	TSharedPtr<FTapControllerWidgetData> Data = FTapControllerManager::Get().FindReceiveInputWidgetData();
	if (!Data)
	{
		UE_LOG(LogTap, Warning, TEXT("No Recv input data."));
		return false;
	}

	if (InKeyEvent.GetKey() == EKeys::Virtual_Back)
	{
		if (Data->OnBackButtonClicked.IsBound())
		{
			Data->OnBackButtonClicked.Execute();
			return true;
		}
	}
	if (FTapControllerManager::Get().GetInputMode() == ETapInputMode::Cursor)
	{
		return FAnalogCursor::HandleKeyDownEvent(SlateApp, InKeyEvent);
	}
	return false;
}

bool FTapControllerInput::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	UE_LOG(LogTap, Log, TEXT("KeyUp: %s-%s"), *InKeyEvent.GetKey().ToString(), *UEnum::GetValueAsString(FTapControllerManager::Get().GetInputMode()));

	if (FTapControllerManager::Get().GetInputMode() == ETapInputMode::Cursor)
	{
		return FAnalogCursor::HandleKeyUpEvent(SlateApp, InKeyEvent);
	}
	return false;
}

bool FTapControllerInput::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	UE_LOG(LogTap, Verbose, TEXT("AnalogInput: %s-%f"), *InAnalogInputEvent.GetKey().ToString(), InAnalogInputEvent.GetAnalogValue());
	if (InAnalogInputEvent.GetKey() == EKeys::Gamepad_RightY)
	{
		AnalogValue.Y = InAnalogInputEvent.GetAnalogValue();
		return true;
	}
	if (InAnalogInputEvent.GetKey() == EKeys::Gamepad_RightX)
	{
		AnalogValue.X = InAnalogInputEvent.GetAnalogValue();
		return true;
	}
	if (FTapControllerManager::Get().GetInputMode() == ETapInputMode::Focus)
	{
		EUINavigation Dir = FSlateApplication::Get().GetNavigationDirectionFromAnalog(InAnalogInputEvent);
		if (Dir != EUINavigation::Invalid)
		{
			if (TSharedPtr<FSlateUser> User = SlateApp.GetUser(0))
			{
				return TapUINavigationImpl(Dir, User.ToSharedRef());
			}
		}
		return false;
	}
	return FAnalogCursor::HandleAnalogInputEvent(SlateApp, InAnalogInputEvent);
}

void FTapControllerInput::UpdateCursorPosition(FSlateApplication& SlateApp, TSharedRef<FSlateUser> SlateUser, const FVector2D& NewPosition, bool bForce)
{
	if (FTapControllerManager::Get().GetInputMode() == ETapInputMode::Cursor)
	{
		const FVector2D OldPosition = SlateUser->GetCursorPosition();

		TSharedPtr<SWidget> Widget = FTapControllerManager::Get().GetCursorInstigatorWidget();
		UE_LOG(LogTap, Verbose, TEXT("OldPos: %s, NewPos: %s, CursorInstigator: %s"), *OldPosition.ToString(), *NewPosition.ToString(), Widget.IsValid() ? TEXT("Valid") : TEXT("Invalid"));
		if (Widget)
		{
			FVector2D Pos = NewPosition;
			
			if (!Widget->GetCachedGeometry().IsUnderLocation(NewPosition)) //光标出界，执行Nav
			{
				FSlateRect Rect = Widget->GetCachedGeometry().GetLayoutBoundingRect();
				FBox2D BoundBox = FBox2D(Rect.GetTopLeft(), Rect.GetBottomRight());
				Pos = BoundBox.GetClosestPointTo(NewPosition);
				
				EUINavigation Dir = EUINavigation::Invalid;
				FVector2D Offset = NewPosition - Pos;
				if (FMath::Abs(Offset.X) > FMath::Abs(Offset.Y))
				{
					if (Offset.X > 0.f)
					{
						Dir = EUINavigation::Right;
					}
					else
					{
						Dir = EUINavigation::Left;
					}
				}
				else
				{
					if (Offset.Y > 0.f)
					{
						Dir = EUINavigation::Down;
					}
					else
					{
						Dir = EUINavigation::Up;
					}
				}
				TapUINavigationImpl(Dir, SlateUser);
			}
			
			//make sure we are actually moving
			int32 NewIntPosX = Pos.X;
			int32 NewIntPosY = Pos.Y;
			int32 OldIntPosX = OldPosition.X;
			int32 OldIntPosY = OldPosition.Y;
			if (bForce || OldIntPosX != NewIntPosX || OldIntPosY != NewIntPosY)
			{
				SlateUser->SetCursorPosition(NewIntPosX, NewIntPosY);
				const FVector2D UpdatedPosition = SlateUser->GetCursorPosition();
				const bool bIsPrimaryUser = FSlateApplication::CursorUserIndex == SlateUser->GetUserIndex();
				FPointerEvent MouseEvent(
					SlateUser->GetUserIndex(),
					UpdatedPosition,
					OldPosition,
					bIsPrimaryUser ? SlateApp.GetPressedMouseButtons() : TSet<FKey>(),
					EKeys::Invalid,
					0,
					bIsPrimaryUser ? SlateApp.GetModifierKeys() : FModifierKeysState()
				);

				SlateApp.ProcessMouseMoveEvent(MouseEvent, true);
			}
		}
		else
		{
			FTapControllerManager::Get().ChangeInputModeFocus();
			FTapControllerManager::Get().FocusToStartupWidget();
		}
	}
}

bool FTapControllerInput::TapUINavigationImpl(EUINavigation Direction, TSharedRef<FSlateUser> SlateUser)
{
	UE_LOG(LogTap, Log, TEXT("UI navigation: %s"), *UEnum::GetValueAsString(Direction));
	TSharedPtr<SWidget> FocusWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();

	if (TSharedPtr<FTapControllerWidgetData> Data = FTapControllerManager::Get().FindReceiveInputWidgetData())
	{
		if (TSharedPtr<SWidget> RootWidget = Data->RootWidget.Pin())
		{
			FWidgetPath FocusPath;
			if (FocusWidget)
			{
				FSlateApplication::Get().FindPathToWidget(FocusWidget.ToSharedRef(), FocusPath);
			}
				
#if ENGINE_MAJOR_VERSION >= 5
			if (!FocusPath.ContainsWidget(RootWidget.Get()) || (FocusWidget && FocusWidget->GetType() == TEXT("SViewport")))
#else
			if (!FocusPath.ContainsWidget(RootWidget.ToSharedRef()) || (FocusWidget && FocusWidget->GetType() == TEXT("SViewport")))
#endif
			{
				FTapControllerManager::Get().FocusToStartupWidget();
				return true;
			}
			else
			{
				UE_LOG(LogTap, Log, TEXT("ProcessReply"));
				FSlateApplication::Get().ProcessReply(FocusPath, FReply::Handled().SetNavigation(Direction, ENavigationGenesis::Controller), nullptr, nullptr);
				if (FocusWidget == FSlateApplication::Get().GetKeyboardFocusedWidget() && FocusWidget->GetType() != TEXT("SComboListType"))
				{
					if (!SlateUser->ShouldShowFocus(FocusWidget))
					{
						UE_LOG(LogTap, Log, TEXT("Refresh focus."));
						FSlateApplication::Get().ClearKeyboardFocus();
						FSlateApplication::Get().SetKeyboardFocus(FocusWidget, EFocusCause::Navigation);
					}
				}
				return true;
			}
		}
	}
	return false;
}
