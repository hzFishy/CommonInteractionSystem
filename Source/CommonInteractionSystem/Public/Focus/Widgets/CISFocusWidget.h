// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "CISFocusWidget.generated.h"


/**
 * Widget instantiated on focus interactions
 */
UCLASS(ClassGroup=(FocusWidget),DisplayName="Focus Widget")
class COMMONINTERACTIONSYSTEM_API UCISFocusWidget : public UUserWidget
{
	GENERATED_BODY()


	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
public:
	void UpdateContent(const FText& FocusText, FGameplayTag IconFocusTag);

	void OnSingleInteraction(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, bool bInteractionSucceeded);
	
	void OnHoldInteractionStarted(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, bool bInteractionSucceeded, float TimeToHold);

	void OnHoldInteractionEnded(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, bool bSuccess);
	
protected:
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Update Content")
	void K2_UpdateContent(const FText& FocusText, FGameplayTag IconFocusTag);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Single Interaction")
	void K2_OnSingleInteraction(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Hold Interaction Started")
	void K2_OnHoldInteractionStarted(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, bool bInteractionSucceeded, float TimeToHold);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Hold Interaction Ended")
	void K2_OnHoldInteractionEnded(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, bool bSuccess);
};
