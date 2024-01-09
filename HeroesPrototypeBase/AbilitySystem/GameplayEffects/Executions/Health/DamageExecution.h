// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DamageExecution.generated.h"

/**
 * Execution used by gameplay effects to apply damage to health attributes. Health attributes can only be modified via
 * executions and cannot be modified directly.
 *
 * This execution accounts for team damage, self-damage, damage fall-off, critical hits, outgoing damage boosts, and
 * incoming damage boosts.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	/** Default constructor. */
	UDamageExecution();

protected:

	/** Performs the execution. */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;



	// Attributes that we want to capture to use in this calculation.

protected:

	/** The base damage dealt by this execution, defined in the gameplay effect. */
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	/** The target's incoming damage multiplier attribute. */
	FGameplayEffectAttributeCaptureDefinition TargetIncomingDamageMultiplierDef;

	/** The source's outgoing damage multiplier attribute. */
	FGameplayEffectAttributeCaptureDefinition SourceOutgoingDamageMultiplierDef;

};
