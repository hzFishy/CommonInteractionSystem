// By hzFishy - 2025 - Do whatever you want with it.


#include "Shared/Component/CISSourcePawnInteractionComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CollisionDebugDrawingPublic.h"
#include "Asserts/FUAsserts.h"
#include "Focus/Components/CISFocusComponent.h"
#include "Interaction/Abilities/CISInteractionGameplayAbility.h"
#include "Interaction/Components/CISInteractionComponent.h"
#include "Interaction/Data/Fragments/Derived/CISUseFragmentHold.h"
#include "Interaction/Data/TargetData/CISInteractionTargetData.h"
#include "Shared/Data/CISCoreTypes.h"
#include "Utility/FUUtilities.h"
#include "CommonInteractionSystem.h"
#include "Draw/FUDraw.h"
#include "Logging/FULogging.h"
#include "Utility/FUOrientedBox.h"


namespace CIS::Debug
{
#if CIS_WITH_DEBUG
	FU_CMD_BOOL_WITH_OPT_FLOAT_CPPONLY(DrawInteractionTrace,
		"CIS.DrawInteractionTrace", "Draw Interaction traces",
		bDrawInteractionTrace, DrawInteractionTraceDuration, 2
	);
	FU_CMD_BOOL_WITH_OPT_FLOAT_CPPONLY(DrawFocusTrace,
		"CIS.DrawFocusTrace", "Draw Focus traces",
		bDrawFocusTrace, DrawFocusTraceDuration, 2
	);
	FU_CMD_BOOL_TOGGLE(DrawFocusFrameState,
		"CIS.DrawFocusFrameState", "Draw Focus frame state",
		bDrawFocusFrameState, false
	);
#endif
}


FCISFocusFrameData::FCISFocusFrameData():
	bFocusResult(false)
{}

void FCISFocusFrameData::Reset()
{
	FocusedActor.Reset();
	FocusedBestFocusComponent.Reset();
	bFocusResult = false;
}

FCISInteractionRunningProcess::FCISInteractionRunningProcess():
	bRunning(false),
	bHoldInteractionFinished(false),
	bHoldInteractionSuccessful(false)
{}

bool FCISInteractionRunningProcess::IsSingle() const
{
	return InteractionTypeTag.MatchesTag(TAG_CIS_Interaction_Types_Single);
}

bool FCISInteractionRunningProcess::IsHold() const
{
	return InteractionTypeTag.MatchesTag(TAG_CIS_Interaction_Types_Hold);
}

void FCISInteractionRunningProcess::Reset()
{
	bRunning = false;
	InteractionTypeTag = FGameplayTag();
	TimerHandle.Invalidate();
	InteractionComponent.Reset();
	SelectedFocusComponent.Reset();

	HoldInteractionDelegateHandle.Reset();
	bHoldInteractionFinished = false;
	bHoldInteractionSuccessful = false;
}


	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCISSourcePawnInteractionComponent::UCISSourcePawnInteractionComponent():
	InteractionLineTraceLength(300),
	InteractionSphereRadius(20),
	TraceChannel(ECC_Visibility),
	bCanInteract(true),
	InteractionAbilityEventTag(TAG_CIS_Interaction_Events_AbilityActivate),
	bAsyncLoadInteractionAbilityClass(true),
	bCanFocus(true),
	bTryFocusOnTick(true)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UCISSourcePawnInteractionComponent::InitializeComponent()
{
	Super::InitializeComponent();

#if WITH_EDITOR
	FU_UTILS_EDITOR_RETURN_NOTGAMEWORLD
#endif

	OwnerSourcePawn = GetOwner<APawn>();
	if (FU_ENSURE_WEAKVALID_MSG(OwnerSourcePawn, "OwnerSourcePawn is invalid"))
	{
		if (auto* ASCI = Cast<IAbilitySystemInterface>(OwnerSourcePawn.Get()))
		{
			OwnerAbilitySystemComponent = ASCI->GetAbilitySystemComponent();
		}
	}

	if (FU_ENSURE_WEAKNOTNULL_MSG(InteractionAbilityClass, "InteractionAbilityClass is not set"))
	{
		auto OnInteractionAbilityLoaded = [this](UClass* LoadedAbility) mutable
		{
			LoadedInteractionAbilityClass = LoadedAbility;
			
			if (FU_ENSURE_WEAKVALID_MSG(OwnerAbilitySystemComponent, "OwnerAbilitySystemComponent is invalid"))
			{
				InteractionAbilitySpecHandle = OwnerAbilitySystemComponent->K2_GiveAbility(LoadedInteractionAbilityClass, 0, -1);

				if (InteractionAbilitySpecHandle.IsValid())
				{
					OwnerAbilitySystemComponent->OnAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnded);
				}
			}
		};
		
		if (bAsyncLoadInteractionAbilityClass)
		{
			InteractionAbilityClass.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateWeakLambda(this,
			[this, OnInteractionAbilityLoaded](const FSoftObjectPath& Path, UObject* Object) mutable
			{
					OnInteractionAbilityLoaded(Cast<UClass>(Object));
			}));
		}
		else
		{
			OnInteractionAbilityLoaded(InteractionAbilityClass.LoadSynchronous());
		}
	}
}

void UCISSourcePawnInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCanFocus && bTryFocusOnTick)
	{
		TryFocus();

#if CIS_WITH_DEBUG
		if (CIS::Debug::bDrawFocusFrameState)
		{
			// draw focused actor and focus scene component
			if (PreviousTryFocusData.FocusedActor.IsValid())
			{
				auto PreviousFocusedActorOrientedBox = FU::Utils::FFUOrientedBox(PreviousTryFocusData.FocusedActor.Get(), false);
				PreviousFocusedActorOrientedBox.DrawDebugFrame(GetWorld(), FColor::Yellow);
				
				if (PreviousTryFocusData.FocusedBestFocusComponent.IsValid())
				{
					FU::Draw::DrawDebugBoxFrame(GetWorld(),
						PreviousTryFocusData.FocusedBestFocusComponent->GetComponentLocation(), 10,
						PreviousTryFocusData.FocusedActor->GetActorRotation().Quaternion(),
						PreviousTryFocusData.bFocusResult ? FColor::Green : FColor::Red, 0
					);
				}
			}
		}
#endif
	}
}


	/*----------------------------------------------------------------------------
		Shared
	----------------------------------------------------------------------------*/
void UCISSourcePawnInteractionComponent::SetInteractionStartPoint(USceneComponent* SceneComponent)
{
	InteractionStartPoint = SceneComponent;
}

void UCISSourcePawnInteractionComponent::SetCanInteract(bool bNewValue)
{
	if (bCanInteract == bNewValue) { return; }
	
	bCanInteract = bNewValue;

	if (!bCanInteract)
	{
		// if was a hold and still running, cancel it
		if (IsHoldRunning())
		{
			CIS_LOG_D("CanInteract was disabled while a hold interaction is being performed, canceling");
			CancelHoldInteraction();
		}
	}
}

void UCISSourcePawnInteractionComponent::SetCanFocus(bool bNewValue)
{
	if (bCanFocus == bNewValue) { return; }
	
	bCanFocus = bNewValue;

	if (!bCanFocus)
	{
		// if was focusing stop
		if (PreviousTryFocusData.FocusedBestFocusComponent.IsValid()
			&& PreviousTryFocusData.FocusedBestFocusComponent->IsFocusedBy(OwnerSourcePawn.Get()))
		{
			PreviousTryFocusData.FocusedBestFocusComponent->StopFocus();
		}
	}
}

void UCISSourcePawnInteractionComponent::DoSharedInteractionTrace(FHitResult& OutResult, bool bFocusTrace)
{
	FCollisionShape Shape = FCollisionShape::MakeSphere(InteractionSphereRadius);
	
	const FVector& StartLocation = InteractionStartPoint->GetComponentLocation();
	FVector EndLocation = InteractionStartPoint->GetComponentLocation() + (InteractionStartPoint->GetForwardVector() * InteractionLineTraceLength);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerSourcePawn.Get());
	
	GetWorld()->SweepSingleByChannel(
		OutResult,
		StartLocation,
		EndLocation,
		InteractionStartPoint->GetForwardVector().Rotation().Quaternion(),
		TraceChannel,
		Shape,
		QueryParams
	);

