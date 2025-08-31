// By hzFishy - 2025 - Do whatever you want with it.


#include "CommonInteractionSystem/Public/Interaction/Components/CISInteractionComponent.h"
#include "Logging/FULogging.h"
#include "Shared/Interfaces/CISInteractionCustomization.h"


	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCISInteractionComponent::UCISInteractionComponent():
	bInteractable(true)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UCISInteractionComponent::InitializeComponent()
{
	Super::InitializeComponent();

#if WITH_EDITOR
	FU_UTILS_EDITOR_RETURN_NOTGAMEWORLD;
#endif
	
	GetOwner()->ForEachComponent<UCISFocusComponent>(false, [this](UCISFocusComponent* Comp) mutable
	{
		FoundFocusComponents.Emplace(Comp);
	});
	
	// validity check
#if WITH_EDITOR
	const bool bHasFocusComp = !FoundFocusComponents.IsEmpty();
	if (!bHasFocusComp)
	{
		CIS_LOG_Validation_E(
			"Actor {0} owns a Interaction Component but doesn't have a Focus Component, must fix this",
			FU_LOG_THIS
		);
	}
#endif
}


	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
bool UCISInteractionComponent::TryInteract(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags)
{
	bool bCanInteract = CanInteractWith(SourcePawn, SourceInteractionTags);
	
	if (bCanInteract)
	{
		OnInteractionStartDelegate.Broadcast(SourcePawn, SourceInteractionTags);
	}
	
	return bCanInteract;
}

void UCISInteractionComponent::SetInteractable(bool bNewValue)
{
	if (bInteractable == bNewValue) { return; }
	
	bInteractable = bNewValue;

	OnInteractableStateChangedDelegate.Broadcast(bInteractable);
}

void UCISInteractionComponent::GetInteractionTags(FGameplayTagContainer& OutInteractionTags) const
{
	OutInteractionTags.AppendTags(InteractionTags);
}

void UCISInteractionComponent::GetFoundFocusComponents(TArray<UCISFocusComponent>& OutFocusComponents) const
{
	OutFocusComponents.Reserve(FoundFocusComponents.Num());
	Algo::Transform(FoundFocusComponents, OutFocusComponents, [](const TWeakObjectPtr<UCISFocusComponent>& InWeak)
	{
		return InWeak.Get();
	});
}

bool UCISInteractionComponent::CanInteractWith(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags) const
{
	if (!bInteractable) { return false; }

	// check if source pawn close enough
	// this overrides any other extra settings
	if (MaxInteractionRangeOverride.IsSet())
	{
		float Distance = FVector::Distance(SourcePawn->GetActorLocation(), GetOwner()->GetActorLocation());
		return Distance <= MaxInteractionRangeOverride.GetValue();
	}
	
	// get potential extra settings
	if (GetOwner()->Implements<UCISInteractionCustomization>())
	{
		return ICISInteractionCustomization::Execute_CanInteractWith(GetOwner(), SourcePawn, SourceInteractionTags);
	}

	return true;
}
