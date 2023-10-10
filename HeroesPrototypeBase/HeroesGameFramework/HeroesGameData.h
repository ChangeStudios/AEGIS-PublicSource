// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HeroesGameData.generated.h"

class UGameplayEffect;

/**
 * Non-mutable data asset that contains global game data. This is a safe place to expose blueprint types to C++.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Heroes Game Data", ShortTooltip = "Data asset containing global game data."))
class UHeroesGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()

	// Utils.

public:

	/** Returns the loaded game data. */
	static const UHeroesGameData& Get();



	// Data.

public:

	/** Instant gameplay effect used to apply damage. Uses SetByCaller for the damage magnitude. This is used to expose the
	 * gameplay effect to C++. */
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects|Health", meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_Instant;

	/** Instant gameplay effect used to apply healing and overhealing. Uses SetByCaller for the healing magnitude. This is used
	 * to expose the gameplay effect to C++. */
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects|Health", meta = (DisplayName = "Heal Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> HealGameplayEffect_Instant;

	/** Duration-based gameplay effect used to modify acceleration. Uses SetByCaller for the acceleration magnitude.
	 * This is used to expose the gameplay effect to C++. */
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects|Movement", meta = (DisplayName = "Acceleration Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> AccelerationGameplayEffect_Duration;

};
