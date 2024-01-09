// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "HeroesAttributeSetBase.generated.h"

class UHealthComponent;
class UHeroesAbilitySystemComponent;
struct FGameplayEffectSpec;

/**
 * This macro defines a set of helper functions for accessing and initializing attributes.
 *
 * The following example of the macro:
 *		ATTRIBUTE_ACCESSORS(UHeroesHealthSet, Health)
 * will create the following functions:
 *		static FGameplayAttribute GetHealthAttribute();
 *		float GetHealth() const;
 *		void SetHealth(float NewVal);
 *		void InitHealth(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/** Delegate used to broadcast attribute events, such as running out of health. */
DECLARE_MULTICAST_DELEGATE_FourParams(FAttributeEventSignature, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitude*/);
/** Delegate used to broadcast attribute changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FAttributeChangedSignature, UActorComponent*, AttributeComponent, float, OldValue, float, NewValue, AActor*, Instigator);


/**
 * The base class for attribute sets. Handles attribute class setup and provides utilities. This class should be
 * derived from and not used directly.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UHeroesAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:

	/** Returns the UWorld of this attribute set's outer object instead of trying to retrieve it itself. */
	UWorld* GetWorld() const override;

	/** Returns this attribute set's owning ASC, cast to the UHeroesAbilitySystemComponent class. */
	UHeroesAbilitySystemComponent* GetHeroesAbilitySystemComponent() const;
};
