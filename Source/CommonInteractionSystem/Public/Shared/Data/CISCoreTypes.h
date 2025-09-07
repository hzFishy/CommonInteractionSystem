// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "NativeGameplayTags.h"
#include "CISCoreTypes.generated.h"


class UCISFocusComponent;
	/*----------------------------------------------------------------------------
		Logging
	----------------------------------------------------------------------------*/
COMMONINTERACTIONSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(LogCISValidation, Log, Log);

#define CIS_LOG_Validation_E(FORMAT, ...) FU_LOG_Validation_E(LogCISValidation, FORMAT, ##__VA_ARGS__);


	/*----------------------------------------------------------------------------
		Delegate Signatures
	----------------------------------------------------------------------------*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCISOnInteractableStateChangedSignature, bool, bnewState);


	/*----------------------------------------------------------------------------
		Gameplay Tags
	----------------------------------------------------------------------------*/
COMMONINTERACTIONSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CIS_Interaction_Events_AbilityActivate);
COMMONINTERACTIONSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CIS_Interaction_Types_Single);
COMMONINTERACTIONSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CIS_Interaction_Types_Hold);
COMMONINTERACTIONSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CIS_Shared_Icons_DefaultInteraction);


	/*----------------------------------------------------------------------------
		Types
	----------------------------------------------------------------------------*/
USTRUCT(BlueprintType, DisplayName="Interaction Focus Params")
struct COMMONINTERACTIONSYSTEM_API FCISInteractionFocusParams
{
	GENERATED_BODY()

	FCISInteractionFocusParams();

	
	/////////////////////////////
	/// In
	
	/**
	 * True when we are only testing the CanFocus result.
	 * Use this to skip heaving code only meant for real focus logic
	 */
	bool bIsTest;
	const FHitResult* FocusHitResult;
	/** If no override this text will be used */
	FText DefaultText;
	/** Tags passed from Pawn Interaction Component */
	FGameplayTagContainer SourceInteractionTags;

	
	/////////////////////////////
	/// Out
	
	/** If not empty we will use this instead of DefaultText */
	FText OverrideText;

	/** Get the end focus text */
	const FText& GetFinalFocusText() const;
};

USTRUCT(BlueprintType, DisplayName="Interaction Params")
struct COMMONINTERACTIONSYSTEM_API FCISInteractionParams
{
	GENERATED_BODY()

	FCISInteractionParams();

	
	/////////////////////////////
	/// In
	
	const FHitResult* HitResult;
	FGameplayTag SourceInteractionTagType;
	/** Tags passed from Pawn Interaction Component */
	FGameplayTagContainer SourceInteractionTags;
	TWeakObjectPtr<UCISFocusComponent> ConsideredFocusComponent;
};
