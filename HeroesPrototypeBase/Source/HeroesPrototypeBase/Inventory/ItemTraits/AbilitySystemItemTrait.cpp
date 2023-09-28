// Copyright Samuel Reitich 2023.


#include "Inventory/ItemTraits/AbilitySystemItemTrait.h"

#include "AbilitySystem/Abilities/HeroesAbilitySet.h"
#include "AbilitySystem/Components/HeroesAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "HeroesLogChannels.h"
#include "AbilitySystem/HeroesAbilitySystemGlobals.h"
#include "Inventory/InventoryItemInstance.h"
#include "Player/PlayerStates/Game/HeroesGamePlayerStateBase.h"

void UAbilitySystemItemTrait::OnItemEnteredInventory(UInventoryItemInstance* ItemInstance)
{
	Super::OnItemEnteredInventory(ItemInstance);

	UHeroesAbilitySystemComponent* HeroesASC = UHeroesAbilitySystemGlobals::GetHeroesAbilitySystemComponentFromActor(ItemInstance->GetCurrentOwner());
	if (HeroesASC)
	{
		// Grant each ability set specified by this trait and save a handle to it to remove it later.
		for (const UHeroesAbilitySet* Set : AbilitySetsToGrant)
		{
			if (Set)
			{
				Set->GiveToAbilitySystem(HeroesASC, &GrantedAbilitySetHandles.AddDefaulted_GetRef(), ItemInstance);
			}
		}
	}
}

void UAbilitySystemItemTrait::OnItemLeftInventory(UInventoryItemInstance* ItemInstance)
{
	Super::OnItemLeftInventory(ItemInstance);

	UHeroesAbilitySystemComponent* HeroesASC = UHeroesAbilitySystemGlobals::GetHeroesAbilitySystemComponentFromActor(ItemInstance->GetCurrentOwner());
	if (HeroesASC)
	{
		// Remove every granted ability set from this actor's owner.
		for (FHeroesAbilitySet_GrantedHandles& Handle : GrantedAbilitySetHandles)
		{
			Handle.RemoveFromAbilitySystem(HeroesASC);
		}

		// Empty the array of handles for granted ability sets.
		GrantedAbilitySetHandles.Empty();
	}
}
