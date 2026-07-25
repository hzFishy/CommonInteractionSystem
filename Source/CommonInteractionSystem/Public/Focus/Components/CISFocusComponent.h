// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintComponentReference.h"
#include "GameplayTagContainer.h"
#include "Components/SceneComponent.h"
#include "Shared/Data/CISCoreTypes.h"
#include "CISFocusComponent.generated.h"
class UCISInteractionComponent;
class UCISFocusWidget;


DECLARE_MULTICAST_DELEGATE_OneParam(FCISOnFocusLostSignature, APawn* /* SourcePawn */);


/**
 * Component used by the other interaction components, placed on interactable actors.
 * You can have multiple focus components on one actor.
 * To know what focus component to use depending on what primitive component was hit, we use a binding system, see LinkedPrimitiveComponents
 * Examples of setup:
 * - FocusComp1
 *    - Linked to BoxComponent1
 *    - Linked to SphereComponent2
 * - FocusComp2
 *    - Linked to CapsuleComponent1
 * 
 * FocusComp1 will be used if the focus query hit BoxComponent1 or SphereComponent2. And FocusComp2 will be used if it was CapsuleComponent1.
 * If none of these 3 primitive components is hit then nothing will happen.
 * 
 * This component is used for UI overlays and more.
 * 
 * Since this component has its own bFocusable the default behavior is to have bFocusable match bInteractable on the linked InteractionComponent
 */
UCLASS(ClassGroup=(CommonInteractionSystem), Blueprintable, DisplayName="Focus Component", meta=(BlueprintSpawnableComponent))
class COMMONINTERACTIONSYSTEM_API UCISFocusComponent : public USceneComponent
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Properties
	----------------------------------------------------------------------------*/
public:
	/* Called when component was focused then stopped being focused */
	FCISOnFocusLostSignature OnFocusLostDelegate;
	
protected:
	/** At least of of these components needs to be hit with a focus query to activiate this focus component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup", meta=(AllowedClasses="/Script/Engine.PrimitiveComponent"))
	TArray<FBlueprintComponentReference> LinkedPrimitiveComponents;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup")
	FText FocusText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup")
	FGameplayTagContainer FocusTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup", meta=(Categories="CIS.Shared.Icons"))
	FGameplayTag IconFocusTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup")
	bool bFocusableByDefault;

	/** If set this will override DefaultFocusWidgetClass in the Project Settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup")
	TSoftClassPtr<UCISFocusWidget> FocusWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CommonInteractionSystem|Setup")
	bool bAsyncLoadFocusWidgetClass;
	
	
	TWeakObjectPtr<UCISInteractionComponent> FoundOwnerInteractionComponent;
	
	UPROPERTY() TSubclassOf<UCISFocusWidget> LoadedFocusWidgetClass;
	
	bool bFocusable;
	
	bool bIsFocused;

	/* Pawn that IS currently focusing */
	TWeakObjectPtr<APawn> FocusingSourcePawn;

	/** Current displaying focus widget instance */
	UPROPERTY() TObjectPtr<UCISFocusWidget> FocusWidget;
	/** Last saved focus params used when focusing */
	FCISInteractionFocusParams LatestFocusParams;
	
	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCISFocusComponent();
	
	virtual void InitializeComponent() override;

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
public:
	const TArray<FBlueprintComponentReference>& GetLinkedPrimitiveComponents() const { return LinkedPrimitiveComponents; };
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Focus")
	const FText& GetFocusText() const { return FocusText; };
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Focus")
	void GetFocusTags(FGameplayTagContainer& Container) const;
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Focus")
	const FGameplayTag& GetIconFocusTag() const { return IconFocusTag; };
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Focus")
	bool IsFocusable() const { return bFocusable; };
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Focus")
	bool IsFocused() const { return bIsFocused; };
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Focus")
	bool IsFocusedBy(APawn* SourcePawn) const { return bIsFocused && FocusingSourcePawn.Get() == SourcePawn; };
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Focus")
	APawn* GetFocusingSourcePawn() const { return FocusingSourcePawn.Get(); };
	
	UFUNCTION(BlueprintPure, Category="CommonInteractionSystem|Focus")
	UCISFocusWidget* GetFocusWidget() const { return FocusWidget.Get(); };
	
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Focus")
	virtual void SetFocusText(const FText& NewFocusText);

	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Focus")
	virtual void SetIconFocusTag(const FGameplayTag& NewIconFocusTag);

	/** Start focusing this focus component, can fail */
	virtual void StartFocus(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, const FHitResult& FocusHitResult);

	/** Stop focusing this focus component */
	virtual void StopFocus();

	virtual bool CanFocus(APawn* SourcePawn, FCISInteractionFocusParams& InteractionFocusParams);

	template<std::derived_from<UUserWidget> WidgetType>
	WidgetType* CreateFocusWidget(const FCISInteractionFocusParams& FocusParams)
	{
		return Cast<WidgetType>(CreateFocusWidget(FocusParams));
	}
	
	virtual UUserWidget* CreateFocusWidget(const FCISInteractionFocusParams& FocusParams);
	
	TSubclassOf<UCISFocusWidget> GetFocusWidgetClass();

	virtual void UpdateFocusWidgetContent(const FCISInteractionFocusParams& FocusParams);
	
	/** Update focus widget content if focusable, if not we stop the focus */
	UFUNCTION(BlueprintCallable, Category="CommonInteractionSystem|Focus")
	virtual void RefreshFocusWidget(bool bCheckFocusStatus = true);

	
	/*----------------------------------------------------------------------------
		Callbacks
	----------------------------------------------------------------------------*/
protected:
	UFUNCTION() void OnInteractableStateChanged(bool bNewState);

	UFUNCTION() void OnSingleInteraction(UCISInteractionComponent* InteractionComponent, APawn* SourcePawn,
		const FGameplayTagContainer& SourceInteractionTags, const FGameplayTagContainer& InFocusTags, bool bInteractionSucceeded);
	
	UFUNCTION() void OnHoldInteractionStarted(UCISInteractionComponent* InteractionComponent, APawn* SourcePawn,
		const FGameplayTagContainer& SourceInteractionTags, const FGameplayTagContainer& InFocusTags, bool bInteractionSucceeded, float TimeToHold);
	
	UFUNCTION() void OnHoldInteractionEnded(UCISInteractionComponent* InteractionComponent, APawn* SourcePawn,
		const FGameplayTagContainer& SourceInteractionTags, const FGameplayTagContainer& InFocusTags, bool bHoldSucceeded);

	UFUNCTION() void OnOwnerDestroyed(AActor* DestroyedActor);
};
