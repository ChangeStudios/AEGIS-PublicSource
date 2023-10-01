// Copyright Samuel Reitich 2023.


#include "Inventory/InventoryList.h"
#include "InventoryItemInstance.h"

TArray<const FInventoryListEntry*> FInventoryList::GetEntries() const
{
	TArray<const FInventoryListEntry*> ReturnArray;

	// Create an of constant pointers to each valid item instance.
	for (FInventoryListEntry Entry : Entries)
	{
		if (IsValid(Entry.Item.Get()))
		{
			ReturnArray.Add(&Entry);
		}
	}

	return ReturnArray;
}

void FInventoryList::AddEntry(UInventoryItemInstance* InItemInstance)
{
	check(IsValid(InItemInstance));

	// Construct a new entry in the inventory list and set its corresponding item instance.
	FInventoryListEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = InItemInstance;

	MarkItemDirty(NewEntry);
}

bool FInventoryList::RemoveEntry(UInventoryItemInstance* InItemInstance)
{
	// Search for an entry with the specified item instance.
	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		FInventoryListEntry& Entry = *It;
		if (Entry.Item == InItemInstance)
		{
			// Remove the entry with the specified item instance if one was found.
			It.RemoveCurrent();
			MarkArrayDirty();

			return true;
		}
	}

	// Return false if the specified item instance is not in this inventory.
	return false;
}
