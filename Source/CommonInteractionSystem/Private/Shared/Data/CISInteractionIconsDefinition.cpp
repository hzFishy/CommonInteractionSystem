// By hzFishy - 2025 - Do whatever you want with it.

#include "Shared/Data/CISInteractionIconsDefinition.h"


UTexture2D* UCISInteractionIconsDefinition::SyncLoadIcon(const FGameplayTag& IconTag) const
{
	if (auto* Entry = Icons.Find(IconTag))
	{
		return Entry->LoadSynchronous();
	}
	
	return nullptr;
}

TSoftObjectPtr<UTexture2D> UCISInteractionIconsDefinition::GetUnloadedIcon(const FGameplayTag& IconTag) const
{
	if (auto* Entry = Icons.Find(IconTag))
	{
		return *Entry;
	}

	return nullptr;
}
