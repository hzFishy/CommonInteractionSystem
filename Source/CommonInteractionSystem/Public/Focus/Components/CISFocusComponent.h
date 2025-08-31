// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SceneComponent.h"
#include "CISFocusComponent.generated.h"


class UCISInteractionComponent;
class UCISFocusWidget;
DECLARE_MULTICAST_DELEGATE(FCISOnFocusLostSignature);


/**
 * Component used by the other interaction components, placed on interactable actors.
 * You can have multiple focus components on one actor, the closest will be used.
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
	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Setup")
	FText FocusText;

	UPROPERTY(EditAnywhere, Category="CommonInteractionSystem|Setup")
	FGameplayTag IconFocusTag;


	bool bFocusable = true;
	bool bIsFocused = false;

	UPROPERTY() TSubclassOf<UCISFocusWidget> LoadedFocusWidgetClass;
	UPROPERTY() TObjectPtr<UCISFocusWidget> FocusWidget;

	TWeakObjectPtr<UBPGWorldSpaceWidgetsSubsystem> WorldSpaceWidgetsSubsystem;
	TWeakObjectPtr<UCISInteractionComponent> FoundOwnerInteractionComponent;
	
	/* Panw that IS currently focusing */
	TWeakObjectPtr<APawn> FocusingSourcePawn;
	
	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCISFocusComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitializeComponent() override;

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/


#pragma region Core

public:
	void SetFocusText(const FText& InText);

	void SetActionType(FGameplayTag Tag);
	
protected:
	void OnUseComponentUsableStateChangedCallback(bool bNewState);

	UFUNCTION() void OnUseComponentUse(APawn* Character, EBPGCoreUseType CoreUseType, EBPGUseTypes UseType);
	
	UUserWidget* CreateFocusWidget(const BPG_InteractionSystem::FBPGInteractionCustomizationCanFocusParams& ExtraParams);
	
public:
	void StartFocus(APawn* P);

	void StopFocus(ABPGPlayerPawnInGame* FocusingCharacter);

	bool CanStartFocus(APawn* FocusingPlayerPawn, bool bSkipIfAlreadyFocusing, BPG_InteractionSystem::FBPGInteractionCustomizationCanFocusParams& OutExtraParams);

	void StartHoldUse(float TimeToHold);

	void EndHoldUse(bool bSuccess);

	/* Will check again if we can focus, will update again the Extra params such as override text */
	void RefreshFocusWidget();

#pragma endregion
	
	
};
