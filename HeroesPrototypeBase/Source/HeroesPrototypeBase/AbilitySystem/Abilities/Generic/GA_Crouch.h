// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HeroesGameplayAbilityBase.h"
#include "GA_Crouch.generated.h"

/**
 * Gameplay ability for the default crouch action.
 *
 * When this ability is activated, it provides a "Crouching" effect and tries to crouch if possible. Whenever the
 * character's movement mode changes (e.g. if they land after jumping), it will check if the character has the
 * "Crouching" effect but is not crouching. If so, it will try to crouch again. This means that as long as a character
 * is trying to crouch, it will eventually do so.
 *
 * When this ability is cancelled, this "Crouching" effect is removed and the character uncrouches as soon as possible
 * (e.g. if they are stuck underneath something and cannot uncrouch yet).
 *
 * Crouching on activation and uncrouching when ending means that this ability will be used as the default crouch
 * ability regardless of the input style (i.e. held vs toggled). The only thing that needs to be changed to change
 * this ability's activation behavior is its activation style, which can be done by subclassing it in blueprints. The
 * ability input component ensures that abilities will be activated and ended at the correct time according to their
 * input and activation style.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UGA_Crouch : public UHeroesGameplayAbilityBase
{
	GENERATED_BODY()

public:

	UGA_Crouch(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	/** Characters can always crouch; it is just a matter of when it takes effect. */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	/** Tries to crouch if possible. If not, we will keep trying to crouch every time our character's movement mode changes, as long as this ability is active. */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Uncrouches as soon as possible. If we cannot uncrouch yet, we still end the ability. The built-in crouch logic handles cases when we cannot uncrouch yet. */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
