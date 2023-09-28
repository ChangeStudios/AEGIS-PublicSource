// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HeroesGameplayAbilityBase.h"
#include "GA_Jump.generated.h"

/**
 * Gameplay ability for the default jump action.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UGA_Jump : public UHeroesGameplayAbilityBase
{
	GENERATED_BODY()

public:

	UGA_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Ends this ability when its input action is released. */
	virtual void OnInputReleased() override;

protected:

	/** Ensures that this character can jump right now. */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	/** Attempts to jump using built-in character movement. */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Ensures that we stop jumping in case it wasn't handled automatically. */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
