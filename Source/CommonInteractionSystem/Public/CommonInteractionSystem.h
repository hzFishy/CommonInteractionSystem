// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Modules/ModuleManager.h"
#include "Console/FUConsole.h"


#define CIS_WITH_DEBUG FU_WITH_CONSOLE


class FCommonInteractionSystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
