// By hzFishy - 2025 - Do whatever you want with it.


#include "CommonInteractionSystem/Public/Interaction/Components/CISInteractionComponent.h"

#include "Asserts/FUAsserts.h"
#include "Focus/Components/CISFocusComponent.h"
#include "Interaction/Data/Fragments/Derived/CISUseFragmentSingle.h"
#include "Interaction/Data/Fragments/Derived/FBPGUseFragmentHold.h"
#include "Logging/FULogging.h"
#include "Shared/Interfaces/CISInteractionCustomization.h"

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCISInteractionComponent::UCISInteractionComponent():
	bInteractable(true),
	InteractionFragment(TInstancedStruct<FCISInteractionFragmentBase>::Make<FCISInteractionFragmentSingle>()),
	CachedBaseInteractionFragment(nullptr)
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

	CachedBaseInteractionFragment = GetFragment<FCISInteractionFragmentBase>();
	
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
FGameplayTag UCISInteractionComponent::GetInteractionTypeTag() const
{
	if (FU_ENSURE_MSG(CachedBaseInteractionFragment, "CachedBaseInteractionFragment is invalid"))
	{
		return CachedBaseInteractionFragment->InteractionTypeTag;
	}
	return FGameplayTag();
}

void UCISInteractionComponent::AppendInteractionTags(FGameplayTagContainer& OutInteractionTags) const
{
	if (FU_ENSURE_MSG(CachedBaseInteractionFragment, "CachedBaseInteractionFragment is invalid"))
	{
		OutInteractionTags.AppendTags(CachedBaseInteractionFragment->InteractionTags);
	}
}

void UCISInteractionComponent::GetFoundFocusComponents(TArray<UCISFocusComponent*>& OutFocusComponents) const
{
	OutFocusComponents.Reserve(FoundFocusComponents.Num());
	Algo::Transform(FoundFocusComponents, OutFocusComponents, [](const TWeakObjectPtr<UCISFocusComponent>& InWeak)
	{
		return InWeak.Get();
	});
}

bool UCISInteractionComponent::IsSingleType() const
{
	if (FU_ENSURE_MSG(CachedBaseInteractionFragment, "CachedBaseInteractionFragment is invalid"))
	{
		return CachedBaseInteractionFragment->InteractionTypeTag.MatchesTag(TAG_CIS_Interaction_Types_Single);
	}
	return false;
}

bool UCISInteractionComponent::IsHoldType() const
{
	if (FU_ENSURE_MSG(CachedBaseInteractionFragment, "CachedBaseInteractionFragment is invalid"))
	{
		return CachedBaseInteractionFragment->InteractionTypeTag.MatchesTag(TAG_CIS_Interaction_Types_Hold);
	}
	return false;
}

void UCISInteractionComponent::SetInteractable(bool bNewValue)
{
	if (bInteractable == bNewValue) { return; }
	
	bInteractable = bNewValue;

	if (!bInteractable && IsHoldType())
	{
		// TODO: cancel running hold interaction
	}

	OnInteractableStateChangedDelegate.Broadcast(bInteractable);
}

bool UCISInteractionComponent::TryInteract(APawn* SourcePawn, const FGameplayTag& SourceInteractionTagType, FGameplayTagContainer SourceInteractionTags)
{
	bool bCanInteract = CanInteractWith(SourcePawn, SourceInteractionTags);

	AppendInteractionTags(SourceInteractionTags);
	
	if (IsSingleType())
	{
		auto* SingleFragment = GetFragment<FCISInteractionFragmentSingle>();
		if (FU_ENSURE_MSG(SingleFragment, "SingleFragment wasn't found"))
		{
			OnInteraction(
				SourcePawn, 
				SourceInteractionTagType,
				SourceInteractionTags, 
				bCanInteract
			);
			
			OnSingleInteractionDelegate.Broadcast(
				this,
				SourcePawn,
				SourceInteractionTags,
				bCanInteract
			);
		}
	}
	else if (IsHoldType())
	{
		auto* HoldFragment = GetFragment<FCISInteractionFragmentHold>();
		if (FU_ENSURE_MSG(HoldFragment, "HoldFragment wasn't found"))
		{
			OnInteraction(
				SourcePawn,
				SourceInteractionTagType,
				SourceInteractionTags,
				bCanInteract
			);

			TriggerOnHoldInteractionEnded(
				SourcePawn,
				SourceInteractionTags,
				bCanInteract
			);
		}
	}
	
	return bCanInteract;
}

void UCISInteractionComponent::OnInteraction(APawn* SourcePawn, const FGameplayTag& SourceInteractionTagType,
	const FGameplayTagContainer& SourceInteractionTags, bool bCanInteractResult)
{
	K2_OnInteraction(SourcePawn, SourceInteractionTagType, SourceInteractionTags, bCanInteractResult);
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

void UCISInteractionComponent::TriggerOnHoldInteractionStarted(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags,
	bool bInteractionSucceeded, float TimeToHold)
{
	OnHoldInteractionStartedDelegate.Broadcast(
		this,
		SourcePawn,
		SourceInteractionTags,
		bInteractionSucceeded,
		TimeToHold
	);
}

void UCISInteractionComponent::TriggerOnHoldInteractionEnded(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags,
	bool bHoldSucceeded)
{
	OnHoldInteractionEndedDelegate.Broadcast(
		this,
		SourcePawn,
		SourceInteractionTags,
		bHoldSucceeded
	);
}

UCISFocusComponent* UCISInteractionComponent::GetBestFocusComponent(APawn* SourcePawn) const
{
	if (FoundFocusComponents.IsEmpty()) { return nullptr; }
	
	UCISFocusComponent* BestComponent = nullptr;
	float BestDistance = FLT_MAX;

	for (int i = 0; i < FoundFocusComponents.Num(); ++i)
	{
		auto* Component = FoundFocusComponents[i].Get();
		float Distance = FVector::Distance(Component->GetComponentLocation(), SourcePawn->GetActorLocation());
		if (Distance < BestDistance)
		{
			BestComponent = Component;
			BestDistance = Distance;
		}
	}

	return BestComponent;
}
