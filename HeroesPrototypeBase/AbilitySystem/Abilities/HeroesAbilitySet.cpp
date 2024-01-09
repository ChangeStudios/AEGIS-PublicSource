// Copyright Samuel Reitich 2024.


#include "AbilitySystem/Abilities/HeroesAbilitySet.h"

#include "AbilitySystem/AttributeSets/HeroesAttributeSetBase.h"
#include "AbilitySystem/Components/HeroesAbilitySystemComponent.h"
#include "HeroesLogChannels.h"

void FHeroesAbilitySet_GrantedHandles::AddGameplayAbilitySpecHandle(const FGameplayAbilitySpecHandle& HandleToAdd)
{
	// Store the handle if it's valid.
	if (HandleToAdd.IsValid())
	{
		GameplayAbilitySpecHandles.Add(HandleToAdd);
	}
}

void FHeroesAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& HandleToAdd)
{
	// Store the effect if it's valid.
	if (HandleToAdd.IsValid())
	{
		GameplayEffectHandles.Add(HandleToAdd);
	}
}

void FHeroesAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* SetToAdd)
{
	// Store the attribute set if it's not null.
	if (SetToAdd != nullptr)
	{
		GrantedAttributeSets.Add(SetToAdd);
	}
}

void FHeroesAbilitySet_GrantedHandles::RemoveFromAbilitySystem(UHeroesAbilitySystemComponent* AbilitySystemToRemoveFrom)
{
	check(AbilitySystemToRemoveFrom)

	// Authority is needed to give or remove ability sets.
	if (!AbilitySystemToRemoveFrom->IsOwnerActorAuthoritative())
	{
		return;
	}

	// Remove every ability granted by the ability set.
	for (const FGameplayAbilitySpecHandle& Handle : GameplayAbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySystemToRemoveFrom->ClearAbility(Handle);
		}
	}

	// Remove every effect granted by the ability set.
	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySystemToRemoveFrom->RemoveActiveGameplayEffect(Handle);
		}
	}

	// Remove every attribute set granted by the ability set.
	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		if (Set != nullptr)
		{
			AbilitySystemToRemoveFrom->RemoveSpawnedAttribute(Set);
		}
	}

	// Clear the handle collections.
	GameplayAbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UHeroesAbilitySet::UHeroesAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHeroesAbilitySet::GiveToAbilitySystem(UHeroesAbilitySystemComponent* AbilitySystemToGiveTo, FHeroesAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(AbilitySystemToGiveTo);

	// Authority is needed to give or remove ability sets.
	if (!AbilitySystemToGiveTo->IsOwnerActorAuthoritative())
	{
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); AbilityIndex++)
	{
		const FHeroesAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		// Ensure that the ability is valid.
		if (!IsValid(AbilityToGrant.GameplayAbility))
		{
			UE_LOG(LogHeroesAbilitySystem, Error, TEXT("GrantedGameplayAbilities[%i] in ability set \"[%s]\" is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		// Create a new spec for the granted gameplay ability.
		UHeroesGameplayAbilityBase* AbilityCDO = AbilityToGrant.GameplayAbility->GetDefaultObject<UHeroesGameplayAbilityBase>();
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityCDO);
		AbilitySpec.SourceObject = SourceObject;

		/* This line is very important. Adding the assigned input tag to the ability's collection of dynamic ability
		 * tags is what allows an ASC to identify and activate it using an input tag, since abilities do not have
		 * hard-coded input tags for TryActivateAbilitiesByTag to use. */
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		// Grant the ability to the specified ASC.
		const FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemToGiveTo->GiveAbility(AbilitySpec);

		// Provide a handle to the granted ability. This can be used to remove it later.
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); EffectIndex++)
	{
		const FHeroesAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		// Ensure that the effect is valid.
		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogHeroesAbilitySystem, Error, TEXT("GrantedGameplayEffects[%i] in ability set \"[%s]\" is not valid."), EffectIndex, *GetNameSafe(this));
			continue;
		}

		// Get the gameplay effect to grant.
		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		// Grant the gameplay effect to the specified ASC.
		const FActiveGameplayEffectHandle GameplayEffectHandle = AbilitySystemToGiveTo->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, AbilitySystemToGiveTo->MakeEffectContext());

		// Provide a handle to the granted effect. This can be used to remove it later.
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// Grant the attribute sets.
	for (int32 AttributeSetIndex = 0; AttributeSetIndex < GrantedAttributeSets.Num(); AttributeSetIndex++)
	{
		const FHeroesAbilitySet_AttributeSet& AttributeSetToGrant = GrantedAttributeSets[AttributeSetIndex];

		// Ensure that the attribute set is valid.
		if (!IsValid(AttributeSetToGrant.AttributeSet))
		{
			UE_LOG(LogHeroesAbilitySystem, Error, TEXT("GrantedAttributeSets[%i] in ability set \"[%s]\" is not valid."), AttributeSetIndex, *GetNameSafe(this));
			continue;
		}

		// Create a new attribute set object to grant.
		UAttributeSet* NewAttributeSet = NewObject<UAttributeSet>(AbilitySystemToGiveTo->GetOwner(), AttributeSetToGrant.AttributeSet);
		// Grant the attribute set to the specified ASC.
		AbilitySystemToGiveTo->AddAttributeSetSubobject(NewAttributeSet);

		// Provide a pointer to the granted attribute set. This can be used to remove it later.
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewAttributeSet);
		}
	}
}
