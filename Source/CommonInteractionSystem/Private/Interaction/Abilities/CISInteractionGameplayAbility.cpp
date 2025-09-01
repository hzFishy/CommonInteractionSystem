// By hzFishy - 2025 - Do whatever you want with it.


#include "Interaction/Abilities/CISInteractionGameplayAbility.h"

#include "Interaction/Data/TargetData/CISInteractionTargetData.h"
#include "Shared/Component/CISSourcePawnInteractionComponent.h"
#include "Shared/Data/CISCoreTypes.h"


UCISInteractionGameplayAbility::UCISInteractionGameplayAbility()
{
	auto DefaultTrigger = FAbilityTriggerData();
	DefaultTrigger.TriggerTag = TAG_CIS_Interaction_Events_AbilityActivate;
	AbilityTriggers.Emplace(DefaultTrigger);
}

const UCISInteractionAbilityEventData* UCISInteractionGameplayAbility::GetInteractionAbilityEventData() const
{
	return Cast<UCISInteractionAbilityEventData>(CurrentEventData.OptionalObject);
}

void UCISInteractionGameplayAbility::ExecuteInteraction(const FGameplayTagContainer& SourceInteractionTags)
{
	// TODO (log)
	if (auto* Avatar = GetAvatarActorFromActorInfo())
	{
		if (auto* InteractionComponent = Avatar->FindComponentByClass<UCISSourcePawnInteractionComponent>())
		{
			InteractionComponent->TryInteraction(SourceInteractionTags);
		}
	}
}
