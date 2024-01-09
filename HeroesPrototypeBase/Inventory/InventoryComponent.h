// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "InventoryList.h"
#include "NativeGameplayTags.h"
#include "Components/ActorComponent.h"
#include "ItemTraits/SlottedItemTrait.h"
#include "InventoryComponent.generated.h"

/** Native gameplay tags relevant to this class. */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_TemporarilyUnarmed);

class UInventoryItemDefinition;

/**
 * Describes the result of an attempted inventory action.
 */
UENUM(BlueprintType)
enum class EInventoryActionResult : uint8
{
	// The action succeeded.
	Success = 0,
	// The action failed because an item instance required to perform the action was not found in the specified inventory.
	Fail_ItemNotFound,
	// The action failed because the item does not have the necessary trait(s) to perform the action.
	Fail_MissingTrait,
	// The action failed for some unspecified reason.
	Fail
};

/**
 * 
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	// Construction.

public:

	/** Default constructor. */
	UInventoryComponent();



	// Initialization.

public:

	/** Initializes this component's properties. */
	virtual void InitializeComponent() override;



	// Networking.

public:

	/** Enables this component to replicate its inventory list as a sub-object. */
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;



	// Inventory management.

// Adding items.
public:

	/** Adds the given item instance to this inventory. If the item is slotted and its slot is occupied by another item,
	 * that other item will be replaced. The item will be dropped if it has the "droppable" trait; otherwise, it will
	 * be destroyed.
	 *
	 * @param ItemToAdd					The item to add to this inventory.
	 * @param bDestroyReplacedItem		(Optional) Don't try to drop the replaced item; always destroy it.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heroes|Inventory")
	void AddItemHard(UInventoryItemInstance* ItemToAdd, bool bDestroyReplacedItem = false);

	/** Adds the given item instance to this inventory if there is space. If the item is slotted and its slot is
	 * occupied by another item, the new item will not be added. Items already in this inventory can never be replaced
	 * with this function. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heroes|Inventory")
	EInventoryActionResult AddItemSoft(UInventoryItemInstance* ItemToAdd);

	/** Performs logic that needs to be executed when an item is added to this inventory, regardless of how it was
	 * added. */
	void AddItem_Internal(UInventoryItemInstance* ItemToAdd);

// Removing items.
public:

	/** Removes the given item instance from this inventory. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heroes|Inventroy")
	EInventoryActionResult RemoveItem(UInventoryItemInstance* ItemToRemove);

	/** Drops the given item instance if it has the "droppable" trait. If the item is successfully dropped, it is
	 * removed from this inventory. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heroes|Inventroy")
	EInventoryActionResult TryDropItem(UInventoryItemInstance* ItemToDrop, bool bThrowItem);

	/** Performs logic that needs to be executed when an item is removed from this inventory, regardless of how it was
	 * removed. */
	void RemoveItem_Internal(UInventoryItemInstance* ItemToRemove);



	// Equipment.

public:

	/** Equips the specified item instance if it is in this inventory and has the "equippable" trait. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heroes|Inventroy")
	EInventoryActionResult TryEquipItem(UInventoryItemInstance* ItemToEquip);

	/** Equips the first equippable item in this inventory that is not currently equipped. Slotted items are searched
	 * first. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heroes|Inventroy")
	bool TryEquipNextItem();

	/** Explicitly unequips the currently equipped item, if one exists, without equipping a new one. This cannot be
	 * manually called by players and should only be called as a result of gameplay events. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heroes|Inventory")
	EInventoryActionResult TryUnequipDirect();

private:

	/** Called when this component's owner's ASC is granted its first TemporarilyUnarmed tag or loses its last
	 * TemporarilyUnarmed tag. Temporarily unequips or automatically re-equips the current item, respectively. */
	UFUNCTION()
	void OnTempUnarmedStateChanged(const FGameplayTag Callback, int32 NewCount);

	/** Handle used to bind logic to when the TemporarilyUnarmed tag is added or removed from this component's owner's
	 * ASC. */
	FDelegateHandle TempUnarmedTagDelegate;



	// Slotted items.

// Utils.
private:

	/** Checks if the specified inventory slot is currently occupied by an item.
	 *
	 * @return	The item currently occupying the specified slot; nullptr if the slot is empty.
	 */
	UInventoryItemInstance* IsSlotOccupied(EInventorySlot SlotToCheck) const;

	/** Adds the given item to its appropriate slot in the array of slotted items. Will not add the given item if its
	 * corresponding slot is occupied by another item. */
	void AddItemToSlot(UInventoryItemInstance* NewItem);

	/** Removes the given item from its appropriate slot in the array of slotted items. Its slot will be made empty. */
	void RemoveItemFromSlot(UInventoryItemInstance* ItemToRemove);

// Slotted item data.
private:

	/** An array of slotted items in this inventory, assigned to their corresponding slots. Only one item of a
	 * designated slot can be in an inventory at any given time.
	 *
	 * Slotted items are assigned as follows:
	 *
	 * Index 0: Primary Weapon
	 * Index 1: Power Weapon
	 * Index 2: Item Pick-Up (e.g. "Relics")
	 * Index 3: Game-Mode Item (e.g. "EMP")
	 */
	UPROPERTY()
	TArray<TObjectPtr<UInventoryItemInstance>> SlottedItems;

	/** The total number of slots for slotted items. Used to initialize @SlottedItems. */
	static constexpr uint32 TotalSlots = 4;



	// Inventory data.

// Utils.
public:

	/** Returns an array of every item instance in this inventory.
	 *
	 * @param FilterByClass		(Optional) Filter for item instances with a specified item definition.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Heroes|Inventory")
	TArray<UInventoryItemInstance*> GetAllItems(TSubclassOf<UInventoryItemDefinition> FilterByClass = nullptr);

	/** Returns an array of every item instance in this inventory, sorted by priority. Slotted items are always
	 * prioritized first. Un-slotted items are prioritized by the order in which they were added to the inventory. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Heroes|Inventory")
	TArray<UInventoryItemInstance*> GetAllItemsOrdered();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Heroes|Inventory")
	UInventoryItemInstance* GetEquippedItem() const { return CurrentlyEquippedItem; }

	/** Returns whether the specified item instance is currently in this inventory. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Heroes|Inventory")
	bool IsItemInInventory(UInventoryItemInstance* ItemToCheck);

// Data.
private:

	/** A collection of item instance pointers that represent the contents of this inventory. */
	UPROPERTY(Replicated)
	FInventoryList Inventory;

	/** The item that is currently equipped, if there is one. */
	UPROPERTY(Replicated)
	TObjectPtr<UInventoryItemInstance> CurrentlyEquippedItem = nullptr;

	/** The item that is currently equipped but is temporarily unequipped due to the TemporarilyUnequipped state. When
	 * this component's owner loses that state, this item is automatically re-equipped. */
	UPROPERTY(Replicated)
	TObjectPtr<UInventoryItemInstance> TempUnequippedItem = nullptr;



	// Debugging.

public:

	/** I can't figure out how to use debug HUDs so I'm just using the tick component to display the inventory's
	 * contents for now. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
