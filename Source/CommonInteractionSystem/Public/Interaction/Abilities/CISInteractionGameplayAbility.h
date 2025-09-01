// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CISInteractionGameplayAbility.generated.h"
class UCISInteractionAbilityEventData;


/**
 * Process explained:
 * 
 * For Single Interaction:
 * - PawnComp: Input
 * - PawnComp: Activate Ability from event with payload data
 * - Ability: On Activation run Commit
 * - Ability: Execute interaction on PawnComp (ExecuteInteraction)
 * - Ability: End Ability
 *
 * For Hold Interaction:
 * - PawnComp: Input
 * - PawnComp: Activate Ability from event with payload data
 * - Ability: On Activation run Commit
 * - Ability: Execute interaction on PawnComp (ExecuteInteraction), this will start the hold logic
 * - PawnComp: Wait for the given hold time then do interaction
 * - Ability: If canceled by GAS end hold logic on PawnComp
 * - PawnComp: If the hold is canceled or completes we end this ability
 */
UCLASS(Classgroup=CommonInteractionSystem, DisplayName="Interaction Gameplay Ability")
class COMMONINTERACTIONSYSTEM_API UCISInteractionGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UCISInteractionGameplayAbility();

	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem")
	const UCISInteractionAbilityEventData* GetInteractionAbilityEventData() const;
	
	/**
	 * Execute the interaction process on the pawn interaction component.
	 * The tags should include the tags that were given to the pawn input interaction function
	 * The user can add more tags
	 */
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem")
	void ExecuteInteraction(const FGameplayTagContainer& SourceInteractionTags);
};
