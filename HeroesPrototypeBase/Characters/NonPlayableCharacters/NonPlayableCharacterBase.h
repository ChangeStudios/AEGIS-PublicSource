// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Characters/HeroesCharacterBase.h"
#include "NonPlayableCharacterBase.generated.h"

class UHealthAttributeSet;
class UAbilitySystemExtensionComponent;
class UCombatComponent;
class UHealthComponent;
class UHeroesAbilitySystemComponent;

/**
 * The base class for all non-playable characters, such as AI-controlled bots. This contains an ability system
 * component, various attribute sets, and other functionality that would normally be handled by a player state.
 *
 * This class should be derived from and not used directly.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API ANonPlayableCharacterBase : public AHeroesCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

	// Construction.

public:

	/** Default constructor. */
	ANonPlayableCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());



	// Initialization.

public:

	/** Initializes the ASC with this character as the owner and avatar. */
	virtual void PostInitializeComponents() override;



	// Ability system.

// Utils.
public:

	/** Typed getter for the ASC. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|NPCs|AbilitySystem")
	UHeroesAbilitySystemComponent* GetHeroesAbilitySystemComponent() const { return AbilitySystemComponent; }

	/** Interfaced getter for the ASC. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

// GAS components.
protected:

	/** The ability system component used by this non-player character. */
	UPROPERTY(VisibleAnywhere, Category = "Heroes|NPCs|AbilitySystem")
	TObjectPtr<UHeroesAbilitySystemComponent> AbilitySystemComponent;

	/** Handles the initialization and uninitialization of this character's ASC. */
	UPROPERTY(VisibleDefaultsOnly, Category = "AbilitySystem")
	UAbilitySystemExtensionComponent* AbilitySystemExtensionComponent;

	/** Handles this character's health. This is initialized and uninitialized by the AbilitySystemExtensionComponent
	 * because it requires an ability system component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;
	
	/** Handles this character's combat multipliers. This is initialized and uninitialized by the
	 * AbilitySystemExtensionComponent because it requires an ability system component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComponent;

	

};
