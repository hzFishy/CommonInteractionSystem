// By hzFishy - 2025 - Do whatever you want with it.


#include "Focus/Components/CISFocusComponent.h"
#include "Asserts/FUAsserts.h"
#include "Focus/Widgets/CISFocusWidget.h"
#include "Interaction/Components/CISInteractionComponent.h"
#include "Shared/Data/CISCoreDeveloperSettings.h"
#include "Shared/Interfaces/CISInteractionCustomization.h"
#include "Subsystems/FWSWorldSpaceLocalSubsystem.h"
#include "Utility/FUUtilities.h"


	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCISFocusComponent::UCISFocusComponent():
	FocusText(FText::FromString("Focus Text")),
	IconFocusTag(GetDefault<UCISCoreDeveloperSettings>()->DefaultFocusIconTag),
	bFocusableByDefault(true),
	FocusWidgetClass(GetDefault<UCISCoreDeveloperSettings>()->DefaultFocusWidgetClass),
	bAsyncLoadFocusWidgetClass(true),
	bFocusable(false),
	bIsFocused(false)
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

	bFocusable = bFocusableByDefault;

	FoundOwnerInteractionComponent = GetOwner()->FindComponentByClass<UCISInteractionComponent>();

	if (FoundOwnerInteractionComponent.IsValid())
	{
		FoundOwnerInteractionComponent->OnSingleInteractionDelegate.AddUniqueDynamic(this, &ThisClass::OnSingleInteraction);
		FoundOwnerInteractionComponent->OnHoldInteractionStartedDelegate.AddUniqueDynamic(this, &ThisClass::OnHoldInteractionStarted);
		FoundOwnerInteractionComponent->OnHoldInteractionEndedDelegate.AddUniqueDynamic(this, &ThisClass::OnHoldInteractionEnded);
		FoundOwnerInteractionComponent->OnInteractableStateChangedDelegate.AddUniqueDynamic(this, &ThisClass::OnInteractableStateChanged);
	}

	if (bAsyncLoadFocusWidgetClass)
	{
		FocusWidgetClass.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateWeakLambda(this,
			[this](const FSoftObjectPath& Path, UObject* Object) mutable
		{
				LoadedFocusWidgetClass = Cast<UClass>(Object);
		}));
	}
	else
	{
		LoadedFocusWidgetClass = FocusWidgetClass.LoadSynchronous();
	}
}


	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
void UCISFocusComponent::SetFocusText(const FText& NewFocusText)
{
	FocusText = NewFocusText;

	if (IsValid(FocusWidget.Get()))
	{ 
		RefreshFocusWidget(false);
	}
}

void UCISFocusComponent::SetIconFocusTag(const FGameplayTag& NewIconFocusTag)
{
	IconFocusTag = NewIconFocusTag;

	if (IsValid(FocusWidget.Get()))
	{
		RefreshFocusWidget(false);
	}
}

void UCISFocusComponent::StartFocus(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, const FHitResult& FocusHitResult)
{
	// skip if already focused
	if (!ensure(!bIsFocused)) { return; }
	
	FCISInteractionFocusParams FocusParams;
	FocusParams.FocusHitResult = &FocusHitResult;
	FocusParams.DefaultText = FocusText;
	FocusParams.SourceInteractionTags = SourceInteractionTags;
	if (!CanFocus(SourcePawn, FocusParams)) { return; }

	FocusingSourcePawn = SourcePawn;
	bIsFocused = true;
	LatestFocusParams = FocusParams;
	
	// on subsystem add a new focus widget on world space container
	{
		if (auto* PC = SourcePawn->GetController<APlayerController>())
		{
			if (auto* LC = PC->GetLocalPlayer())
			{
				if (auto* WorldSpaceWidgetsSubsystem = LC->GetSubsystem<UFWSWorldSpaceLocalSubsystem>())
				{
					FFWSWorldSpaceSubsystemParams SubsystemParams;
					// TODO: It could be more interesting to create the focus widget more early and keep it hidden until desired
					SubsystemParams.TargetUserWidget = CreateFocusWidget(FocusParams);
					SubsystemParams.TargetSceneComponent = this;
					WorldSpaceWidgetsSubsystem->AddWorldSpaceWidget(SubsystemParams);
				}
			}
		}
	}
}

void UCISFocusComponent::StopFocus()
{
	// if we are not focusing do nothing
	if (!ensure(bIsFocused)) { return; }
	
	bIsFocused = false;
	auto* OldSourcePawn = FocusingSourcePawn.Get();
	FocusingSourcePawn.Reset();
	
	// TODO: It could be more interesting to keep the widget hidden until desired
	FocusWidget->RemoveFromParent();
	FocusWidget = nullptr;
	
	OnFocusLostDelegate.Broadcast(OldSourcePawn);
}

