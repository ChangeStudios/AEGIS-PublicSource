// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "ItemTraits/InventoryItemTraitBase.h"
#include "InventoryItemDefinition.generated.h"

/**
 * Defines an item's behavior when its owner's avatar "dies" or is destroyed. By default, items remain in their owner's
 * inventory, even after death.
 */
UENUM(BlueprintType)
enum class EItemDeathBehavior : uint8
{
	// This item remains in its current inventory.
	None = 0,
	// This item is removed from its current inventory.
	Removed,
	// This item is dropped. This requires the "Droppable" trait.
	Dropped
};

/**
 * A collection of data that defines an inventory item. Items are defined by a set of universal data and a collection
 * of "traits." Each trait defines and holds runtime data for a specific property of the item; items only contain traits
 * that are relevant to them. For example, only items that can be equipped by the player have the "equippable" trait.
 */
UCLASS(Abstract, Blueprintable)
class HEROESPROTOTYPEBASE_API UInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

	// Construction.

public:

	/** Default constructor. */
	UInventoryItemDefinition() {};



	// Universal item data.

public:

	/** The name of this item. This is used for pick-up prompts, in the hot-bar, and in other player-facing experiences. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Universal Item Data")
	FText DisplayName;

	/** How this item reacts to its owner's avatar's death or destruction. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Universal Item Data")
	EItemDeathBehavior DeathBehavior;



	// Traits.

public:

	/** Returns an array of this item's traits. */
	TArray<UInventoryItemTraitBase*> GetTraits() const;

	/** Returns this item definition's trait of the specified class. Returns nullptr if this item definition does not
	 * have the specified trait. */
	template <class T>
	T* FindTraitByClass() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, UInventoryItemTraitBase>::Value, "'T' template parameter to FindTraitByClass must be derived from UInventoryItemTraitBase.");
		for (UInventoryItemTraitBase* Trait : Traits)
		{
			if (Cast<T>(Trait))
			{
				return Cast<T>(Trait);
			}
		}

		return nullptr;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Heroes|Inventory", DisplayName = "Find Trait by Class")
	UInventoryItemTraitBase* B_FindTraitByClass(TSubclassOf<UInventoryItemTraitBase> TraitToFind);

protected:

	/** This item definition's traits, which provide static data and hold relevant runtime data. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UInventoryItemTraitBase>> Traits;
};