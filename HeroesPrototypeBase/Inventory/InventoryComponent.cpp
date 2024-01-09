// Copyright Samuel Reitich 2024.


#include "Inventory/InventoryComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "HeroesLogChannels.h"
#include "InventoryItemDefinition.h"
#include "InventoryItemInstance.h"
#include "InventoryItemPickupActor.h"
#include "Animation/AnimInstances/Characters/HeroFirstPersonAnimInstance.h"
#include "Animation/AnimInstances/Characters/PrototypeAnimInstanceV3.h"
#include "Camera/CameraComponent.h"
#include "Characters/Components/FirstPersonSkeletalMeshComponent.h"
#include "Characters/Heroes/HeroBase.h"
#include "Components/CapsuleComponent.h"
#include "Engine/ActorChannel.h"
#include "ItemTraits/DroppableItemTrait.h"
#include "ItemTraits/EquippableItemTrait.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerStates/Game/HeroesGamePlayerStateBase.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_State_TemporarilyUnarmed, "State.TemporarilyUnarmed", "The player temporarily has no item equipped. When this tag is removed, the item that was previously equipped will be automatically re-equipped. Used for action-blocking abilities.");

static TAutoConsoleVariable<int32> CVarShowInventory
(
	TEXT("ShowDebugInventory"),
	0,
	TEXT("Draws inventory system debug info.")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat
);

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}
	
