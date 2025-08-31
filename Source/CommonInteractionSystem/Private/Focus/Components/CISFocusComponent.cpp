// By hzFishy - 2025 - Do whatever you want with it.


#include "Focus/Components/CISFocusComponent.h"

#include "Interaction/Components/CISInteractionComponent.h"
#include "Utility/FUUtilities.h"


/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCISFocusComponent::UCISFocusComponent():
	FocusText(FText::FromString("Focus Text"))
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UCISFocusComponent::InitializeComponent()
{
	Super::InitializeComponent();

#if WITH_EDITOR
	FU_UTILS_EDITOR_RETURN_NOTGAMEWORLD;
#endif

	FoundOwnerInteractionComponent = GetOwner()->FindComponentByClass<UCISInteractionComponent>();

	if (FoundOwnerInteractionComponent.IsValid())
	{
		FoundOwnerInteractionComponent->OnInteractionStartDelegate.AddUniqueDynamic(this, &ThisClass::OnUseComponentUse);
		FoundOwnerInteractionComponent->OnInteractableStateChangedDelegate.AddUniqueDynamic(this, &ThisClass::OnUseComponentUsableStateChangedCallback);
	}

	FocusWidgetClass = GetDefault<UBPGInteractionSystemDeveloperSettings>()->FocusWidgetClass.LoadSynchronous();
}

void UCISFocusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