bool UCISFocusComponent::CanFocus(APawn* SourcePawn, FCISInteractionFocusParams& InteractionFocusParams)
{
	// if we are not focusable deny
	if (!bFocusable || !FoundOwnerInteractionComponent.IsValid()) { return false; }
	
	// check if interaction component accepts the source
	FCISInteractionParams InteractionParams;
	InteractionParams.HitResult = InteractionFocusParams.FocusHitResult;
	InteractionParams.SourceInteractionTags = InteractionFocusParams.SourceInteractionTags;
	bool bResult = FoundOwnerInteractionComponent->CanInteractWith(FocusingSourcePawn.Get(), InteractionParams);
	
	if (bResult)
	{
		// get potential extra settings
		if (GetOwner()->Implements<UCISInteractionCustomization>())
		{
			bResult = ICISInteractionCustomization::Execute_CanFocusWith(GetOwner(), FocusingSourcePawn.Get(), InteractionFocusParams);
		}
	}
	
	return bResult;
}

UUserWidget* UCISFocusComponent::CreateFocusWidget(const FCISInteractionFocusParams& FocusParams)
{
	if (FU_ENSURE_VALID_MSG(LoadedFocusWidgetClass, "LoadedFocusWidgetClass is invalid"))
	{
		FocusWidget = CreateWidget<UCISFocusWidget>(GetWorld(), LoadedFocusWidgetClass);
		UpdateFocusWidgetContent(FocusParams);
		FocusWidget->AddToViewport();
	}

	return FocusWidget.Get();
}

void UCISFocusComponent::UpdateFocusWidgetContent(const FCISInteractionFocusParams& FocusParams)
{
	if (IsValid(FocusWidget.Get()))
	{
		FocusWidget->UpdateContent(FocusParams.GetFinalFocusText(), IconFocusTag);
	}
}

void UCISFocusComponent::RefreshFocusWidget(bool bCheckFocusStatus)
{
	if (IsValid(FocusWidget.Get()))
	{
		if (bCheckFocusStatus && FU_ENSURE_WEAKVALID_MSG(FocusingSourcePawn, "FocusingSourcePawn invalid"))
		{
			FCISInteractionFocusParams& NewFocusParams = LatestFocusParams;
			NewFocusParams.DefaultText = FocusText;
			if (CanFocus(FocusingSourcePawn.Get(), NewFocusParams))
			{
				UpdateFocusWidgetContent(LatestFocusParams);
			}
			else
			{
				StopFocus();
			}
		}
		else
		{
			UpdateFocusWidgetContent(LatestFocusParams);
		}
	}
}


	/*----------------------------------------------------------------------------
		Callbacks
	----------------------------------------------------------------------------*/
void UCISFocusComponent::OnInteractableStateChanged(bool bNewState)
{
	if (bFocusable == bNewState) { return; }
	
	// sync focus comp with interaction component
	bFocusable = bNewState;

	// if was focused, clear
	if (!bFocusable && bIsFocused)
	{
		FU_ENSURE_WEAKVALID_MSG(FocusingSourcePawn, "FocusingSourcePawn is null");
		StopFocus();
	}
}

void UCISFocusComponent::OnSingleInteraction(UCISInteractionComponent* InteractionComponent, APawn* SourcePawn,
		const FGameplayTagContainer& SourceInteractionTags, bool bInteractionSucceeded)
{
	if (IsValid(FocusWidget))
	{
		FocusWidget->OnSingleInteraction(SourcePawn, SourceInteractionTags, bInteractionSucceeded);
	}
}

void UCISFocusComponent::OnHoldInteractionStarted(UCISInteractionComponent* InteractionComponent, APawn* SourcePawn,
	const FGameplayTagContainer& SourceInteractionTags, bool bInteractionSucceeded, float TimeToHold)
{
	if (IsValid(FocusWidget))
	{
		FocusWidget->OnHoldInteractionStarted(SourcePawn, SourceInteractionTags, bInteractionSucceeded, TimeToHold);
	}
}

void UCISFocusComponent::OnHoldInteractionEnded(UCISInteractionComponent* InteractionComponent, APawn* SourcePawn,
	const FGameplayTagContainer& SourceInteractionTags, bool bHoldSucceeded)
{
	if (IsValid(FocusWidget))
	{
		FocusWidget->OnHoldInteractionEnded(SourcePawn, SourceInteractionTags, bHoldSucceeded);
	}
}