void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Initialize the array of slotted items.
	SlottedItems = TArray<UInventoryItemInstance*>();
	SlottedItems.SetNum(TotalSlots);

	// Bind a delegate to when the TemporarilyUnarmed state if given to or removed from this component's owner's ASC.
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		TempUnarmedTagDelegate = ASC->RegisterGameplayTagEvent(TAG_State_TemporarilyUnarmed, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UInventoryComponent::OnTempUnarmedStateChanged);
	}
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	// Set up proper replication of this inventory's items.
	for (FInventoryListEntry& Entry : Inventory.Entries)
	{
		UInventoryItemInstance* Item = Entry.Item;

		if (IsValid(Item))
		{
			bWroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void UInventoryComponent::AddItemHard(UInventoryItemInstance* ItemToAdd, bool bDestroyReplacedItem)
{
	// Only the server can add and remove items from a player's inventory.
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	check(ItemToAdd);

	// Check if the new item is slotted.
	if (const USlottedItemTrait* SlotTrait = ItemToAdd->GetItemDefinition()->FindTraitByClass<USlottedItemTrait>())
	{
		// If the item is slotted, check if its slot is already occupied by another item. If it is, that item will be replaced.
		if (UInventoryItemInstance* ReplacedItem = IsSlotOccupied(SlotTrait->Slot))
		{
			EInventoryActionResult DropResult = EInventoryActionResult::Fail;

			// Try to drop the replaced item, unless it was explicitly requested that the replaced item be destroyed.
			if (!bDestroyReplacedItem)
			{
				DropResult = TryDropItem(ReplacedItem, false);
			}

			// If the replaced item could not be dropped, destroy it.
			if (DropResult != EInventoryActionResult::Success)
			{
				RemoveItem(ReplacedItem);
			}
		}
	}

	// Add the new item to this inventory.
	AddItem_Internal(ItemToAdd);
}

EInventoryActionResult UInventoryComponent::AddItemSoft(UInventoryItemInstance* ItemToAdd)
{
	// Only the server can add and remove items from a player's inventory.
	if (!GetOwner()->HasAuthority())
	{
		return EInventoryActionResult::Fail;
	}

	check(ItemToAdd);

	// Check if the new item is slotted.
	if (const USlottedItemTrait* SlotTrait = ItemToAdd->GetItemDefinition()->FindTraitByClass<USlottedItemTrait>())
	{
		// If the item is slotted, check if its slot is already occupied by another item. If it is, then the new item will not be added.
		if (UInventoryItemInstance* OccupyingItem = IsSlotOccupied(SlotTrait->Slot))
		{
			return EInventoryActionResult::Fail;
		}
	}

	// If the item is not slotted or its slot is empty, add it to this inventory.
	AddItem_Internal(ItemToAdd);

	return EInventoryActionResult::Success;
}

void UInventoryComponent::AddItem_Internal(UInventoryItemInstance* ItemToAdd)
{
	// Only the server can add and remove items from a player's inventory.
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Add the given item to this inventory.
	Inventory.AddEntry(ItemToAdd);

	// Add the given item to its assigned slot if it's slotted.
	AddItemToSlot(ItemToAdd);

	// Update the new item's owner.
	if (AHeroesGamePlayerStateBase* OwningPlayerState = GetOwner<AHeroesGamePlayerStateBase>())
	{
		ItemToAdd->SetCurrentOwner(OwningPlayerState);
	}

	// Initialize each of the new item's traits with the new inventory.
	for (UInventoryItemTraitBase* Trait : ItemToAdd->GetItemDefinition()->GetTraits())
	{
		Trait->OnItemEnteredInventory(ItemToAdd);
	}

	// If this is the first and only item in this inventory, try to auto-equip it.
	if (Inventory.Entries.Num() == 1)
	{
		TryEquipItem(ItemToAdd);
	}

	// Update the UI.
}

EInventoryActionResult UInventoryComponent::RemoveItem(UInventoryItemInstance* ItemToRemove)
{
	// Only the server can add and remove items from a player's inventory.
	if (!GetOwner()->HasAuthority())
	{
		return EInventoryActionResult::Fail;
	}

	check(ItemToRemove);

	// Make sure that the specified item is in this inventory.
	if (!IsItemInInventory(ItemToRemove))
	{
		return EInventoryActionResult::Fail_ItemNotFound;
	}

	// Remove the given item from this inventory.
	RemoveItem_Internal(ItemToRemove);

	return EInventoryActionResult::Success;
}

EInventoryActionResult UInventoryComponent::TryDropItem(UInventoryItemInstance* ItemToDrop, bool bThrowItem)
{
	// Only the server can drop items.
	if (!GetOwner()->HasAuthority())
	{
		return EInventoryActionResult::Fail;
	}

	// Make sure that the specified item is in this inventory.
	if (!ItemToDrop || !IsItemInInventory(ItemToDrop))
	{
		return EInventoryActionResult::Fail_ItemNotFound;
	}

	// Drop the given item if it has the "droppable" trait.
	if (UDroppableItemTrait* DroppableItemTrait = ItemToDrop->GetItemDefinition()->FindTraitByClass<UDroppableItemTrait>())
	{
		// Get the player that is dropping this item to determine where to spawn the dropped item actor.
		const APlayerState* PS = GetOwner<APlayerState>();
		AHeroBase* Hero = PS ? PS->GetPawn<AHeroBase>() : nullptr;

		if (Hero)
		{
			// Use the player's first-person camera to spawn the item where they're looking.
			const UCameraComponent* HeroCamera = Hero->GetFirstPersonCameraComponent();
			const FVector ForwardVector = HeroCamera->GetForwardVector();
			const FVector SpawnLocation = HeroCamera->GetComponentLocation() + (ForwardVector * Hero->GetCapsuleComponent()->GetScaledCapsuleRadius());
			const FRotator SpawnRotation = HeroCamera->GetComponentRotation();
			const FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation);

			// Spawn and initialize a new item actor to represent the dropped item.
			AInventoryItemPickupActor* ItemActor = GetWorld()->SpawnActorDeferred<AInventoryItemPickupActor>(DroppableItemTrait->ActorToDrop, SpawnTransform, nullptr, Hero, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			ItemActor->Init(ItemToDrop);
			ItemActor->FinishSpawning(SpawnTransform);

			// Match the item's initial velocity to the velocity of the player that is dropping it.
			ItemActor->GetRootMesh()->SetAllPhysicsLinearVelocity(Hero->GetVelocity());

			/* Throw the item forward if desired. */
			if (bThrowItem)
			{
				const FVector Impulse = ForwardVector * DroppableItemTrait->ThrowingForce;
				ItemActor->GetRootMesh()->AddImpulse(Impulse);
			}

			// If the item is successfully dropped, remove it from this inventory.
			RemoveItem_Internal(ItemToDrop);

			return EInventoryActionResult::Success;
		}
	}
	else
	{
		return EInventoryActionResult::Fail_MissingTrait;
	}

	return EInventoryActionResult::Fail;
}

void UInventoryComponent::RemoveItem_Internal(UInventoryItemInstance* ItemToRemove)
{
	// Only the server can add and remove items from a player's inventory.
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// If the item that was removed was currently equipped, unequip it and try to equip another item.
	if (ItemToRemove == CurrentlyEquippedItem)
	{
		if (UEquippableItemTrait* EquippableItemTrait = CurrentlyEquippedItem->GetItemDefinition()->FindTraitByClass<UEquippableItemTrait>())
		{
			EquippableItemTrait->OnUnequipped(CurrentlyEquippedItem);

			// If another item could not be equipped, revert to the "unarmed" character animation data.
			if (!TryEquipNextItem())
			{
				CurrentlyEquippedItem = nullptr;

				const APlayerState* PS = GetOwner<APlayerState>();
				const AHeroBase* Hero = PS->GetPawn<AHeroBase>();

				// TODO: Replace this when creating the final class.
				if (UPrototypeAnimInstanceV3* FPPAnimInstance = Cast<UPrototypeAnimInstanceV3>(Hero->GetFirstPersonMesh()->GetAnimInstance()))
				{
					FPPAnimInstance->UpdateCharacterAnimationData(FPPAnimInstance->DefaultCharacterAnimationData);
				}
			}
		}
	}

	// Add the given item to this inventory.
	Inventory.RemoveEntry(ItemToRemove);

	// Remove the given item from its assigned slot if it's slotted.
	RemoveItemFromSlot(ItemToRemove);

	// Clear the item's owner.
	if (AHeroesGamePlayerStateBase* OwningPlayerState = GetOwner<AHeroesGamePlayerStateBase>())
	{
		ItemToRemove->SetCurrentOwner(nullptr);
	}

	// Uninitialize each of the removed item's traits from this inventory.
	for (UInventoryItemTraitBase* Trait : ItemToRemove->GetItemDefinition()->GetTraits())
	{
		Trait->OnItemLeftInventory(ItemToRemove);
	}

	// Update the UI.
}

EInventoryActionResult UInventoryComponent::TryEquipItem(UInventoryItemInstance* ItemToEquip)
{
	// Only the server can equip items from a player's inventory.
	if (!GetOwner()->HasAuthority())
	{
		return EInventoryActionResult::Fail;
	}

	// Don't equip the specified item if it's already equipped or is null.
	if (CurrentlyEquippedItem && CurrentlyEquippedItem == ItemToEquip)
	{
		return EInventoryActionResult::Fail;
	}

	// Make sure that the specified item is valid and in this inventory.
	if (ItemToEquip != nullptr && IsItemInInventory(ItemToEquip))
	{
		// Equip the new item if it has the "equippable" trait.
		if (UEquippableItemTrait* EquippableItemTrait = ItemToEquip->GetItemDefinition()->FindTraitByClass<UEquippableItemTrait>())
		{
			// Unequip the current item.
			if (CurrentlyEquippedItem)
			{
				if (UEquippableItemTrait* OldEquippableItemTrait = CurrentlyEquippedItem->GetItemDefinition()->FindTraitByClass<UEquippableItemTrait>())
				{
					OldEquippableItemTrait->OnUnequipped(CurrentlyEquippedItem);
				}
			}

			// Equip the new item.
			CurrentlyEquippedItem = ItemToEquip;
			EquippableItemTrait->OnEquipped(ItemToEquip);

			return EInventoryActionResult::Success;
		}

		return EInventoryActionResult::Fail_MissingTrait;
	}

	return EInventoryActionResult::Fail_ItemNotFound;
}

bool UInventoryComponent::TryEquipNextItem()
{
	// Only the server can equip items from a player's inventory.
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	// Try to equip every item that this player has, excluding their currently equipped one, until one is successfully equipped.
	for (UInventoryItemInstance* Item : GetAllItemsOrdered())
	{
		if (Item != CurrentlyEquippedItem)
		{
			EInventoryActionResult Result = TryEquipItem(Item);

			if (Result == EInventoryActionResult::Success)
			{
				return true;
			}
		}
	}

	// Return false if no item could be successfully equipped.
	return false;
}

EInventoryActionResult UInventoryComponent::TryUnequipDirect()
{
	if (!CurrentlyEquippedItem)
	{
		return EInventoryActionResult::Fail_ItemNotFound;
	}

	// Unequip the currently equipped item.
	if (UEquippableItemTrait* EquippableItemTrait = CurrentlyEquippedItem->GetItemDefinition()->FindTraitByClass<UEquippableItemTrait>())
	{
		EquippableItemTrait->OnUnequipped(CurrentlyEquippedItem);
		CurrentlyEquippedItem = nullptr;
	}

	return EInventoryActionResult::Success;
}

void UInventoryComponent::OnTempUnarmedStateChanged(const FGameplayTag Callback, int32 NewCount)
{
	// Only the server should handle this event.
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Temporarily unequips the current item if this component's owner has entered the TemporarilyUnequipped state.
	if (NewCount > 0)
	{
		if (CurrentlyEquippedItem)
		{
			// Cache the item that will be re-equipped when the state is removed.
			TempUnequippedItem = CurrentlyEquippedItem;
			TryUnequipDirect();
		}
		else
		{
			TempUnequippedItem = nullptr;
		}
	}
	// Automatically re-equip the current item if this component's owner has exited the TemporarilyUnequipped state.
	else
	{
		if (TempUnequippedItem && !CurrentlyEquippedItem)
		{
			if (TryEquipItem(TempUnequippedItem) == EInventoryActionResult::Fail_ItemNotFound)
			{
				// If the player no longer has the item that they unequipped, try to equip the next item in their inventory.
				TryEquipNextItem();
			}

			TempUnequippedItem = nullptr;
		}
	}
}

TArray<UInventoryItemInstance*> UInventoryComponent::GetAllItems(TSubclassOf<UInventoryItemDefinition> FilterByClass)
{
	// Convert the inventory list entries into an array of pointers to each of their item instances.
	TArray<UInventoryItemInstance*> OutItems;
	for (FInventoryListEntry& Entry : Inventory.Entries)
	{
		if (Entry.Item)
		{
			// If we aren't filtering by an item definition, return every item in the inventory.
			if (FilterByClass == nullptr)
			{
				OutItems.Add(Entry.Item);
			}
			// If we are filtering by an item definition, only return items with the specified item definition class.
			else if (Entry.Item->GetItemDefinition()->IsA(FilterByClass))
			{
				OutItems.Add(Entry.Item);
			}
		}
	}

	return OutItems;
}

TArray<UInventoryItemInstance*> UInventoryComponent::GetAllItemsOrdered()
{
	// Create an array into which items will be sorted.
	TArray<UInventoryItemInstance*> OutItems;

	// Add all slotted items first.
	for (UInventoryItemInstance* Item : SlottedItems)
	{
		if (Item)
		{
			OutItems.Add(Item);
		}
	}

	// Add any non-slotted items in the order they were added to this array.
	for (FInventoryListEntry& Entry : Inventory.Entries)
	{
		if (Entry.Item)
		{
			OutItems.AddUnique(Entry.Item);
		}
	}

	return OutItems;
}

bool UInventoryComponent::IsItemInInventory(UInventoryItemInstance* ItemToCheck)
{
	// Check the item of every entry in the inventory list for a matching item instance.
	for (FInventoryListEntry& Entry : Inventory.Entries)
	{
		if (Entry.Item && Entry.Item == ItemToCheck)
		{
			return true;
		}
	}

	return false;
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const bool bShowDebug = CVarShowInventory.GetValueOnGameThread() != 0;
	if (bShowDebug)
	{
		for (FInventoryListEntry& Entry : Inventory.Entries)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::Printf(TEXT("Item: [%s]"), *Entry.Item->GetName()));
		}

		if (Inventory.Entries.Num() == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("Inventory is empty.")));
		}
	}
}

