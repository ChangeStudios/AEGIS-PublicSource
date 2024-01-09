// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DamageExecutionDataAsset.generated.h"

class UCurveFloat;

/**
 * Data asset that defines rules for a gameplay effect's applied damage. These are used in the DamageExecution to
 * calculate the final applied damage value.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UDamageExecutionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Whether or not this damage affect can damage damage the actor who created it. */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Execution Data")
	bool bCanDamageSelf = false;

	/** Whether or not this damage effect can affect teammates, or if it can only damage enemies. This does not include
	 * the actor who created the effect. CanDamageSelf overrides this. */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Execution Data")
	bool bCanDamageAllies = false;

	/** Whether or not damage falloff is enabled for this effect. Falloff is calculated using the distance between
	 * the effect causer and the target at the time of effect application. If this is true, then the given damage
	 * will be overridden by the damage falloff curve's given damage. */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Execution Data")
	bool bDamageFalloffEnabled = false;

	/** The curve that determines that damage falloff, if it is enabled for this effect. The X-axis represents the
	 * distance between the effect causer and the target, in meters, and the Y-axis is the corresponding damage at
	 * that distance. This overrides the damage provided to the damage execution. */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Execution Data")
	UCurveFloat* DamageFalloffCurve = nullptr;

	/** Whether or not this damage effect can make critical hits. This should be true for most hitscan damage and false for
	 * most projectile and AoE damage. */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Execution Data")
	bool bCanCrit = true;

	/** How much the base damage should be scaled if this effect made a critical hit, if this effect can make critical
	 * hits. */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Execution Data")
	float CritMultiplier = 2.0f;

	/** If true, the damage value will be applied exactly, ignoring any incoming/outgoing damage multipliers. Damage
	 * will still be ignored if the target has full damage invulnerability. */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Execution Data")
	bool bTrueDamage = false;

};