#if CIS_WITH_DEBUG
	if ((!bFocusTrace && CIS::Debug::bDrawInteractionTrace) || (bFocusTrace && CIS::Debug::bDrawFocusTrace))
	{
		DrawSphereSweeps(
			GetWorld(),
			StartLocation,
			EndLocation,
			Shape.GetCapsuleRadius(),
			{OutResult},
			bFocusTrace ? 0 : CIS::Debug::DrawInteractionTraceDuration
		);
	}
#endif
}

	
	/*----------------------------------------------------------------------------
		Interaction
	----------------------------------------------------------------------------*/
void UCISSourcePawnInteractionComponent::OnInputSingleInteraction(FGameplayTagContainer SourceInteractionTags)
{
	// only pass if aimed object accept single interactions
	auto* InteractionComponent = GetInteractionComponentFromInteractionTrace();
	if (IsValid(InteractionComponent))
	{
		if (InteractionComponent->IsSingleType())
		{
			OnInputSingleOrHandleInteractionStart(
				InteractionComponent,
				TAG_CIS_Interaction_Types_Single,
				SourceInteractionTags
			);
		}
	}
}

void UCISSourcePawnInteractionComponent::OnInputStartHoldInteraction(FGameplayTagContainer SourceInteractionTags)
{
	// only pass if aimed object accept hold interactions
	auto* InteractionComponent = GetInteractionComponentFromInteractionTrace();
	if (IsValid(InteractionComponent))
	{
		if (InteractionComponent->IsHoldType())
		{
			OnInputSingleOrHandleInteractionStart(
				InteractionComponent,
				TAG_CIS_Interaction_Types_Hold,
				SourceInteractionTags
			);
		}
	}
}

void UCISSourcePawnInteractionComponent::OnInputHoldInteractionEnd()
{
	// cancel hold if running
	if (IsHoldRunning())
	{
		CIS_LOG_D("Use stopped the hold interaction input while hold interaction was running, canceling it")
		CancelHoldInteraction();
	}
}

UCISInteractionComponent* UCISSourcePawnInteractionComponent::GetInteractionComponentFromInteractionTrace()
{
	if (InteractionStartPoint.IsValid())
	{
		AActor* HitActor = nullptr;
		FHitResult Result;
		DoSharedInteractionTrace(Result, false);
		if (IsValid(Result.GetActor()))
		{
			HitActor = Result.GetActor();
		}
		
		if (IsValid(HitActor))
		{
			return HitActor->FindComponentByClass<UCISInteractionComponent>();
		}
	}
	
	return nullptr;
}

bool UCISSourcePawnInteractionComponent::TryInteraction(const FGameplayTagContainer& SourceInteractionTags)
{
	if (InteractionStartPoint.IsValid())
	{
		AActor* HitActor = nullptr;
		FHitResult Result;
		DoSharedInteractionTrace(Result, false);
		if (IsValid(Result.GetActor()))
		{
			HitActor = Result.GetActor();
		}
		
		if (IsValid(HitActor))
		{
			auto* InteractionComponent = HitActor->FindComponentByClass<UCISInteractionComponent>();

			if (IsValid(InteractionComponent))
			{
				if (InteractionComponent->IsSingleType())
				{
					CIS_LOG_D("Trying to single interact with {0}", *FU::Utils::GetObjectDetailedName(InteractionComponent));
					return InteractionComponent->TryInteract(
						OwnerSourcePawn.Get(),
						TAG_CIS_Interaction_Types_Single,
						SourceInteractionTags
					);
				}
				else if (InteractionComponent->IsHoldType())
				{
					// detect if hold finished (if we started it previously)
					if (IsHoldRunning() && SharedInteractionRunningProcess.bHoldInteractionFinished)
					{
						CIS_LOG_D("Trying to interact with {0} from completed hold interaction",
							*FU::Utils::GetObjectDetailedName(InteractionComponent)
						);
						// interaction could still fail here if something changed in game while the hold was done
						return InteractionComponent->TryInteract(
							OwnerSourcePawn.Get(),
							TAG_CIS_Interaction_Types_Hold,
							SourceInteractionTags
						);
					}
					// dont use if already using
					else if (IsHoldRunning()) { return false; }

					CIS_LOG_D("Starting hold interaction process with {0}", *FU::Utils::GetObjectDetailedName(InteractionComponent));
					// otherwise start a hold operation
					return HoldInteractionStart(InteractionComponent, SourceInteractionTags);
				}
			}
		}
	}
	
	CIS_LOG_D("Try Interaction failed");
	return false;
}

