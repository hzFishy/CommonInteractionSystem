// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Interaction/Data/Fragments/CISInteractionFragmentBase.h"
#include "Shared/Data/CISCoreTypes.h"
#include "FBPGUseFragmentHold.generated.h"


USTRUCT(DisplayName="Interaction Fragment - Hold", BlueprintType)
struct COMMONINTERACTIONSYSTEM_API FCISInteractionFragmentHold : public FCISInteractionFragmentBase
{
	GENERATED_BODY();

public:
	FCISInteractionFragmentHold():
		FCISInteractionFragmentBase(TAG_CIS_Interaction_Types_Hold), HoldTime(1)
	{};

	/* Time to hold the interaction to complete */
	UPROPERTY(EditAnywhere, meta = (ClampMin=0))
	float HoldTime;
};
