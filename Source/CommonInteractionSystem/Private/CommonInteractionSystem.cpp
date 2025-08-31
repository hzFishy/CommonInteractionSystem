// By hzFishy - 2025 - Do whatever you want with it.

#include "CommonInteractionSystem.h"

#define LOCTEXT_NAMESPACE "FCommonInteractionSystemModule"

void FCommonInteractionSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FCommonInteractionSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCommonInteractionSystemModule, CommonInteractionSystem)