void UCISSourcePawnInteractionComponent::OnInputSingleOrHandleInteractionStart(UCISInteractionComponent* InteractionComponent,
	const FGameplayTag& InteractionTypeTag, const FGameplayTagContainer& SourceInteractionTags)
{
	if (FU_ENSURE_VALID(InteractionComponent))
	{
		// trigger ability, the interaction code should be called inside
		if (OwnerAbilitySystemComponent.IsValid() && InteractionAbilitySpecHandle.IsValid())
		{
			FGameplayEventData EventData;
			EventData.EventTag = InteractionAbilityEventTag;
			EventData.Instigator = OwnerSourcePawn.Get();
			EventData.Target = OwnerSourcePawn.Get();
			
			auto* InteractionData = NewObject<UCISInteractionAbilityEventData>();
			InteractionData->SourceTags = SourceInteractionTags;
			InteractionData->InteractionTagType = InteractionTypeTag;
			EventData.OptionalObject = InteractionData;

			CIS_LOG_D("Gameplay Event with tag {0} sent to owner pawn {1}",
				InteractionAbilityEventTag, *FU::Utils::GetObjectDetailedName(OwnerSourcePawn.Get())
			);
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				OwnerSourcePawn.Get(),
				InteractionAbilityEventTag,
				EventData
			);
		}
	}
}

bool UCISSourcePawnInteractionComponent::IsHoldRunning() const
{
	return SharedInteractionRunningProcess.bRunning && SharedInteractionRunningProcess.IsHold();
}

bool UCISSourcePawnInteractionComponent::HoldInteractionStart(UCISInteractionComponent* InteractionComponent, const FGameplayTagContainer& SourceInteractionTags)
{
	bool bCanInteractResult = InteractionComponent->CanInteractWith(OwnerSourcePawn.Get(), SourceInteractionTags);
	const auto* HoldFragment = InteractionComponent->GetFragment<FCISInteractionFragmentHold>();
	
	if (bCanInteractResult && ensureAlways(HoldFragment))
	{
		SharedInteractionRunningProcess.Reset();
		SharedInteractionRunningProcess.bRunning = true;
		SharedInteractionRunningProcess.InteractionTypeTag = TAG_CIS_Interaction_Types_Hold;
		SharedInteractionRunningProcess.InteractionComponent = InteractionComponent;
		SharedInteractionRunningProcess.SelectedFocusComponent = InteractionComponent->GetBestFocusComponent(OwnerSourcePawn.Get());

		// handle blocking movement/look input
		{
			auto* OwnerPlayerController = OwnerSourcePawn->GetController();
			if (IsValid(OwnerPlayerController))
			{
				if (HoldFragment->bDisableMovementInputWhileHold)
				{
					OwnerPlayerController->SetIgnoreMoveInput(true);
				}
				if (HoldFragment->bDisableLookInputWhileHold)
				{
					OwnerPlayerController->SetIgnoreLookInput(true);
				}
			}
		}
		
		// start timer
		auto TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::OnHoldInteractionFinished, true);
		GetWorld()->GetTimerManager().SetTimer(
			SharedInteractionRunningProcess.TimerHandle,
			TimerDelegate,
			HoldFragment->HoldTime,
			false
		);

		// set up callbacks
		{
			SharedInteractionRunningProcess.HoldInteractionDelegateHandle = SharedInteractionRunningProcess.SelectedFocusComponent->OnFocusLostDelegate.AddUObject(
				this,
				&ThisClass::OnHoldObjectFocusLostCallback
			);

			InteractionComponent->OnInteractableStateChangedDelegate.AddUniqueDynamic(
				this, &ThisClass::OnCurrentHoldInteractableStateChanged);
		}
		InteractionComponent->TriggerOnHoldInteractionStarted(
			OwnerSourcePawn.Get(),
			SourceInteractionTags,
			true,
			HoldFragment->HoldTime
		);

		CIS_LOG_D("Hold Interaction Started")

		return true;
	}
	else
	{
		InteractionComponent->TriggerOnHoldInteractionStarted(
			OwnerSourcePawn.Get(),
			SourceInteractionTags,
			false,
			HoldFragment->HoldTime
		);

		CIS_LOG_D("Hold Interaction couldn't start because conditions failed")
		
		return false;
	}
}

