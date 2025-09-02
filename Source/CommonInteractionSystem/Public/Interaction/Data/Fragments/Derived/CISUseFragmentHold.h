// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Interaction/Data/Fragments/CISInteractionFragmentBase.h"
#include "Shared/Data/CISCoreTypes.h"
#include "CISUseFragmentHold.generated.h"


USTRUCT(DisplayName="Interaction Fragment - Hold", BlueprintType)
struct COMMONINTERACTIONSYSTEM_API FCISInteractionFragmentHold : public FCISInteractionFragmentBase
{
	GENERATED_BODY();

public:
	FCISInteractionFragmentHold():
		FCISInteractionFragmentBase(TAG_CIS_Interaction_Types_Hold),
		HoldTime(1),
		bDisableMovementInputWhileHold(false),
		bDisableLookInputWhileHold(false)
	{};

	/* Time to hold the interaction to complete */
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem", meta = (ClampMin=0))
	float HoldTime;

	/**
	 * Block movement input while the user is holding the interaction hold key.
	 * This uses SetIgnoreMoveInput on the Controller of the OwnerSourcePawn.
	 * See SetIgnoreMoveInput and ResetIgnoreMoveInput to manually start/stop ignoring movement input in your code.
	 */
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem")
	bool bDisableMovementInputWhileHold;
	
	/**
	 * Block look input while the user is holding the interaction hold key
	 * This uses SetIgnoreLookInput on the Controller of the OwnerSourcePawn.
	 * See SetIgnoreLookInput and ResetIgnoreLookInput to manually start/stop ignoring movement input in your code.
	 */
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem")
	bool bDisableLookInputWhileHold;
};
