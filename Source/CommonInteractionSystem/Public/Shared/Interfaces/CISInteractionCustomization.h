// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Data/CISCoreTypes.h"
#include "UObject/Interface.h"
#include "CISInteractionCustomization.generated.h"


UINTERFACE(DisplayName="Interaction Customization")
class UCISInteractionCustomization : public UInterface
{
	GENERATED_BODY()
};


/**
 * Extra settings pulled from this interface that interactable actors can implement 
 */
class COMMONINTERACTIONSYSTEM_API ICISInteractionCustomization
{
	GENERATED_BODY()

public:
	/**
	 * Used when we try to interact with an actor
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="CommonInteractionSystem")
	bool CanInteractWith(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags) const;

	/**
	 * Should we show the focus widget when StartFocus is called ?
	 * Also used when refreshing focus status.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="CommonInteractionSystem")
	bool CanFocusWith(APawn* SourcePawn, UPARAM(ref) FCISInteractionFocusParams& OutCustomizationFocusParams) const;
};
