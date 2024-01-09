// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HeroesAbilitySet.h"
#include "Inventory/ItemTraits/InventoryItemTraitBase.h"
#include "AbilitySystemItemTrait.generated.h"

class UInventoryItemInstance;

/**
 * Provides this item's owner with a collection of ability sets while the item is in their inventory. These ability
 * sets are removed when this item is removed from the inventory.
 */
UCLASS(DisplayName = "Grants Ability Sets")
class HEROESPROTOTYPEBASE_API UAbilitySystemItemTrait : public UInventoryItemTraitBase
{
	GENERATED_BODY()

public:

	/** Grants this item's ability sets to the owner of the specified item instance. */
	virtual void OnItemEnteredInventory(UInventoryItemInstance* ItemInstance) override;

	/** Removes this item's ability sets from the owner of the specified item instance. */
	virtual void OnItemLeftInventory(UInventoryItemInstance* ItemInstance) override;



	// Static data.

protected:

	/** The ability sets to grant this item's owner while it is in their inventory. */
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UHeroesAbilitySet>> AbilitySetsToGrant;



	// Runtime data.

protected:

	/** The handles for the ability sets currently applied by this item. Used to remove the ability sets when this item
	 * is remove from its current inventory. */
	TArray<FHeroesAbilitySet_GrantedHandles> GrantedAbilitySetHandles;
};
