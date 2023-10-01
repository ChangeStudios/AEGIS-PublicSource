// Copyright Samuel Reitich 2023.


#include "Inventory/ItemTraits/InitialItemStatisticsTrait.h"

#include "Inventory/InventoryItemInstance.h"

void UInitialItemStatisticsTrait::OnItemInstanceCreated(UInventoryItemInstance* ItemInstance)
{
	Super::OnItemInstanceCreated(ItemInstance);

	if (ItemInstance)
	{
		// Add the specified quantity of each of this item's initial stat tags.
		for (const TTuple<FGameplayTag, int32> TagQuantityPair : InitialItemStatistics)
		{
			ItemInstance->AddStatTags(TagQuantityPair.Key, TagQuantityPair.Value);
		}
	}
}
