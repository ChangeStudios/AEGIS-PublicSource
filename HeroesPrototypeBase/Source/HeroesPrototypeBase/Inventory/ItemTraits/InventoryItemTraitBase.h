// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemTraitBase.generated.h"

class UInventoryItemInstance;

/**
 * The base class for all item traits. Item traits are used by inventory item definitions to define the properties of
 * an item and hold runtime data specific to that item. Each item trait defines and holds data for a unique property,
 * such as whether or not an item can be equipped.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class HEROESPROTOTYPEBASE_API UInventoryItemTraitBase : public UObject
{
	GENERATED_BODY()

public:

	/** Called upon the creation of an item instance whose item definition contains this trait. */
	virtual void OnItemInstanceCreated(UInventoryItemInstance* ItemInstance) {};

	/** Called immediately before this trait's item instance is destroyed. */
	virtual void OnItemInstanceDestroyed(UInventoryItemInstance* ItemInstance) {};

	/** Called immediately after this item is placed into a new actor's inventory. */
	virtual void OnItemEnteredInventory(UInventoryItemInstance* ItemInstance) {};

	/** Called immediately before this item is removed from its current inventory. This happens when an item is
	 * directly removed by the system or when it is dropped by the owning actor. */
	virtual void OnItemLeftInventory(UInventoryItemInstance* ItemInstance) {};

};