void UCISSourcePawnInteractionComponent::OnHoldObjectFocusLostCallback(APawn* SourcePawn)
{
	// cancel hold if running
	if (IsHoldRunning())
	{
		CIS_LOG_D("User was performing hold interaction but object lost focus, canceling hold")
		CancelHoldInteraction();
	}
}

void UCISSourcePawnInteractionComponent::OnCurrentHoldInteractableStateChanged(bool bNewState)
{
	// cancel hold if running
	if (!bNewState && IsHoldRunning())
	{
		CIS_LOG_D("Target interaction component became unusable while user was performing interaction, canceling hold")
		CancelHoldInteraction();
	}
}

void UCISSourcePawnInteractionComponent::OnHoldInteractionFinished(bool bSuccess)
{
	CIS_LOG_D("Hold interaction finished (Success: {0})", bSuccess)
	
	const auto* HoldFragment = SharedInteractionRunningProcess.InteractionComponent->GetFragment<FCISInteractionFragmentHold>();
	ensureAlways(HoldFragment);

	// unsubscribe to delegates
	{
		SharedInteractionRunningProcess.SelectedFocusComponent->OnFocusLostDelegate.Remove(SharedInteractionRunningProcess.HoldInteractionDelegateHandle);
		
		SharedInteractionRunningProcess.InteractionComponent->OnInteractableStateChangedDelegate.RemoveDynamic(
			this, &ThisClass::OnCurrentHoldInteractableStateChanged);
	}
	
	SharedInteractionRunningProcess.bHoldInteractionFinished = true;
	SharedInteractionRunningProcess.bHoldInteractionSuccessful = bSuccess;

	// handle unblocking movement/look input
	{
		auto* OwnerPlayerController = OwnerSourcePawn->GetController();
		if (IsValid(OwnerPlayerController))
		{
			if (HoldFragment->bDisableMovementInputWhileHold)
			{
				OwnerPlayerController->SetIgnoreMoveInput(false);
			}
			if (HoldFragment->bDisableLookInputWhileHold)
			{
				OwnerPlayerController->SetIgnoreLookInput(false);
			}
		}
	}
	
	if (bSuccess)
	{
		// this will also call TriggerOnHoldInteractionEnded with true or false (depends if the interaction succeeds)
		TryInteraction(SharedInteractionRunningProcess.SourceInteractionTags);
	}
	else
	{
		SharedInteractionRunningProcess.InteractionComponent->TriggerOnHoldInteractionEnded(
			OwnerSourcePawn.Get(),
			SharedInteractionRunningProcess.SourceInteractionTags,
			false
		);
	}

	CleanHoldInteraction();
}

void UCISSourcePawnInteractionComponent::OnAbilityEnded(const FAbilityEndedData& EndedData)
{
	// see if its a interaction ability
	if (!InteractionAbilitySpecHandle.IsValid() || EndedData.AbilitySpecHandle != InteractionAbilitySpecHandle) { return; }

	if (IsHoldRunning())
	{
		CIS_LOG_D("Interaction Ability ended while user was performing a hold interaction, canceling the hold")
		CancelHoldInteraction();
	}
}

void UCISSourcePawnInteractionComponent::CancelHoldInteraction()
{
	CIS_LOG_D("Canceling current hold interaction")
	
	check(SharedInteractionRunningProcess.bRunning && SharedInteractionRunningProcess.IsHold());

	if (!SharedInteractionRunningProcess.bHoldInteractionSuccessful)
	{
		OnHoldInteractionFinished(false);
	}
}

