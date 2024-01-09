// Copyright Samuel Reitich 2024.


#include "AbilitySystem/Abilities/HeroesGameplayAbilityBase.h"

#include "AbilitySystem/Components/HeroesAbilitySystemComponent.h"
#include "Characters/HeroesCharacterBase.h"

#include "AbilitySystemLog.h"
#include "AbilitySystem/HeroesGameplayEffectContext.h"

UHeroesAbilitySystemComponent* UHeroesGameplayAbilityBase::GetHeroesAbilitySystemComponentFromActorInfo() const
{
	// Return this ability has an actor, get its ASC.
	return (CurrentActorInfo ? Cast<UHeroesAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

AHeroesCharacterBase* UHeroesGameplayAbilityBase::GetCharacterFromActorInfo() const
{
	// If this ability has an avatar actor that is of the base heroes character subclass, return it casted to that class.
	if (CurrentActorInfo && GetAvatarActorFromActorInfo()->IsA(AHeroesCharacterBase::StaticClass()))
	{
		return Cast<AHeroesCharacterBase>(GetAvatarActorFromActorInfo());
	}

	return nullptr;
}

void UHeroesGameplayAbilityBase::OnInputReleased()
{}

bool UHeroesGameplayAbilityBase::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UHeroesGameplayAbilityBase::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	// Call optional blueprint logic for when an ability is given.
	B_OnGiveAbility();
	
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UHeroesGameplayAbilityBase::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	// Call optional blueprint logic for when an ability is removed.
	B_OnRemoveAbility();
	
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UHeroesGameplayAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Add this ability's ongoing effects to its ability system component.
	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		// Make a context handle with which to create the effects.
		const FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();

		// Apply every ongoing effect that ISN'T automatically removed when this ability ends.
		for (auto GameplayEffect : OngoingEffectsToApplyOnStart)
		{
			// Ensure the given effect is valid.
			if (!GameplayEffect.Get())
			{
				continue;
			}

			// Create a spec from the given effect class and context.
			FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(GameplayEffect, 1, EffectContextHandle);
			if (EffectSpecHandle.IsValid())
			{
				// Try to apply the new effect spec to the ASC.
				FActiveGameplayEffectHandle ActiveEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
				if (!ActiveEffectHandle.WasSuccessfullyApplied())
				{
					ABILITY_LOG(Log, TEXT("Ability %s failed to apply ongoing effect %s."), *GetName(), *GetNameSafe(GameplayEffect));
				}
			}
		}

		/* Apply every ongoing effect that IS automatically removed when this ability ends. To get the handles to
		 * to remove these effects later, this ability has to be instantiated. */
		if (IsInstantiated())
		{
			for (auto GameplayEffect : OngoingEffectsToApplyOnStartAndRemoveOnEnd)
			{
				// Ensure the given effect is valid.
				if (!GameplayEffect.Get())
				{
					continue;
				}

				// Create a spec from the given effect class and context.
				FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(GameplayEffect, 1, EffectContextHandle);
				if (EffectSpecHandle.IsValid())
				{
					// Try to apply the new effect spec to the ASC.
					FActiveGameplayEffectHandle ActiveEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
					if (!ActiveEffectHandle.WasSuccessfullyApplied())
					{
						ABILITY_LOG(Log, TEXT("Ability %s failed to apply ongoing effect %s."), *GetName(), *GetNameSafe(GameplayEffect));

						continue;
					}

					// If the effect was successfully applied, save its handle to remove it when this ability ends.
					EffectsToRemoveOnEndHandles.Add(ActiveEffectHandle);
				}
			}
		}
	}
}

void UHeroesGameplayAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	/* Remove every effect that was applied when this ability started that needs to be removed when its ends. We can
	 * only use handles to remove effects if this ability is instantiated. */
	if (IsInstantiated())
	{
		for (FActiveGameplayEffectHandle ActiveEffectHandle : EffectsToRemoveOnEndHandles)
		{
			if (ActiveEffectHandle.IsValid())
			{
				// Remove the applied effect via its handle.
				ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
			}
		}

		// After removing each applied effect, reset the array of effect handles.
		EffectsToRemoveOnEndHandles.Reset();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UHeroesGameplayAbilityBase::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

void UHeroesGameplayAbilityBase::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

FGameplayEffectContextHandle UHeroesGameplayAbilityBase::MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
{
	// Create a regular effect context.
	FGameplayEffectContextHandle OutEffectContextHandle = Super::MakeEffectContext(Handle, ActorInfo);
	// Get the effect context and convert it to an FHeroesGameplayEffectContext to be modified.
	FHeroesGameplayEffectContext* HeroesEffectContext = FHeroesGameplayEffectContext::GetHeroesContextFromHandle(OutEffectContextHandle);
	check(HeroesEffectContext)

	// Use HeroesEffectContext to modify any context values.

	return OutEffectContextHandle;
}
