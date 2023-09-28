// Copyright Samuel Reitich 2023.


#include "Inventory/InventoryItemDefinition.h"

TArray<UInventoryItemTraitBase*> UInventoryItemDefinition::GetTraits() const
{
	TArray<UInventoryItemTraitBase*> OutTraits;

	for (UInventoryItemTraitBase* Trait : Traits)
	{
		if (Trait)
		{
			OutTraits.Add(Trait);
		}
	}

	return OutTraits;
}

UInventoryItemTraitBase* UInventoryItemDefinition::B_FindTraitByClass(TSubclassOf<UInventoryItemTraitBase> TraitToFind)
{
	for (UInventoryItemTraitBase* Trait : Traits)
	{
		if (Trait->IsA(TraitToFind))
		{
			return Trait;
		}
	}

	return nullptr;
}
