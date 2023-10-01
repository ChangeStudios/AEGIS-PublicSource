// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemTraits/InventoryItemTraitBase.h"
#include "SlottedItemTrait.generated.h"

/**
 * Defines the inventory slot into which a slotted item is placed.
 */
UENUM(BlueprintType)
enum class EInventorySlot : uint8
{
	// "Primary weapon" slot.
	PrimaryWeapon = 0,
	// "Power weapon" slot.
	PowerWeapon = 1,
	// "Item pick-up" (e.g. relics) slot.
	PickUp = 2,
	// "Game-mode item" (e.g. the "EMP") slot.
	GameMode = 3
};

/**
 * Assigns this item to a designated inventory slot. Each inventory slot can only have one item in it at a time. If a
 * "slotted" item is picked up, it will replace any item that is already in its corresponding slot. See @EInventorySlot.
 */
UCLASS(DisplayName = "Slotted")
class HEROESPROTOTYPEBASE_API USlottedItemTrait : public UInventoryItemTraitBase
{
	GENERATED_BODY()

	// Static data.

public:

	/** The inventory slot into which this item is placed. */
	UPROPERTY(EditDefaultsOnly)
	EInventorySlot Slot = EInventorySlot::PrimaryWeapon;
};
