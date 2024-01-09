// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "HeroesAbilitySystemComponent.generated.h"

/**
 * The ability system component class used by all actors in this project that want to utilize the gameplay abilities
 * system. This component provides an interface for its owning actor to interact with GAS.
 */
UCLASS(ClassGroup = AbilitySystem, meta = (BlueprintSpawnableComponent))
class HEROESPROTOTYPEBASE_API UHeroesAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
};
