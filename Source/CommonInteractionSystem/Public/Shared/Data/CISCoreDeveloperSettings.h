// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "GameplayTagContainer.h"
#include "CISCoreDeveloperSettings.generated.h"
class UCISFocusWidget;


/**
 * 
 */
UCLASS(Config=Game, DisplayName="Common Interaction System")
class COMMONINTERACTIONSYSTEM_API UCISCoreDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UCISCoreDeveloperSettings();

	virtual FName GetCategoryName() const override;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Focus")
	TSoftClassPtr<UCISFocusWidget> DefaultFocusWidgetClass;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Focus", meta=(Categories="CIS.Shared.Icons"))
	FGameplayTag DefaultFocusIconTag;
};
