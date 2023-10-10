// Copyright Samuel Reitich 2023.


#include "AbilitySystem/AttributeSets/HeroesAttributeSetBase.h"

#include "AbilitySystem/Components/HeroesAbilitySystemComponent.h"

// class UWorld;

UWorld* UHeroesAttributeSetBase::GetWorld() const
{
	// Return this attribute set's outer object's UWorld.
	const UObject* OuterObject = GetOuter();
	check(OuterObject);
	return OuterObject->GetWorld();
}

UHeroesAbilitySystemComponent* UHeroesAttributeSetBase::GetHeroesAbilitySystemComponent() const
{
	return Cast<UHeroesAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
