// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HeroesAbilitySet.h"
#include "Inventory/ItemTraits/InventoryItemTraitBase.h"
#include "EquippableItemTrait.generated.h"

class UHeroesAbilitySet;
class UItemCharacterAnimationData;

/**
 * Defines where an item actor is attached when it's equipped. This defines creates an objective attachment location,
 * regardless of the equipping actor's skeleton, the player's perspective, etc.
 */
UENUM(BlueprintType)
enum class EEquipmentAttachSocket : uint8
{
	// The character's hand.
	Hand = 0
};

/**
 * A map with each equipment attachment socket's corresponding socket name.
 */
static TMap<EEquipmentAttachSocket, FName> EquipmentAttachSocketNames
{
	{ EEquipmentAttachSocket::Hand, FName("EquipSocket_Hand") }
};

/**
 * Allows this item to be equipped by players. Defines data and logic used to equip this item.
 */
UCLASS(DisplayName = "Equippable", BlueprintType)
class HEROESPROTOTYPEBASE_API UEquippableItemTrait : public UInventoryItemTraitBase
{
	GENERATED_BODY()

	// Equipment logic.

public:

	/** Called when the player equips this item. Default implementation spawns and attaches this item's actor, grants
	 * this item's equipped ability sets, updates the character's animation data, and plays this item's "equip"
	 * animation. */
	virtual void OnEquipped(UInventoryItemInstance* ItemToEquip);

	/** Called when the player unequips this item. Default implementation destroys this item's actor and removes this
	 * item's equipped ability sets. */
	virtual void OnUnequipped(UInventoryItemInstance* ItemToUnequip);

	/** Blueprint-implemented event called when the player equips this item. Called after @OnEquipped. */
	UFUNCTION(BlueprintImplementableEvent)
	void B_OnEquipped(UInventoryItemInstance* ItemToEquip);

	/** Blueprint-implemented event called when the player unequips this item. Called after @OnUnequipped.*/
	UFUNCTION(BlueprintImplementableEvent)
	void B_OnUnequipped(UInventoryItemInstance* ItemToUnequip);



	// Static data.

// Actor spawned when this item is equipped.
public:

	/** The actor that will be spawned when this item is equipped and destroyed when unequipped. */
	UPROPERTY(EditDefaultsOnly, Category = "Equipped Actor", DisplayName = "Actor to Spawn When This Item is Equipped")
	TSubclassOf<AActor> ActorToSpawnOnEquip;

	/** The socket to which this item's actor attaches when spawned. */
	UPROPERTY(EditDefaultsOnly, Category = "Equipped Actor")
	EEquipmentAttachSocket AttachmentSocket;

	/** The relative offset that will be applied to this item's actor when spawned after being attached to the
	 * specified socket. */
	UPROPERTY(EditDefaultsOnly, Category = "Equipped Actor")
	FTransform AttachmentOffset;

// Animation.
public:

	/** The amount of time it takes to equip this weapon. The "equip" animation is scaled to match this duration so
	 * that equip times can be adjusted without affecting animations. */
	UPROPERTY(EditDefaultsOnly, Category = "Animation", DisplayName = "Time to Equip")
	float EquipTime;

	/** The set of character animations used by this item. The character will switch to these animations when
	 * equipping this item. */
	UPROPERTY(EditDefaultsOnly, Category = "Animation", DisplayName = "Item Character Animation Data")
	TObjectPtr<UItemCharacterAnimationData> CharacterAnimationData;

// Ability system.
public:

	/** Ability sets granted to the player when this item is equipped and removes when this item is unequipped. */
	UPROPERTY(EditDefaultsOnly, Category = "Ability System", DisplayName = "Ability Sets to Grant When Equipped")
	TArray<TObjectPtr<UHeroesAbilitySet>> GrantedAbilitySetsOnEquip;



	// Runtime data.

public:

	/** The actor that represents this item in first-person when equipped. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<AActor> FirstPersonEquippedActor = nullptr;

	/** The actor that represents this item in third-person when equipped. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<AActor> ThirdPersonEquippedActor = nullptr;

	/** A collection of handles for ability sets granted to the player while this item is equipped. */
	TArray<FHeroesAbilitySet_GrantedHandles> GrantedAbilitySetHandles;
};
