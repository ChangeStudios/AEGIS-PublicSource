// Copyright Samuel Reitich 2024.


#include "AbilityInputComponent.h"

#include "AbilitySystem/Abilities/HeroesAbilitySet.h"
#include "AbilitySystem/Components/HeroesAbilitySystemComponent.h"
#include "HeroesLogChannels.h"
#include "AbilitySystem/HeroesAbilitySystemGlobals.h"
#include "Input/HeroesInputActionSet.h"


UAbilityInputComponent* UAbilityInputComponent::FindAbilityInputComponent(const AActor* Actor)
{
	// Try to find a component matching this class in the given actor. Return a nullptr otherwise.
	return (Actor ? Actor->FindComponentByClass<UAbilityInputComponent>() : nullptr);
}

void UAbilityInputComponent::BindAbilityActions(const UHeroesInputActionSet* ActionsToBind)
{
	// Bind every ability's input action to this component's handler function, which activates the ability using its tag.
	for (const FHeroesInputAction& AbilityInputAction : ActionsToBind->AbilityInputActions)
	{
		BindAction(AbilityInputAction.InputAction, ETriggerEvent::Triggered, this, &ThisClass::Input_AbilityInputTagPressed, AbilityInputAction.InputTag);
		BindAction(AbilityInputAction.InputAction, ETriggerEvent::Completed, this, &ThisClass::Input_AbilityInputTagReleased, AbilityInputAction.InputTag);
	}
}

void UAbilityInputComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	// Activate the triggered ability using its input tag.
	if (const APawn* Pawn = GetOwner<APawn>())
	{
		if (UHeroesAbilitySystemComponent* HeroesASC = UHeroesAbilitySystemGlobals::GetHeroesAbilitySystemComponentFromActor(Pawn))
		{
			// Cache the array in case it changes while iterating.
			const TArray<FGameplayAbilitySpec> ActivatableAbilities = HeroesASC->GetActivatableAbilities();
			for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities)
			{
				// The input tag with which to activate an ability is added to DynamicAbilityTags when an ability set is given to an ability system.
				if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
				{
					if (UHeroesGameplayAbilityBase* Ability = Cast<UHeroesGameplayAbilityBase>(AbilitySpec.Ability))
					{
						// Try to activate the ability depending on its activation style.
						switch (Ability->GetActivationStyle())
						{
						// Try to activate the ability if it is immediately activated when its input is triggered.
						case EHeroesAbilityActivationStyle::ActivateOnInputTriggered:
							{
								HeroesASC->TryActivateAbility(AbilitySpec.Handle);
								break;
							}

						// If the ability should be toggled, activate it or cancel it depending on whether or not it is currently active.
						case EHeroesAbilityActivationStyle::ToggleOnInputTriggered:
							{
								if (AbilitySpec.IsActive())
								{
									HeroesASC->CancelAbility(Ability);
								}
								else
								{
									HeroesASC->TryActivateAbility(AbilitySpec.Handle);
								}
								break;
							}

						// Tru to activate the ability once if it should be activated when its input is first triggered, then cancelled when the input is completed.
						case EHeroesAbilityActivationStyle::ActivateWhileInputHeld:
							{
								HeroesASC->TryActivateAbility(AbilitySpec.Handle);
								break;
							}

						// Try to activate the ability once if it should be continually activated while its input is active. The input action's triggers will handle continuous activation.
						case EHeroesAbilityActivationStyle::ActivateWhileInputActive:
							{
								HeroesASC->TryActivateAbility(AbilitySpec.Handle);
								break;
							}

						// No other activation styles use input to activate their abilities.
						default:
							UE_LOG(LogHeroes, Warning, TEXT("Tried to activate ability %s with input, but its activation style does not use input!"), *GetNameSafe(Ability));
							break;
						}
					}
				}
			}
		}
	}
}

void UAbilityInputComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	// Activate the triggered ability using its input tag.
	if (const APawn* Pawn = GetOwner<APawn>())
	{
		if (UHeroesAbilitySystemComponent* HeroesASC = UHeroesAbilitySystemGlobals::GetHeroesAbilitySystemComponentFromActor(Pawn))
		{
			for (const FGameplayAbilitySpec& AbilitySpec : HeroesASC->GetActivatableAbilities())
			{
				// The input tag with which to activate an ability is added to DynamicAbilityTags when an ability set is given to an ability system.
				if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
				{
					if (UHeroesGameplayAbilityBase* Ability = Cast<UHeroesGameplayAbilityBase>(AbilitySpec.Ability))
					{
						// When an input is completed, cancel any active abilities that need to be cancelled when their input is released.
						if (Ability->GetActivationStyle() == EHeroesAbilityActivationStyle::ActivateWhileInputHeld)
						{
							HeroesASC->CancelAbility(Ability);
						}
					}
				}
			}
		}
	}
}
