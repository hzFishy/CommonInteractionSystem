// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "GameplayTagContainer.h"
#include "CISInteractionFragmentBase.generated.h"


/**
 *	Base struct for any interaction fragment 
 */
USTRUCT(meta = (Hidden), BlueprintType)
struct COMMONINTERACTIONSYSTEM_API FCISInteractionFragmentBase
{
	GENERATED_BODY();

	FCISInteractionFragmentBase() {};
	FCISInteractionFragmentBase(const FGameplayTag& InTypeTag)
	{
		InteractionTypeTag = InTypeTag;
	};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem", meta=(Categories="CIS.Interaction.Types"))
	FGameplayTag InteractionTypeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem")
	FGameplayTagContainer InteractionTags;
};
