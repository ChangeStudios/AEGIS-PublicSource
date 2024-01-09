// Copyright Samuel Reitich 2024.

#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryList.generated.h"

class UInventoryComponent;
class UInventoryItemInstance;
struct FInventoryList;

/**
 * A single entry in an inventory list. This represents one item instance in an inventory.
 */
USTRUCT(BlueprintType)
struct FInventoryListEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:

	/** Default constructor. */
	FInventoryListEntry()
	{}

private:

	friend FInventoryList;
	friend UInventoryComponent;

	/** The item instance represented by this entry. */
	UPROPERTY()
	TObjectPtr<UInventoryItemInstance> Item = nullptr;
};

/**
 * A list that tracks the items in an actor's inventory. This structure is the "inventory" itself. This contains a
 * collection of inventory list entries, each of which represents a single item instance.
 */
USTRUCT(BlueprintType)
struct FInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	// Construction.

public:

	/** Default constructor. */
	FInventoryList()
	{}



	// Fast array serializer implementation.

public:

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryListEntry, FInventoryList>(Entries, DeltaParams, *this);
	}



	// Inventory management.

public:

	/** Getter for this inventory list's entries. Returns constant pointers because entries should never be changed
	 * externally. */
	TArray<const FInventoryListEntry*> GetEntries() const;

	/** Creates and adds a new inventory list entry to represent the given item. */
	void AddEntry(UInventoryItemInstance* InItemInstance);

	/** Removes the entry of the specified item instance from the inventory list.
	 *
	 * @return	If the inventory list has an entry for the specified item instance.
	 */
	bool RemoveEntry(UInventoryItemInstance* InItemInstance);



	// Inventory data.

private:

	friend UInventoryComponent;

	/** The list of entries that represents the actual contents of the inventory. This array is automatically
	 * replicated by the fast array serializer. */
	UPROPERTY()
	TArray<FInventoryListEntry> Entries;
};

/**
 * Fast array serializer implementation.
 */
template<>
struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};