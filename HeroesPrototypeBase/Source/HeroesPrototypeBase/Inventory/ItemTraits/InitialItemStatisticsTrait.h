// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inventory/ItemTraits/InventoryItemTraitBase.h"
#include "InitialItemStatisticsTrait.generated.h"

/**
 * Defines a collection of statistic gameplay tags that this item starts with. These tags, in their specified
 * quantities, are granted to this item when it is initialized. This defines the starting values of data that will be
 * tracked with gameplay tags, like a weapon's maximum magazine size, starting reserve ammo, etc.
 */
UCLASS(DisplayName = "Initial Statistics")
class HEROESPROTOTYPEBASE_API UInitialItemStatisticsTrait : public UInventoryItemTraitBase
{
	GENERATED_BODY()

public:

	/** Adds this item's initial stat tags to its stat tag collection. */
	virtual void OnItemInstanceCreated(UInventoryItemInstance* ItemInstance) override;



	// Static data.

public:

	/** The stat tags that this items starts with and their initial quantity. */
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, int32> InitialItemStatistics;
	
};
