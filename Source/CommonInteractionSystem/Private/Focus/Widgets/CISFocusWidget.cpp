// By hzFishy - 2025 - Do whatever you want with it.


#include "Focus/Widgets/CISFocusWidget.h"


	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
void UCISFocusWidget::UpdateContent(const FText& FocusText, FGameplayTag IconFocusTag)
{
	K2_UpdateContent(FocusText, IconFocusTag);
}

void UCISFocusWidget::OnSingleInteraction(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, const FGameplayTagContainer& FocusTags, bool bInteractionSucceeded)
{
	K2_OnSingleInteraction(SourcePawn, SourceInteractionTags, FocusTags, bInteractionSucceeded);
}

void UCISFocusWidget::OnHoldInteractionStarted(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, const FGameplayTagContainer& FocusTags, bool bInteractionSucceeded, float TimeToHold)
{
	K2_OnHoldInteractionStarted(SourcePawn, SourceInteractionTags, FocusTags, bInteractionSucceeded, TimeToHold);
}

void UCISFocusWidget::OnHoldInteractionEnded(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, const FGameplayTagContainer& FocusTags, bool bSuccess)
{
	K2_OnHoldInteractionEnded(SourcePawn, SourceInteractionTags, FocusTags, bSuccess);
}
