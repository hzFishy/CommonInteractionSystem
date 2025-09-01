// By hzFishy - 2025 - Do whatever you want with it.


#include "Shared/Data/CISCoreTypes.h"


	/*----------------------------------------------------------------------------
		Logging
	----------------------------------------------------------------------------*/
DEFINE_LOG_CATEGORY(LogCISValidation);


	/*----------------------------------------------------------------------------
		Gameplay Tags
	----------------------------------------------------------------------------*/
UE_DEFINE_GAMEPLAY_TAG(TAG_CIS_Interaction_Events_AbilityActivate, "CIS.Interaction.Events.AbilityActivate");
UE_DEFINE_GAMEPLAY_TAG(TAG_CIS_Interaction_Types_Single, "CIS.Interaction.Types.Single");
UE_DEFINE_GAMEPLAY_TAG(TAG_CIS_Interaction_Types_Hold, "CIS.Interaction.Types.Hold");
UE_DEFINE_GAMEPLAY_TAG(TAG_CIS_Shared_Icons_DefaultInteraction, "CIS.Shared.Icons.DefaultInteraction");


	/*----------------------------------------------------------------------------
		Types
	----------------------------------------------------------------------------*/
FCISInteractionFocusParams::FCISInteractionFocusParams():
	bIsTest(false)
{}

const FText& FCISInteractionFocusParams::GetFinalFocusText() const
{
	return OverrideText.IsEmpty() ? DefaultText : OverrideText;
}
