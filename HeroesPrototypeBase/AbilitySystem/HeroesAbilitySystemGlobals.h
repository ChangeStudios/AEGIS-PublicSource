// Copyright Samuel Reitich 2024.

#pragma once

#include "AbilitySystemGlobals.h"
#include "HeroesAbilitySystemGlobals.generated.h"

class UHeroesAbilitySystemComponent;
class UObject;
struct FGameplayEffectContext;

/**
 * Provides global utilities for the ability system and handles global ability system actions.
 */
UCLASS(Config=Game)
class UHeroesAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_UCLASS_BODY()

	/** Overridden to create a new FHeroesGameplayEffectContext instead of a new FGameplayEffectContext. */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;

	/** Searches the passed actor for a heroes ability system component using IAbilitySystemInterface. Will revert to a
	 * standard component search if the interface is missing. */
	static UHeroesAbilitySystemComponent* GetHeroesAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = true);

};
