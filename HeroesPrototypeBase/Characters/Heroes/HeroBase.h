// Copyright Samuel Reitich 2024.

#pragma once

#include "AbilitySystemInterface.h"

#include "CoreMinimal.h"
#include "Characters/HeroesCharacterBase.h"
#include "HeroBase.generated.h"

class UInventoryManagementComponent;
class UCameraComponent;
class UFirstPersonSkeletalMeshComponent;
class UViewModelSkeletalMeshComponent;

class UAbilityInputComponent;
class UHeroesInputActionSet;
class UNativeInputComponent;
class UPlayerMappableInputConfig;

class UCombatComponent;
class UHealthComponent;
class UHeroesCharacterMovementComponent;

class UAbilitySystemExtensionComponent;
class UHeroesAbilitySet;
class UHeroesAbilitySystemComponent;

DECLARE_DELEGATE(FJumpedDelegateSignature);

/**
 * The base class for all playable characters, a.k.a. "heroes." Heroes share functionality utilized by all
 * playable characters, such as movement, health, a first-person camera, and an interface with the ability system. Even
 * if a character is not technically a "hero," it should still be derived from this class, as long as it is a character
 * that the player can control.
 *
 * This class should be derived from and not used directly.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API AHeroBase : public AHeroesCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

	// Construction.

public:

	/** Default constructor. */
	AHeroBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());



	// Initialization.

protected:

	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/** Performs server-side initialization for the owning player's ASC and grants this character's default ability sets. */
	virtual void PossessedBy(AController* NewController) override;

	/** Performs client-side initialization for the owning player's ASC. */
	virtual void OnRep_PlayerState() override;



	// Character components.

// Public accessors.
public:

	/** Accessor for this character's first-person camera component. */
	UFUNCTION(Category = "Heroes|Characters|HeroBase|Components")
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Accessor for this character's first-person mesh. */
	UFUNCTION(Category = "Heroes|Characters|HeroBase|Components")
	UFirstPersonSkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	
	/** Accessor for this character's third-person mesh. */
	UFUNCTION(Category = "Heroes|Characters|HeroBase|Components")
	USkeletalMeshComponent* GetThirdPersonMesh() const { return ThirdPersonMesh; }

// Character component sub-objects.
protected:

	/** The mesh only visible in first-person. This is only seen by the local player and spectators. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFirstPersonSkeletalMeshComponent> FirstPersonMesh;

	/** The mesh only visible in third-person. This mesh is seen by all other players, and also the local player when in a third-person perspective. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ThirdPersonMesh;

	/** First-person camera. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;



	// Ability system.

// Utils.
public:

	/** Gets this player's ASC, which is stored in its player state. */
	UFUNCTION(BlueprintPure, Category = "Heroes|Characters|HeroBase|AbilitySystem")
	UHeroesAbilitySystemComponent* GetHeroesAbilitySystemComponent() const;

	/** Gets this player's ASC using the ability system interface. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Getter for this character's health component. */
	UFUNCTION(BlueprintPure, Category = "Heroes|Characters|HeroBase|Components")
	UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	/** Getter for this character's combat component. */
	UFUNCTION(BlueprintPure, Category = "Heroes|Characters|HeroBase|Components")
	UCombatComponent* GetCombatComponent() const { return CombatComponent; }

	/** Getter for this character's custom character movement component. */
	UFUNCTION(BlueprintPure, Category = "Heroes|Characters|HeroBase|Components")
	UHeroesCharacterMovementComponent* GetHeroesCharacterMovementComponent() const;

// GAS components.
protected:

	/** Serves as an interface between this character and its ability system component, handling initialization and
	 * uninitialization. */
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UAbilitySystemExtensionComponent> AbilitySystemExtensionComponent;

	/** Handles this character's health. This is initialized and uninitialized by the AbilitySystemExtensionComponent
	 * because it requires an ability system component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent;

	/** Handles this character's combat multipliers. This is initialized and uninitialized by the
	 * AbilitySystemExtensionComponent because it requires an ability system component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> CombatComponent;

// Ability sets.
protected:

	/** A collection of default ability sets that will be granted and bound upon initialization. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability System")
	TArray<TObjectPtr<UHeroesAbilitySet>> DefaultAbilitySets;



	// Input.

// Configs.
protected:

	/** A collection of every default input config that a player can use. When initializing input, each contextually
	 * valid config will be set up.
	 *
	 * TODO: Create platform-specific tags (e.g. "supports gamepad") to determine which configs to actually set up.
	 */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TArray<TObjectPtr<UPlayerMappableInputConfig>> DefaultInputConfigs;

// Input components.
protected:

	/** Handles binding native input actions. */
	UPROPERTY(VisibleDefaultsOnly)
	UNativeInputComponent* NativeInputComponent;

	/** Handles binding ability input actions. */
	UPROPERTY(VisibleDefaultsOnly)
	UAbilityInputComponent* AbilityInputComponent;

// Input action sets.
protected:

	/** These action sets will be bound when the game starts. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability System")
	TArray<UHeroesInputActionSet*> DefaultActionSets;

// Input functions.
public:

	/** Called to bind functionality to input. */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

};
