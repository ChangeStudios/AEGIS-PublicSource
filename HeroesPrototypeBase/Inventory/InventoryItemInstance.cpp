// Copyright Samuel Reitich 2024.

#include "Inventory/InventoryItemInstance.h"
#include "InventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerStates/Game/HeroesGamePlayerStateBase.h"

UInventoryItemInstance::UInventoryItemInstance()
{
}

void UInventoryItemInstance::Init(TSubclassOf<UInventoryItemDefinition> InItemDefinition, AHeroesGamePlayerStateBase* InCurrentOwner)
{
	check(InItemDefinition);

	// Set this item's item definition to access static item data.
	ItemDefinitionClass = InItemDefinition;

	// Create a new instance of this item's definition object to hold runtime data for this item.
	ItemDefinition = NewObject<UInventoryItemDefinition>(GetWorld(), InItemDefinition);

	// Perform item initialization logic for each of this item's traits.
	for (UInventoryItemTraitBase* Trait : ItemDefinition->GetTraits())
	{
		if (Trait != nullptr)
		{
			Trait->OnItemInstanceCreated(this);

			// If this item was created in-place in an actor's inventory, perform inventory-entry logic for each trait.
			if (IsValid(InCurrentOwner))
			{
				Trait->OnItemEnteredInventory(this);
			}
		}
	}

	// Initialize this item's owner if it already has one.
	if (IsValid(InCurrentOwner))
	{
		CurrentOwner = InCurrentOwner;
	}
}

void UInventoryItemInstance::BeginDestroy()
{
	// Perform item destruction logic for each of this item's traits.
	if (ItemDefinition)
	{
		for (UInventoryItemTraitBase* Trait : ItemDefinition->GetTraits())
		{
			if (Trait)
			{
				Trait->OnItemInstanceDestroyed(this);
			}
		}
	}

	UObject::BeginDestroy();
}

const UInventoryItemDefinition* UInventoryItemInstance::GetItemDefinition() const
{
	return ItemDefinition;
}

void UInventoryItemInstance::SetCurrentOwner(AHeroesGamePlayerStateBase* InCurrentOwner)
{
	CurrentOwner = InCurrentOwner;
}

void UInventoryItemInstance::AddStatTags(FGameplayTag Tag, int32 Quantity)
{
	StatTags.AddTags(Tag, Quantity);
}

void UInventoryItemInstance::RemoveStatTags(FGameplayTag Tag, int32 Quantity)
{
	StatTags.RemoveTags(Tag, Quantity);
}

int32 UInventoryItemInstance::GetStatTagCount(FGameplayTag Tag) const
{
	return StatTags.GetTagCount(Tag);
}

bool UInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemDefinitionClass);
	DOREPLIFETIME(ThisClass, ItemDefinition);
	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, CurrentOwner);
}
