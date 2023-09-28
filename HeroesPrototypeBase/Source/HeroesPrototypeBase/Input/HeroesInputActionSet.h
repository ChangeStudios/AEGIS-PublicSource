// Copyright Samuel Reitich 2023.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HeroesInputActionSet.generated.h"

class UInputAction;

/**
 * Data used to associate an input tag with an input action. This is used in input action sets to link
 * abilities in ability sets with their input action using their input tag.
 */
USTRUCT(BlueprintType)
struct FHeroesInputAction
{
	GENERATED_BODY()

public:

	/** The input action represented by this structure. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	/** The input tag corresponding to the input action. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * Data used to link input tags with input actions. Input tags are linked to abilities via ability sets and
 * input actions are linked to input mappings with mapping contexts. This allows us to activate abilities
 * with an input tag, triggered by that tag's input action.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UHeroesInputActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Searches for a native input action in this action set that is associated with the given input tag. Returns
	 * nullptr if one was not found. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Input|InputActions")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	/** Searches for an ability input action in this action set that is associated with the given input tag. Returns
	 * nullptr if one was not found. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Input|InputActions")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;



	// Input action collections.

public:

	/** Input actions that do not have gameplay abilities associated with them. To use these actions, you
	 * must manually bind them to handler functions. This should be done in an actor component that
	 * initializes player input. */
	UPROPERTY(EditDefaultsOnly)
	TArray<FHeroesInputAction> NativeInputActions;

	/** Input actions with input tags linked to abilities. These actions can be automatically bound to
	 * their corresponding abilities using their input tags. */
	UPROPERTY(EditDefaultsOnly)
	TArray<FHeroesInputAction> AbilityInputActions;
};
