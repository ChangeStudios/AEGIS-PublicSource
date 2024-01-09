// Copyright Samuel Reitich 2024.

#pragma once

#include "AbilitySystem/Abilities/HeroesGameplayAbilityBase.h"

#include "GameplayAbilitySpec.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HeroesAbilitySet.generated.h"

class UGameplayEffect;
class UHeroesAttributeSetBase;
class UHeroesGameplayAbilityBase;

class UHeroesAbilitySystemComponent;

/**
 * Data used to grant a gameplay ability via an ability set. This associates an ability with its corresponding input
 * tag. For example, ultimate abilities should be associated with the Input.Ultimate tag.
 *
 * Any abilities granted to an ASC should be given through an ability set. Granting an ability set automatically
 * assigns the granted gameplay abilities with their corresponding input tags, allowing them to be activated using
 * input actions with matching input tags.
 *
 * This method provides greater flexibility than hard-coding input tags into each gameplay ability.
 */
USTRUCT(BlueprintType)
struct FHeroesAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	/** The gameplay ability to grant. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHeroesGameplayAbilityBase> GameplayAbility = nullptr;

	/** The tag used to link this ability to an input action. This can differ depending on the ability set. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};



/**
 * Data used to grant a gameplay effect via an ability set.
 */
USTRUCT(BlueprintType)
struct FHeroesAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	/** The gameplay effect to grant. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	/** The level of the gameplay effect to grant. */
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};



/**
 * Data used to grant an attribute set via an ability set.
 */
USTRUCT(BlueprintType)
struct FHeroesAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:

	/** The attribute set to grant. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHeroesAttributeSetBase> AttributeSet = nullptr;
};



/**
 * Data used to store handles to track and manage abilities, effects, and attribute sets granted by an
 * ability set.
 */
USTRUCT(BlueprintType)
struct FHeroesAbilitySet_GrantedHandles
{
	GENERATED_BODY()

	// Handle managers.

public:

	/** Adds the given gameplay ability spec handle to the collection of ability handles. */
	void AddGameplayAbilitySpecHandle(const FGameplayAbilitySpecHandle& HandleToAdd);
	/** Adds the given gameplay effect handle to the collection of effect handles. */
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& HandleToAdd);
	/** Adds the given attribute set to the collection of tracked attribute sets. */
	void AddAttributeSet(UAttributeSet* SetToAdd);

	/** Removes every granted ability, effect, and attribute set from the specified ability system. */
	void RemoveFromAbilitySystem(UHeroesAbilitySystemComponent* AbilitySystemToRemoveFrom);



	// Handle collections.

protected:

	/** Handles to the granted gameplay abilities. */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GameplayAbilitySpecHandles;

	/** Handles to the granted gameplay effects. */
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	/** Pointers to the granted attribute sets. */
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};



/**
 * A data asset used to grant a collection of gameplay abilities, gameplay effects, and/or attribute sets. When an
 * ability set is given to an ASC, every ability, effect, and attribute set in the ability set will be given to the ASC.
 */
UCLASS(BlueprintType, Const)
class HEROESPROTOTYPEBASE_API UHeroesAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Default constructor. */
	UHeroesAbilitySet(const FObjectInitializer& ObjectInitializer);

	/**
	 *	Grants the ability set to the specified ability system component. This gives the ASC each ability,
	 *	effect, and attribute set in this ability set.
	 *
	 *	@param AbilitySystemToGiveTo	The ASC to grant this ability set to.
	 *	@param OutGrantedHandles		A collection of handles to the granted abilities, effects, and attribute sets. 
	 									This can be used to remove these later.
	 *	@param SourceObject				The object responsible for granting this ability set.
	 */
	void GiveToAbilitySystem(UHeroesAbilitySystemComponent* AbilitySystemToGiveTo, FHeroesAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;



	// Granted collections.

public:

	/** The gameplay abilities to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=GameplayAbility))
	TArray<FHeroesAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	/** The gameplay effects to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FHeroesAbilitySet_GameplayEffect> GrantedGameplayEffects;

	/** The attribute sets to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FHeroesAbilitySet_AttributeSet> GrantedAttributeSets;

};
