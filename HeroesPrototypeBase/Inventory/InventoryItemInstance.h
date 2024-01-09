// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagStack.h"
#include "InventoryItemInstance.generated.h"

class AHeroesGamePlayerStateBase;
class UInventoryItemDefinition;

/**
 * Represents a single instance of an item that exists in the game. Item instances should always either be in a
 * player's inventory or in an unowned InventoryItemPickupActor.
 *
 * Item instances are not bound to any single inventory throughout their lifetime, allowing the same item instance to
 * be passed between multiple players' inventories without losing information.
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()

	// Construction.

public:

	/** Default constructor. */
	UInventoryItemInstance();

	/** Initializes this item instance with the specified item definition. Creates a new item definition object of the
	 * specified class to hold runtime data for this item instance. Must be called when a new item instance is
	 * created. */
	virtual void Init(TSubclassOf<UInventoryItemDefinition> InItemDefinition, AHeroesGamePlayerStateBase* InCurrentOwner = nullptr);

	/** Uninitializes all of this item instance's traits when the item is destroyed. */
	virtual void BeginDestroy() override;



	// Networking.

public:

	/** Allow this object to be referenced over the network. */
	virtual bool IsSupportedForNetworking() const override { return true; }



	// Item data.

// Getters/setters.
public:

	/** Getter for this item's definition object. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Heroes|Inventory")
	const UInventoryItemDefinition* GetItemDefinition() const;

	/** Getter for this item instance's current owning player state. */
	const AHeroesGamePlayerStateBase* GetCurrentOwner() const { return CurrentOwner; }

	/** Setter for this item instance's current owning player state. This is used to update this item's owner when it
	 * enters or exits an inventory. Can be used to clear this item's owner by passing a nullptr. */
	void SetCurrentOwner(AHeroesGamePlayerStateBase* InCurrentOwner);

// Statistic tag management.
public:

	/** Adds the given quantity of the specified tag to this item's statistic tags. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heroes|Inventory")
	void AddStatTags(FGameplayTag Tag, int32 Quantity);

	/** Removes the given quantity of the specified tag from this item's statistic tags. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heroes|Inventory")
	void RemoveStatTags(FGameplayTag Tag, int32 Quantity);

	/** Returns the quantity of the specified tag in this item's container of statistic tags. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Heroes|Inventory")
	int32 GetStatTagCount(FGameplayTag Tag) const;

	/** Returns true if this item has at least one of the specified tags in its container of statistic tags. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Heroes|Inventory")
	bool HasStatTag(FGameplayTag Tag) const;

// Data.
private:

	/** The class of this inventory's item definition. This is used to access static data related to this item. */
	UPROPERTY(Replicated)
	TSubclassOf<UInventoryItemDefinition> ItemDefinitionClass;

	/** The item definition object created by this item instance when initialized. This object holds item-specific
	 * runtime data for this item instance. */
	UPROPERTY(Replicated)
	TObjectPtr<UInventoryItemDefinition> ItemDefinition;

	/** A collection of gameplay tag stacks used to track runtime statistics for this item. E.g. a weapon's current
	 * ammunition. */
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

	/** The current owning player state of this item. This is the player state containing the inventory list that this
	 * item is in. This is null if this item is not currently in a player's inventory. */
	UPROPERTY(Replicated)
	TObjectPtr<AHeroesGamePlayerStateBase> CurrentOwner = nullptr;
};
