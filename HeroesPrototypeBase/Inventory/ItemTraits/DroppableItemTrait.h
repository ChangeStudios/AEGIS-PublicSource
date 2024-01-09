// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemTraits/InventoryItemTraitBase.h"
#include "DroppableItemTrait.generated.h"

class AInventoryItemPickupActor;

/**
 * Allows this item to be "dropped" by players. When an item is dropped, it is removed from its current inventory and
 * a proxy actor is spawned and dropped onto the ground. The proxy actor can be picked up by any player to add the
 * item to their inventory.
 */
UCLASS(DisplayName = "Droppable")
class HEROESPROTOTYPEBASE_API UDroppableItemTrait : public UInventoryItemTraitBase
{
	GENERATED_BODY()

	// Static data.

public:

	/** The item pick-up actor to spawn when this item is dropped. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AInventoryItemPickupActor> ActorToDrop = nullptr;

	/** The velocity with which to throw this item's dropped actor when spawned. Items are thrown when they are
	 * manually dropped by players, but not when they are dropped by a player after dying or after swapping an item. */
	UPROPERTY(EditDefaultsOnly, DisplayName = "Force With Which to Throw Item")
	float ThrowingForce; 
};
