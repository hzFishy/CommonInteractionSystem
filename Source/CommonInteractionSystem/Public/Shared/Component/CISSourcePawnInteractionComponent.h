// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/ActorComponent.h"
#include "Shared/Data/CISCoreTypes.h"
#include "CommonInteractionSystem.h"
#include "CISSourcePawnInteractionComponent.generated.h"
class UAbilitySystemComponent;
class UCISInteractionGameplayAbility;
class UCISInteractionComponent;
class UCISFocusComponent;


USTRUCT(BlueprintType, DisplayName="Focus Frame Data")
struct COMMONINTERACTIONSYSTEM_API FCISFocusFrameData
{
	GENERATED_BODY()

	FCISFocusFrameData();
	
	/** Result of CanFocus */
	bool bFocusResult;
	TWeakObjectPtr<AActor> FocusedActor;
	TWeakObjectPtr<UCISFocusComponent> FocusedBestFocusComponent;
#if CIS_WITH_DEBUG
	TWeakObjectPtr<UPrimitiveComponent> HitPrimitiveComponent;
#endif
	void Reset();
};


USTRUCT(BlueprintType, DisplayName="Interaction Running Process")
struct COMMONINTERACTIONSYSTEM_API FCISInteractionRunningProcess
{
	GENERATED_BODY()

	FCISInteractionRunningProcess();
	

	bool IsSingle() const;
	
	bool IsHold() const;

	void Reset();

	
	/////////////////////////////
	/// Shared
	/** True if a interaction is handled this frame */
	bool bRunning;
	FGameplayTag InteractionTypeTag;
	FGameplayTagContainer SourceInteractionTags;
	FGameplayTagContainer FocusTags;
	FTimerHandle TimerHandle;
	TWeakObjectPtr<UCISInteractionComponent> InteractionComponent;
	TWeakObjectPtr<UCISFocusComponent> SelectedFocusComponent;
	
	/////////////////////////////
	/// Hold Only
	FDelegateHandle HoldInteractionDelegateHandle;
	/** True when the hold time elapsed (no cancels) */
	bool bHoldInteractionFinished;
	bool bHoldInteractionSuccessful;
};


/**
 *	Interaction and focus features for a pawn which has an ASC.
 *	Use the various "On Input ..." functions to trigger the system.
 *	
 *	You must set the InteractionStartPoint with SetInteractionStartPoint.
 *	The player HUD must implement UFWSWorldSpaceContainerWidgetGetter with GetWorldSpaceContainerWidget which returns a UFWSWorldSpaceContainerWidget.
 */
UCLASS(ClassGroup=(CommonInteractionSystem), Blueprintable, DisplayName="Source Pawn Interaction Component", meta=(BlueprintSpawnableComponent))
class COMMONINTERACTIONSYSTEM_API UCISSourcePawnInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Properties
	----------------------------------------------------------------------------*/
