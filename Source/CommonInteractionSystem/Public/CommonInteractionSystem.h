// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Modules/ModuleManager.h"
#include "Console/FUConsole.h"


#define CIS_WITH_DEBUG FU_WITH_CONSOLE

COMMONINTERACTIONSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(LogCommonInteractionSystem, Log, All);
#define CIS_LOG_D(FORMAT, ...) _FU_LOG_OBJECT_D(LogCommonInteractionSystem, FORMAT, ##__VA_ARGS__)
#define CIS_LOG_W(FORMAT, ...) _FU_LOG_OBJECT_W(LogCommonInteractionSystem, FORMAT, ##__VA_ARGS__)
#define CIS_LOG_E(FORMAT, ...) _FU_LOG_OBJECT_E(LogCommonInteractionSystem, FORMAT, ##__VA_ARGS__)


class FCommonInteractionSystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