void UCISSourcePawnInteractionComponent::CleanHoldInteraction()
{
	GetWorld()->GetTimerManager().ClearTimer(SharedInteractionRunningProcess.TimerHandle);

	SharedInteractionRunningProcess.Reset();
	
	// End ability
	if (OwnerAbilitySystemComponent.IsValid() && InteractionAbilitySpecHandle.IsValid())
	{
		OwnerAbilitySystemComponent->CancelAbilityHandle(InteractionAbilitySpecHandle);
	}
}

	
	/*----------------------------------------------------------------------------
		Focus
	----------------------------------------------------------------------------*/
void UCISSourcePawnInteractionComponent::TryFocus()
{
	if (!IsValid(InteractionStartPoint.Get())) { return; }
	if (!bCanFocus) { return; }

	// reset current data because we are starting a new frame
	CurrentTryFocusData.Reset();
	
	DoSharedInteractionTrace(LastFocusSweepResult, true);
	CurrentTryFocusData.FocusedActor = LastFocusSweepResult.GetActor();
	
	if (CurrentTryFocusData.FocusedActor.IsValid())
	{
		/**
		 *	Process recap
		 *	- if its a different focus scene component or different actor -> new focus
		 *  - if its the same focus scene component BUT the focus result changed -> new focus
		 */

		// get the interaction and focus component for the current focused actor
		auto* CurrentInteractionComponent = CurrentTryFocusData.FocusedActor->FindComponentByClass<UCISInteractionComponent>();
		if (IsValid(CurrentInteractionComponent))
		{
			CurrentTryFocusData.FocusedBestFocusComponent = CurrentInteractionComponent->GetBestFocusComponent(OwnerSourcePawn.Get());
			if (CurrentTryFocusData.FocusedBestFocusComponent.IsValid())
			{
				FCISInteractionFocusParams Params;
				Params.bIsTest = true;
				CurrentTryFocusData.bFocusResult = CurrentTryFocusData.FocusedBestFocusComponent->CanFocus(OwnerSourcePawn.Get(), Params);
			}
		}

		// fastest check first
		if (PreviousTryFocusData.FocusedBestFocusComponent.IsValid()
			&& !CurrentTryFocusData.FocusedBestFocusComponent.IsValid()
			&& PreviousTryFocusData.FocusedBestFocusComponent->IsFocusedBy(OwnerSourcePawn.Get()))
		{
			PreviousTryFocusData.FocusedBestFocusComponent->StopFocus();
		}
		// see if different actor or focus component (since one actor can have multiple focus component) or different focus result
		// previous focused actor, previous focus component and CurrentBestFocusComponent can be null
		else if (PreviousTryFocusData.FocusedActor != CurrentTryFocusData.FocusedActor
			|| PreviousTryFocusData.FocusedBestFocusComponent != CurrentTryFocusData.FocusedBestFocusComponent
			|| PreviousTryFocusData.bFocusResult != CurrentTryFocusData.bFocusResult)
		{
			// if previous focused component is valid we need to stop focusing it
			if (PreviousTryFocusData.FocusedBestFocusComponent.IsValid())
			{
				// dont need to stop if it never started
				if (PreviousTryFocusData.bFocusResult
					&& PreviousTryFocusData.FocusedBestFocusComponent->IsFocusedBy(OwnerSourcePawn.Get()))
				{
					PreviousTryFocusData.FocusedBestFocusComponent->StopFocus();
				}
			}

			// now we start new focus
			if (CurrentTryFocusData.FocusedBestFocusComponent.IsValid() && CurrentTryFocusData.bFocusResult)
			{
				FGameplayTagContainer Tags;
				GetFocusInteractionTags(Tags);
				CurrentTryFocusData.FocusedBestFocusComponent->StartFocus(OwnerSourcePawn.Get(), Tags);
			}
		}
	}
	else if (PreviousTryFocusData.FocusedBestFocusComponent.IsValid()
		&& PreviousTryFocusData.FocusedBestFocusComponent->IsFocusedBy(OwnerSourcePawn.Get()))
	{
		PreviousTryFocusData.FocusedBestFocusComponent->StopFocus();
	}

	// update last player focus try struct so its usable for next frame
	PreviousTryFocusData = CurrentTryFocusData;
	CurrentTryFocusData.Reset();
}

void UCISSourcePawnInteractionComponent::GetFocusInteractionTags(FGameplayTagContainer& OutTags) const
{
	
}