protected:
	/** Used for interactions and focus */
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Shared")
	float InteractionLineTraceLength;
	
	/** Used for interactions and focus */
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Shared")
	float InteractionSphereRadius;

	/** Used for interactions and focus */
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Shared")
	TEnumAsByte<ECollisionChannel> TraceChannel;
	
	/** If true we do not rely on GAS and call directly TryInteraction */
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Shared")
	bool bNoGAS;
	
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Interaction")
	bool bCanInteract;

	/** Event tag used to trigger the interaction gameplay ability */
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Interaction", meta=(Categories="CIS.Interaction.Events", EditCondition="!bNoGAS"))
	FGameplayTag InteractionAbilityEventTag;

	/**
	 *  You have to implement "Activate Ability From Event" with InteractionAbilityEventTag.
	 *  If the commit is successfull you can call ExecuteInteraction
	 *  and use GetInteractionAbilityEventData to get the input tags.
	 *  You need to end the ability if its singe or let it run if hold,
	 *  to know the type of the interaction read InteractionTagType on the optional object.
	 */
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Interaction", meta=(EditCondition="!bNoGAS"))
	TSoftClassPtr<UCISInteractionGameplayAbility> InteractionAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Interaction", meta=(EditCondition="!bNoGAS"))
	bool bAsyncLoadInteractionAbilityClass;
	
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Focus")
	bool bCanFocus;

	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Focus")
	bool bTryFocusOnTick;
	
	
	/////////////////////////////
	/// Shared
	/* Interaction and focus start trace location */
	TWeakObjectPtr<USceneComponent> InteractionStartPoint;
	TWeakObjectPtr<APawn> OwnerSourcePawn;
	TWeakObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
	FCISInteractionRunningProcess SharedInteractionRunningProcess;

	/////////////////////////////
	/// Interaction
	UPROPERTY() TSubclassOf<UCISInteractionGameplayAbility> LoadedInteractionAbilityClass;
	FGameplayAbilitySpecHandle InteractionAbilitySpecHandle;
	
	/////////////////////////////
	/// Focus
	FHitResult LastFocusSweepResult;
	FCISFocusFrameData PreviousTryFocusData;
	/**
	 * Focus Data of current TryFocus execution, this is only valid inside the function.
	 * see PreviousTryFocusData for external use
	 */
	FCISFocusFrameData CurrentTryFocusData;

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCISSourcePawnInteractionComponent();
	
	virtual void InitializeComponent() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	/*----------------------------------------------------------------------------
		Shared
	----------------------------------------------------------------------------*/
public:
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Interaction")
	void SetInteractionStartPoint(USceneComponent* SceneComponent);
	
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Interaction")
	void SetCanInteract(bool bNewValue);
	
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Interaction")
	void SetCanFocus(bool bNewValue);
	
protected:
	void DoSharedInteractionTrace(FHitResult& OutResult, bool bFocusTrace);

	
	/*----------------------------------------------------------------------------
		Interaction
	----------------------------------------------------------------------------*/
public:
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Interaction")
	void OnInputSingleInteraction(FGameplayTagContainer SourceInteractionTags);
	
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Interaction")
	void OnInputHoldStartInteraction(FGameplayTagContainer SourceInteractionTags);
	
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Interaction")
	void OnInputHoldEndInteraction();
	
	UCISInteractionComponent* GetInteractionComponentFromInteractionTrace();
	
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Interaction")
	bool TryInteraction(const FGameplayTagContainer& SourceInteractionTags);
	
protected:
	void OnInputSingleOrHoldInteractionStart(UCISInteractionComponent* InteractionComponent,
		const FGameplayTag& InteractionTypeTag, const FGameplayTagContainer& SourceInteractionTags);
	
	bool IsHoldRunning() const;
	
	bool HoldInteractionStart(UCISInteractionComponent* InteractionComponent, FCISInteractionParams& InteractionParams);

	/**
	 * Called when an object we were starting holding
	 * isn't focused anymore
	 */
	void OnHoldObjectFocusLostCallback(APawn* SourcePawn);

	/** Callback from the current target interaction component for a hold interaction */
	UFUNCTION() void OnCurrentHoldInteractableStateChanged(bool bNewState);
	
	/** Called once the hold time passed or if canceled */
	void OnHoldInteractionFinished(bool bSuccess);

	void OnAbilityEnded(const FAbilityEndedData& EndedData);
	
	/** Cancel hold interaction */
	void CancelHoldInteraction();

	/** Clean hold interaction */
	void CleanHoldInteraction();

	
	/*----------------------------------------------------------------------------
		Focus
	----------------------------------------------------------------------------*/
public:
	/* Trace for focusable objects */
	void TryFocus();

	const FHitResult& GetLastFocusSweepResult() const { return LastFocusSweepResult; };

	virtual void GetFocusInteractionTags(FGameplayTagContainer& OutTags) const;
};
