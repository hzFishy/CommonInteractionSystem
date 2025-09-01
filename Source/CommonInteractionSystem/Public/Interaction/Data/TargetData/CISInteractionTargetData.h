// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Abilities/GameplayAbilityTypes.h"
#include "CISInteractionTargetData.generated.h"


UCLASS(BlueprintType, DisplayName="Interaction Ability Target Data")
class COMMONINTERACTIONSYSTEM_API UCISInteractionAbilityEventData : public UObject
{
	GENERATED_BODY()

public:
	UCISInteractionAbilityEventData();
	
	UPROPERTY(BlueprintReadOnly, Category="CommonInteractionSystem|Hold")
	FGameplayTag InteractionTagType;
	
	UPROPERTY(BlueprintReadOnly, Category="CommonInteractionSystem|Shared")
	FGameplayTagContainer SourceTags;
};
