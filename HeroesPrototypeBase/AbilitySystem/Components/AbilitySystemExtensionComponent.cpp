// Copyright Samuel Reitich 2024.


#include "AbilitySystem/Components/AbilitySystemExtensionComponent.h"

#include "CombatComponent.h"
#include "HealthComponent.h"
#include "HeroesCharacterMovementComponent.h"
#include "HeroesLogChannels.h"
#include "AbilitySystem/Components/HeroesAbilitySystemComponent.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Behavior_PersistsThroughAvatarDestruction, "Ability.Behavior.PersistsThroughAvatarDestruction", "Indicates that an ability is not canceled when its activator (its ASC’s avatar) dies or is unpossessed.");

UAbilitySystemExtensionComponent::UAbilitySystemExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// The ASC needs to be set when the owning pawn is possessed by a player with one.
	AbilitySystemComponent = nullptr;
}

UAbilitySystemExtensionComponent* UAbilitySystemExtensionComponent::FindAbilitySystemExtensionComponent(const AActor* Actor)
{
	// Try to find a component matching this class in the given actor. Return a nullptr otherwise.
	return (Actor ? Actor->FindComponentByClass<UAbilitySystemExtensionComponent>() : nullptr);
}

void UAbilitySystemExtensionComponent::InitializeAbilitySystem(UHeroesAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	// If the ASC has not changed, do not initialize it again.
	if (AbilitySystemComponent == InASC)
	{
		return;
	}

	// If the pawn had a previous ASC, clean it up before getting the new one.
	if (AbilitySystemComponent)
	{
		UninitializeAbilitySystem();
	}

	// Cache this component's owning pawn.
	APawn* Pawn = GetOwner<APawn>();
	// Retrieve the specified ASC's current avatar if it already has one.
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogHeroes, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	// If the specified ASC already had a pawn acting as its avatar, we need to kick it out so that this component's owning pawn can become the avatar.
	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		UE_LOG(LogHeroes, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		// This only happens on clients when lagged. Their new pawn may be spawned and possessed before the old one is removed.
		ensure(!ExistingAvatar->HasAuthority());

		if (UAbilitySystemExtensionComponent* OtherExtensionComponent = FindAbilitySystemExtensionComponent(ExistingAvatar))
		{
			// Uninitialize the ASC from the old avatar.
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	// Cache the new ASC.
	AbilitySystemComponent = InASC;
	// Set this component's owning pawn as the ASC's new avatar.
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	// Initialize the health component, if the ASC's owner has one.
	if (UHealthComponent* HealthComponent = UHealthComponent::FindHealthComponent(GetOwner()))
	{
		HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
	}

	// Initialize the combat component, if the ASC's owner has one.
	if (UCombatComponent* CombatComponent = UCombatComponent::FindCombatComponent(GetOwner()))
	{
		CombatComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
	}

	// Initialize the movement component, if the ASC's owner has one.
	if (UHeroesCharacterMovementComponent* MovementComponent = UHeroesCharacterMovementComponent::FindHeroesCharacterMovementComponent(GetOwner()))
	{
		MovementComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
	}
}

void UAbilitySystemExtensionComponent::UninitializeAbilitySystem()
{
	// We can't uninitialize an ASC if we don't have one.
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Only uninitialize the ASC if this component's owning pawn is its current avatar.
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		// Cancel all ongoing abilities, except for ones marked to persist through their ASC's avatar's destruction.
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(TAG_Ability_Behavior_PersistsThroughAvatarDestruction);
		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);

		// Remove all cues.
		AbilitySystemComponent->RemoveAllGameplayCues();

		// Reset the ASC's avatar, if it still has an owner.
		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		/* If the ASC doesn't have a valid owner, clear all of its actor info, not just its avatar. If the ASC doesn't
		 * have a valid actor owner, then its information regarding that owner is obsolete and should be reset. */
		else
		{
			AbilitySystemComponent->ClearActorInfo();
		}
	}

	// Clear the cached ASC.
	AbilitySystemComponent = nullptr;

	// Uninitialize the health component, if the ASC's owner has one.
	if (UHealthComponent* HealthComponent = UHealthComponent::FindHealthComponent(GetOwner()))
	{
		HealthComponent->UninitializeFromAbilitySystem();
	}

	// Uninitialize the combat component, if the ASC's owner has one.
	if (UCombatComponent* CombatComponent = UCombatComponent::FindCombatComponent(GetOwner()))
	{
		CombatComponent->UninitializeFromAbilitySystem();
	}

	// Uninitialize the movement component, if the ASC's owner has one.
	if (UHeroesCharacterMovementComponent* MovementComponent = UHeroesCharacterMovementComponent::FindHeroesCharacterMovementComponent(GetOwner()))
	{
		MovementComponent->UninitializeFromAbilitySystem();
	}
}
