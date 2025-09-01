// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Interaction/Data/Fragments/CISInteractionFragmentBase.h"
#include "Shared/Data/CISCoreTypes.h"
#include "CISUseFragmentSingle.generated.h"


USTRUCT(DisplayName="Interaction Fragment - Single", BlueprintType)
struct COMMONINTERACTIONSYSTEM_API FCISInteractionFragmentSingle : public FCISInteractionFragmentBase
{
	GENERATED_BODY();
	
	FCISInteractionFragmentSingle():
		FCISInteractionFragmentBase(TAG_CIS_Interaction_Types_Single)
	{};
	
};
