// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Shared/Data/CISCoreTypes.h"
#include "CISInteractionComponent.generated.h"
class UCISFocusComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCISOnInteractionStartSignature, APawn*, SourcePawn, const FGameplayTagContainer&, SourceInteractionTags);


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

	UPROPERTY(BlueprintAssignable, DisplayName="On Interaction Start", Category="CommonInteractionSystem")
	FCISOnInteractionStartSignature OnInteractionStartDelegate;

protected:
	/** If true the owning actor is usable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup")
	bool bInteractable;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup")
	FGameplayTagContainer InteractionTags;

	/** If not set, we use the default interaction distance set in player interaction component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup")
	TOptional<float> MaxInteractionRangeOverride;
	
	/** All focus component we found after gather on init */
	UPROPERTY()
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
	/** Usually called from a succesful trace on a interaction ability */
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem")
	bool TryInteract(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags);
	
	/** Set new interactable state */
	void SetInteractable(bool bNewValue);

	void GetInteractionTags(FGameplayTagContainer& OutInteractionTags) const;
	
	void GetFoundFocusComponents(TArray<UCISFocusComponent>& OutFocusComponents) const;
	
	bool CanInteractWith(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags) const;
};
