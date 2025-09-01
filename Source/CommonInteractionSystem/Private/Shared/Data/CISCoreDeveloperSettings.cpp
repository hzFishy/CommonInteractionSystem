// By hzFishy - 2025 - Do whatever you want with it.


#include "Shared/Data/CISCoreDeveloperSettings.h"

#include "Focus/Widgets/CISFocusWidget.h"
#include "Shared/Data/CISCoreTypes.h"


UCISCoreDeveloperSettings::UCISCoreDeveloperSettings():
	DefaultFocusWidgetClass(UCISFocusWidget::StaticClass()),
	DefaultFocusIconTag(TAG_CIS_Shared_Icons_DefaultInteraction)
{
	
}

FName UCISCoreDeveloperSettings::GetCategoryName() const
{
	return "Plugins";
}
