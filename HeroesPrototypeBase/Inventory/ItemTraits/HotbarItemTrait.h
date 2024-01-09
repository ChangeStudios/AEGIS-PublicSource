// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemTraits/InventoryItemTraitBase.h"
#include "HotbarItemTrait.generated.h"

/**
 * Makes this item visible in the hotbar in the user-interface. Items do not have to be slotted or equippable to appear
 * in the hotbar, although they usually are.
 */
UCLASS(DisplayName = "Appears in Hotbar")
class HEROESPROTOTYPEBASE_API UHotbarItemTrait : public UInventoryItemTraitBase
{
	GENERATED_BODY()

	// Static data.

public:

	/** The icon that represents this item in the hotbar. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> HotbarIcon;
};
