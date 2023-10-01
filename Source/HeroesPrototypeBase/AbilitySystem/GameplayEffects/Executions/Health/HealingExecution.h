// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "HealingExecution.generated.h"

/**
 * Execution used by gameplay effects to apply healing to health attributes. Health attributes can only be modified via
 * executions and cannot be modified directly.
 *
 * This execution accounts for enemy healing, self-healing, incoming healing boosts, and outgoing healing boosts.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UHealingExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	/** Default constructor. */
	UHealingExecution();

protected:

	/** Performs the execution. */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;



	// Attributes that we want to capture to use in this calculation.

protected:

	/** The base healing applied by this execution, defined in the gameplay effect. */
	FGameplayEffectAttributeCaptureDefinition BaseHealingDef;

	/** The target's incoming healing multiplier attribute. */
	FGameplayEffectAttributeCaptureDefinition TargetIncomingHealingMultiplierDef;

	/** The source's outgoing healing multiplier attribute. */
	FGameplayEffectAttributeCaptureDefinition SourceOutgoingHealingMultiplierDef;

};
