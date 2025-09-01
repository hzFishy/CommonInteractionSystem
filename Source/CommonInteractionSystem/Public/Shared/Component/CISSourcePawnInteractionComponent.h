// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/ActorComponent.h"
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
 *	Interaction and focus.
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

	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Interaction")
	bool bCanInteract;

	/** Event tag used to trigger the interaction gameplay ability */
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Interaction", meta=(Categories="CIS.Interaction.Events"))
	FGameplayTag InteractionAbilityEventTag;
	
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Interaction")
	TSoftClassPtr<UCISInteractionGameplayAbility> InteractionAbilityClass;
	
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
	void OnInputSingleAndHoldInteractionStart(FGameplayTagContainer SourceInteractionTags);
	
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Interaction")
	void OnInputHoldInteractionEnd();
	
	UCISInteractionComponent* GetInteractionComponentFromInteractionTrace();
	
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Interaction")
	bool TryInteraction(const FGameplayTagContainer& SourceInteractionTags);
	
protected:
	bool IsHoldRunning() const;
	
	bool HoldInteractionStart(UCISInteractionComponent* InteractionComponent, const FGameplayTagContainer& SourceInteractionTags);

	/**
	 * Called when an object we were starting holding
	 * isn't focused anymore
	 */
	void OnHoldObjectFocusLostCallback(APawn* SourcePawn);
	
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
