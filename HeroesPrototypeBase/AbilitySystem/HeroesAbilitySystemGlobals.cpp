// Copyright Samuel Reitich 2023.


#include "HeroesAbilitySystemGlobals.h"

#include "AbilitySystemComponent.h"
#include "HeroesGameplayEffectContext.h"
#include "Components/HeroesAbilitySystemComponent.h"

struct FGameplayEffectContext;

UHeroesAbilitySystemGlobals::UHeroesAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UHeroesAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FHeroesGameplayEffectContext();
}

UHeroesAbilitySystemComponent* UHeroesAbilitySystemGlobals::GetHeroesAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActor(Actor, LookForComponent);
	UHeroesAbilitySystemComponent* HeroesASC = ASC ? Cast<UHeroesAbilitySystemComponent>(ASC) : nullptr;

	return HeroesASC ? HeroesASC : nullptr;
}