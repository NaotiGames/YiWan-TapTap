#pragma once

#include "CoreMinimal.h"
#include "AAUBackViewWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "AAUMobileTipWidget.generated.h"

class UTapButton;
class UNativeWidgetHost;
class STapControllerTip;
/**
 *  
 */
UCLASS()
class ANTIADDICTION_API UAAUMobileTipWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	static UAAUMobileTipWidget* ShowUI();

	void SetContent(const FString& Title, const FString& Content, const FString& ComfirmStr);
	
	TFunction<void()> ComformBlock;
	
protected:
	virtual void NativeOnInitialized() override;
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
	
	UFUNCTION()
	void OnSubmitBtnClick();

	TSharedPtr<STapControllerTip> ControllerTip;

	UPROPERTY(Meta = (BindWidget))
	UNativeWidgetHost* ControllerTipHost;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleLabel;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DetailLabel;

	UPROPERTY(meta = (BindWidget))
	USizeBox* ContentBox;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SubButtonLabel;

	UPROPERTY(meta = (BindWidget))
	UTapButton* SubmitBtn;

private:

	
};
