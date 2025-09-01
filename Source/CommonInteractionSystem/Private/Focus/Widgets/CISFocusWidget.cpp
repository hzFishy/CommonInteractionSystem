// By hzFishy - 2025 - Do whatever you want with it.


#include "Focus/Widgets/CISFocusWidget.h"


	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
void UCISFocusWidget::UpdateContent(const FText& FocusText, FGameplayTag IconFocusTag)
{
	K2_UpdateContent(FocusText, IconFocusTag);
}

void UCISFocusWidget::OnSingleInteraction(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, bool bInteractionSucceeded)
{
	K2_OnSingleInteraction(SourcePawn, SourceInteractionTags);
}

void UCISFocusWidget::OnHoldInteractionStarted(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, bool bInteractionSucceeded, float TimeToHold)
{
	K2_OnHoldInteractionStarted(SourcePawn, SourceInteractionTags, bInteractionSucceeded, TimeToHold);
}

void UCISFocusWidget::OnHoldInteractionEnded(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, bool bSuccess)
{
	K2_OnHoldInteractionEnded(SourcePawn, SourceInteractionTags, bSuccess);
}
