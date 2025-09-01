// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "CISInteractionIconsDefinition.generated.h"


/**
 * Holds data relative to icons
 */
UCLASS(ClassGroup=CommonInteractionSystem, DisplayName="Interaction Icons Definition")
class COMMONINTERACTIONSYSTEM_API UCISInteractionIconsDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CommonInteractionSystem")
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> Icons;

	UFUNCTION(BlueprintPure, Category = "CommonInteractionSystem")
	UTexture2D* SyncLoadIcon(const FGameplayTag& IconTag) const;
	
	UFUNCTION(BlueprintPure, Category = "CommonInteractionSystem")
	TSoftObjectPtr<UTexture2D> GetUnloadedIcon(const FGameplayTag& IconTag) const;
};
