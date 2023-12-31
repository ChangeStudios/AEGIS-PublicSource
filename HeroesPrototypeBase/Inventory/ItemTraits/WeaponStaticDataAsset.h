// Copyright SamuelReitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponStaticDataAsset.generated.h"

class UCameraShakeBase;
class UCurveVector;
class UNiagaraSystem;

/**
 * Defines this weapon's fire mode.
 */
UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	FullAuto = 0	UMETA(DisplayName = "Fully Automatic"),
	SemiAuto = 1	UMETA(DisplayName = "Semi-Automatic"),
	BoltAction = 2	UMETA(DisplayName = "Animation-Per-Shot"), /* E.g. bolt-action, pump-action, etc. */
	Charged = 4		UMETA(DisplayName = "Charged"),
	Persistent = 5	UMETA(DisplayName = "Persistent")
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class HEROESPROTOTYPEBASE_API UWeaponStaticDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	// Gameplay.

	/** The curve that represents this weapon's recoil pattern. The X and Y curves define the horizontal and vertical
	 * recoil patterns, respectively. The Z curve does not represent anything. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveVector> RecoilCurve;

	/** The curve that represents this weapon's spread as a result of its current heat. The X and Y curves define the
	 * horizontal and vertical spread in degrees. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveVector> SpreadCurve;

	/** When aiming down sights, this weapon's spread is multiplied by this value. Lower numbers increase accuracy. */
	UPROPERTY(EditDefaultsOnly)
	float AimingSpreadMultiplier;

	/** When moving, this weapon's spread is multiplied by the curve value corresponding to their current movement
	 * speed (as a vector length float value). */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveVector> MovementSpreadMultiplierCurve;

	/** Fire-rate, in rounds-per-minute. Only used for fully automatic and semi-automatic weapons. */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "FireMode == 0 || FireMode == 1"), BlueprintReadOnly)
	int32 FireRate;

	/** How this weapon is fired. */
	UPROPERTY(EditDefaultsOnly)
	EWeaponFireMode FireMode;

	/** If this weapon can have a bullet in the chamber, increasing its magazine capacity by 1 when reloading without
	 * an empty magazine. */
	bool bCanBeChambered;

	/** The amount of time without firing another shot at which the weapon's "heat" begins to cool down, resetting
	 * the weapon's recoil and accuracy. */
	float TimeBeforeHeadCooldown;

	/** How quickly this weapon's heat scales from 0.0 to 1.0. Measured in heat-per-shot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HeatRate;

	/** The amount of time it takes for this weapon's heat to cool to 0.0 from 1.0 when it stops firing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HeatCooldownRate;

	/** The maximum size of this weapon's magazine. Used when a weapon is spawned and when a weapon is reloaded. 0 for
	 * weapons that do not use shot-based ammunition (e.g. swords). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MagazineSize;



	// VFX.

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> RecoilCameraShake;

	TObjectPtr<UNiagaraSystem> EnemyHitEffect;

	TObjectPtr<UNiagaraSystem> SurfaceHitEffect;
};