UInventoryItemInstance* UInventoryComponent::IsSlotOccupied(EInventorySlot SlotToCheck) const
{
	// Slots' enum values correspond to their slot array indices.
	const uint8 SlotIndex = static_cast<uint8>(SlotToCheck);

	if (SlottedItems[SlotIndex])
	{
		return SlottedItems[SlotIndex];
	}

	return nullptr;
}

void UInventoryComponent::AddItemToSlot(UInventoryItemInstance* NewItem)
{
	check(NewItem);

	if (const USlottedItemTrait* SlotTrait = NewItem->GetItemDefinition()->FindTraitByClass<USlottedItemTrait>())
	{
		EInventorySlot ItemSlot = SlotTrait->Slot;
		const uint8 SlotIndex = static_cast<uint8>(ItemSlot);

		// Only place this item its corresponding slot if that slot is empty.
		if (IsSlotOccupied(ItemSlot))
		{
			SlottedItems[SlotIndex] = NewItem;
		}
	}
}

void UInventoryComponent::RemoveItemFromSlot(UInventoryItemInstance* ItemToRemove)
{
	check(ItemToRemove);

	// Remove the given item by clearing its corresponding slot.
	if (const USlottedItemTrait* SlotTrait = ItemToRemove->GetItemDefinition()->FindTraitByClass<USlottedItemTrait>())
	{
		EInventorySlot ItemSlot = SlotTrait->Slot;
		const uint8 SlotIndex = static_cast<uint8>(ItemSlot);

		// Make sure that the specified item is in its corresponding slot.
		if (SlottedItems[SlotIndex] == ItemToRemove)
		{
			SlottedItems[SlotIndex] = nullptr;
		}
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Inventory);
	DOREPLIFETIME(UInventoryComponent, CurrentlyEquippedItem);
}
