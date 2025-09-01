// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Interaction/Data/Fragments/CISInteractionFragmentBase.h"
#include "Shared/Data/CISCoreTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "CISInteractionComponent.generated.h"
class UCISFocusComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCISOnSingleInteractionSignature,
	UCISInteractionComponent*, InteractionComponent,
	APawn*, SourcePawn,
	const FGameplayTagContainer&, SourceInteractionTags,
	bool, bInteractionSucceeded
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FCISOnHoldInteractionStartedSignature,
	UCISInteractionComponent*, InteractionComponent,
	APawn*, SourcePawn,
	const FGameplayTagContainer&, SourceInteractionTags,
	bool, bInteractionSucceeded,
	float, TimeToHold
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCISOnHoldInteractionEndedSignature,
	UCISInteractionComponent*, InteractionComponent,
	APawn*, SourcePawn,
	const FGameplayTagContainer&, SourceInteractionTags,
	bool, bHoldSucceeded
);


/**
 *	Component placed on usable actors, the owning actor needs to have at least one focus scene component.
 *	Only use one Interaction Component per actor
 */
UCLASS(ClassGroup=(CommonInteractionSystem), Blueprintable, DisplayName="Interaction Component", meta=(BlueprintSpawnableComponent))
class COMMONINTERACTIONSYSTEM_API UCISInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Properties
	----------------------------------------------------------------------------*/
public:
	UPROPERTY(BlueprintAssignable, DisplayName="On Interactable State Changed", Category="CommonInteractionSystem")
	FCISOnInteractableStateChangedSignature OnInteractableStateChangedDelegate;

	UPROPERTY(BlueprintAssignable, DisplayName="On Single Interaction", Category="CommonInteractionSystem")
	FCISOnSingleInteractionSignature OnSingleInteractionDelegate;

	UPROPERTY(BlueprintAssignable, DisplayName="On Hold Interaction Started", Category="CommonInteractionSystem")
	FCISOnHoldInteractionStartedSignature OnHoldInteractionStartedDelegate;

	UPROPERTY(BlueprintAssignable, DisplayName="On Hold Interaction Ended", Category="CommonInteractionSystem")
	FCISOnHoldInteractionEndedSignature OnHoldInteractionEndedDelegate;

protected:
	/** If true the owning actor is usable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup")
	bool bInteractable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup")
	TInstancedStruct<FCISInteractionFragmentBase> InteractionFragment;
	
	/** If not set, we use the default interaction distance set in player interaction component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup")
	TOptional<float> MaxInteractionRangeOverride;
	

	/** Cached base interaction fragment */
	const FCISInteractionFragmentBase* CachedBaseInteractionFragment;
	/** All focus component we found after gather on init */
	TArray<TWeakObjectPtr<UCISFocusComponent>> FoundFocusComponents;


	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCISInteractionComponent();
	
	virtual void InitializeComponent() override;


	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
public:
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Interaction")
	FGameplayTag GetInteractionTypeTag() const;
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Interaction")
	void AppendInteractionTags(FGameplayTagContainer& OutInteractionTags) const;
	
	template <std::derived_from<FCISInteractionFragmentBase> T>
	const T* GetFragment() const
	{
		if (InteractionFragment.IsValid())
		{
			if (const T* Frag = InteractionFragment.GetPtr<T>())
			{
				return Frag;
			}
		}
		return nullptr;
	}
	
	template <std::derived_from<FCISInteractionFragmentBase> T>
	T* GetMutableFragment() const
	{
		if (InteractionFragment.IsValid())
		{
			if (T* Frag = InteractionFragment.GetMutablePtr<T>())
			{
				return Frag;
			}
		}
		return nullptr;
	}
	
	void GetFoundFocusComponents(TArray<UCISFocusComponent*>& OutFocusComponents) const;
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Interaction")
	bool IsSingleType() const;
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Interaction")
	bool IsHoldType() const;
	
	/** Set new interactable state */
	virtual void SetInteractable(bool bNewValue);
	
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Interaction")
	virtual bool TryInteract(APawn* SourcePawn, const FGameplayTag& SourceInteractionTagType,
	                         FGameplayTagContainer SourceInteractionTags);

	virtual void OnInteraction(APawn* SourcePawn, const FGameplayTag& SourceInteractionTagType,
		const FGameplayTagContainer& SourceInteractionTags, bool bCanInteractResult);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Interaction")
	void K2_OnInteraction(APawn* SourcePawn, const FGameplayTag& SourceInteractionTagType,
		const FGameplayTagContainer& SourceInteractionTags, bool bCanInteractResult);
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Interaction")
	virtual bool CanInteractWith(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags) const;

	void TriggerOnHoldInteractionStarted(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags,
		bool bInteractionSucceeded, float TimeToHold);
	
	void TriggerOnHoldInteractionEnded(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags,
		bool bHoldSucceeded);
	
	/** Get closest focus component */
	UCISFocusComponent* GetBestFocusComponent(APawn* SourcePawn) const;
};
