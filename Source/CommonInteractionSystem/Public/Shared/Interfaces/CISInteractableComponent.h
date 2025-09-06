// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CISInteractableComponent.generated.h"


UINTERFACE()
class UCISInteractableComponent : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COMMONINTERACTIONSYSTEM_API ICISInteractableComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="CommonInteractionSystem")
	bool OnInteraction(APawn* SourcePawn, const FGameplayTagContainer& SourceInteractionTags, const FHitResult& HitResult);
};
