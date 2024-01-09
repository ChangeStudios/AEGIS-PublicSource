// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HeroesGameplayAbilityBase.h"
#include "GA_DropItem.generated.h"

/**
 * Gameplay ability for the generic "drop item" action. Drops the player's currently equipped item if it is droppable.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UGA_DropItem : public UHeroesGameplayAbilityBase
{
	GENERATED_BODY()

public:

	UGA_DropItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	/** Ensures that this character has an item equipped right now. */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	/** Attempts to drop the equipped item using the inventory component's built-in drop function. */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
