// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CISCoreTypes.generated.h"

	/*----------------------------------------------------------------------------
		Logging
	----------------------------------------------------------------------------*/
COMMONINTERACTIONSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(LogCISValidation, Log, Log);

#define CIS_LOG_Validation_E(FORMAT, ...) FU_LOG_Validation_E(LogCISValidation, FORMAT, ##__VA_ARGS__);


	/*----------------------------------------------------------------------------
		Delegate Signatures
	----------------------------------------------------------------------------*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCISOnInteractableStateChangedSignature, bool, bnewState);
