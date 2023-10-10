// Copyright Samuel Reitich 2023.

#pragma once

#include "NativeGameplayTags.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilitySystemExtensionComponent.generated.h"

/** Native gameplay tags relevant to this class. */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Behavior_PersistsThroughAvatarDestruction);

class UHeroesAbilitySystemComponent;

/**
 * A component used for a pawn to manage its ability system component. This separates pawns' ability system logic into
 * a unique component, and primarily handles the changing of ability system components, including initialization and
 * uninitialization.
 *
 * This component can be housed in the pawn class, regardless of where the pawn's ability system is actually located
 * (i.e. in the pawn itself or in its player state).
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UAbilitySystemExtensionComponent : public UActorComponent
{
	GENERATED_BODY()

	// Construction.

public:

	/** Default constructor. */
	UAbilitySystemExtensionComponent(const FObjectInitializer& ObjectInitializer);



	// Ability system.

// Accessors.
public:

	/** Returns the specified actor's AbilitySystemExtensionComponent if it has one. Returns a nullptr otherwise. */
	UFUNCTION(BlueprintPure, Category = "Heroes|AbilitySystem|ExtensionComponent")
	static UAbilitySystemExtensionComponent* FindAbilitySystemExtensionComponent(const AActor* Actor);

	/** Gets the current ability system component, which may not be owned by its avatar. */
	UFUNCTION(BlueprintPure, Category = "Heroes|AbilitySystem|ExtensionComponent")
	UHeroesAbilitySystemComponent* GetHeroesAbilitySystemComponent() const { return AbilitySystemComponent; }

// ASC management.
public:

	/** Initializes the given ASC with the given owner and makes this component's owning pawn the ASC's avatar. This
	 * should be called when the pawn is possessed by a player with an ASC. If the ASC is stored in the pawn itself,
	 * this should be called when that pawn is initialized.
	 *
	 * @param InASC			The ability system component that this component's owning pawn will become the avatar of.
	 * @param InOwnerActor	The actor that will become the owner of the specified ASC. This can be its current owner if
	 *						the owner does not need to change.
	 */
	void InitializeAbilitySystem(UHeroesAbilitySystemComponent* InASC, AActor* InOwnerActor);

	/** Uninitializes this ASC from its current avatar. */
	void UninitializeAbilitySystem();

protected:

	/** Pointer to the ability system component that is cached for convenience. */
	UPROPERTY()
	TObjectPtr<UHeroesAbilitySystemComponent> AbilitySystemComponent;
};
