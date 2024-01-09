// Copyright Samuel Reitich 2024.

#pragma once

#include "AbilitySystemInterface.h"

#include "CoreMinimal.h"
#include "Player/PlayerStates/HeroesPlayerStateBase.h"
#include "HeroesGamePlayerStateBase.generated.h"

class UInventoryComponent;
class UHeroesAbilitySystemComponent;

/**
 * The base class for all player states used in games. Contains persistent player data that is tracked regardless of
 * the game mode, such as the player's inventory, the currently selected hero, or the player's kill count.
 *
 * Most importantly, the game player state owns and manages the player's ability system component, allowing persistent
 * ability data, like cooldowns and ultimate charge, to be tracked when the player changes possession (e.g. switching
 * heroes) or does not control a pawn (e.g. while dead).
 *
 * This class should be derived from and not used directly.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API AHeroesGamePlayerStateBase : public AHeroesPlayerStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

	// Construction.

public:

	/** Default constructor. */
	AHeroesGamePlayerStateBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());



	// Initialization.

public:

	/** Initializes the ASC with this player state as the owner. */
	virtual void PostInitializeComponents() override;



	// Inventory.

public:

	/** Getter for the inventory component. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Heroes|PlayerState|Inventory")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

protected:

	/** The inventory component used to persistently hold and manage this player's inventory. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UInventoryComponent> InventoryComponent;



	// Ability system.

public:

	/** Typed getter for the ASC. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Heroes|PlayerState|AbilitySystem")
	UHeroesAbilitySystemComponent* GetHeroesAbilitySystemComponent() const { return AbilitySystemComponent; }

	/** Interfaced getter for the ASC. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	/** The ability system component used by player characters. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UHeroesAbilitySystemComponent> AbilitySystemComponent;
};
