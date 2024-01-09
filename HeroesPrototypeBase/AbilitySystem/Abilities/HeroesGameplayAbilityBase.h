// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HeroesGameplayAbilityBase.generated.h"

class AHeroesCharacterBase;
class UGameplayEffect;
class UHeroesAbilitySystemComponent;

/**
 * Defines how an ability is meant to be activated. This is used to determine how to bind the ability.
 */
UENUM(BlueprintType)
enum class EHeroesAbilityActivationStyle : uint8
{
	// Try to activate the ability immediately when its input is triggered.
	ActivateOnInputTriggered,
	// Try to activate the ability once when its input is triggered and cancel it when its input is triggered again.
	ToggleOnInputTriggered,
	// Try to activate the ability once when its input is triggered and cancel it when its input is completed.
	ActivateWhileInputHeld,
	// Continually try to activate the ability while the input is active.
	ActivateWhileInputActive,
	// Try to activate the ability when an avatar is assigned to its ASC.
	ActivateOnSpawn,
	// Try to activate the ability when it's granted to an ASC that already has an avatar.
	ActivatedOnGranted
};

/**
 * The base class for gameplay abilities. This class should be derived from and not used directly.
 *
 * This provides common functionality and various helper functions for gameplay abilities.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UHeroesGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

	// Helper functions

public:

	/** Returns this ability's instigating ASC. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|AbilitySystem|Abilities")
	UHeroesAbilitySystemComponent* GetHeroesAbilitySystemComponentFromActorInfo() const;

	/** Returns this ability's avatar actor if its class is a subclass of the base heroes character class. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|AbilitySystem|Abilities")
	AHeroesCharacterBase* GetCharacterFromActorInfo() const;



	// Ability handler functions.

public:

	/** Optional function called when this ability's input action is released. */
	virtual void OnInputReleased();

protected:

	/** Called when this ability is given to an ASC. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Heroes|AbilitySystem|Abilities", DisplayName="OnGiveAbility")
	void B_OnGiveAbility();

	/** Called when this ability is removed from an ASC. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Heroes|AbilitySystem|Abilities", DisplayName="OnRemoveAbility")
	void B_OnRemoveAbility();



	// Gameplay ability overrides.

protected:

	/** Ensures this ability's actor info is valid. */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	/** Calls the blueprint implementation of OnGiveAbility. */
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	/** Calls the blueprint implementation of OnRemoveAbility. */
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	/** Applies the gameplay effects applied by this ability. */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	/** Removes the gameplay effects applied by this ability that need to be removed when it ends. */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	/** Returns true if the player can afford this ability's costs. This is only used for characters that have "energy"-
	 * based abilities. */
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	/** Applies the cost of this ability. Costs are only applied when the ability successfully activates. */
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	/** Creates an effect context of type FHeroesGameplayEffectContext. */
	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;



	// Ability properties.

// Getters.
public:

	/** Getter for this ability's activation style. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|AbilitySystem|AbilityActivation")
	EHeroesAbilityActivationStyle GetActivationStyle() const { return ActivationStyle; }

// Properties.
protected:

	/** Defines how this ability is meant to be activated. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityActivation")
	EHeroesAbilityActivationStyle ActivationStyle;



	// Gameplay effects.

protected:

	/** Gameplay effects that are applied to the instigating ASC when this ability is activated and persist after the
	 * ability ends. These effects must be removed manually. */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToApplyOnStart;

	/** Gameplay effects that are applied to the instigating ASC when this ability is activated and automatically
	 * removed when this ability ends. */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToApplyOnStartAndRemoveOnEnd;

	/** Handles used to track effects applied by this ability that need to be removed when it ends. */
	TArray<FActiveGameplayEffectHandle> EffectsToRemoveOnEndHandles;
};
