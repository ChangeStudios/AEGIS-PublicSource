// Copyright Samuel Reitich 2024.

#pragma once

#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "AbilitySystem/AttributeSets/HeroesAttributeSetBase.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UBaseHealthAttributeValueData;
class UHeroesAbilitySystemComponent;
struct FOnAttributeChangeData;

/**
 * Defines the current state of an actor's death.
 */
UENUM(BlueprintType)
enum class EDeathState : uint8
{
	Alive = 0,
	DeathStarted,
	DeathFinished
};



/**
 * A component used to provide health functionality to its owner with the gameplay ability system. This acts as an
 * interface to the ability system's health framework and the attributes in the health attribute set.
 *
 * To use this component, a UHealthAttributeSet sub-object needs to be created in the constructor of whichever class
 * contains the ability system component (i.e. the pawn or the player state).
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	// Initialization.

public:

	/** Default constructor. */
	UHealthComponent(const FObjectInitializer& ObjectInitializer);

	/** Links this health component and its health attribute set to the given ASC. Must be called after the ASC is
	 * initialized. */
	void InitializeWithAbilitySystem(UHeroesAbilitySystemComponent* InASC);

	/** Unlinks this health component and its health attribute set from the given ASC. */
	void UninitializeFromAbilitySystem();

	/** Returns whether or not this component has been initialized with an ASC and a health attribute set. */
	bool IsInitialized() const { return HeroesASC && HealthAttributeSet; }

protected:

	/** Called when this component is unregistered. Wraps UninitializeFromAbilitySystem. */
	virtual void OnUnregister() override;



	// Utils.

public:

	/** Retrieves the health component from a given actor, if it has one. */
	UFUNCTION(BlueprintPure, Category = "Heroes|Health")
	static UHealthComponent* FindHealthComponent(const AActor* ActorToSearch) { return ActorToSearch ? ActorToSearch->FindComponentByClass<UHealthComponent>() : nullptr; }

	/** Returns the original instigator of the change data's gameplay effect. Returns nullptr if the data's gameplay
	 * effect is null. */
	static AActor* GetInstigatorFromAttributeChangeData(const FOnAttributeChangeData& ChangeData);

	/** The data asset containing the base values for health attributes. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	TObjectPtr<UBaseHealthAttributeValueData> BaseHealthAttributeData = nullptr;


// Accessors
public:

	/** Gets the current health value. Wrapper for attribute set's GetHealth function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Health")
	float GetHealth() const;

	/** Gets the current maximum health value. Wrapper for attribute set's GetMaximumHealth function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Health")
	float GetMaximumHealth() const;

	/** Gets the current overhealth value. Wrapper for attribute set's GetOverhealth function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Health")
	float GetOverhealth() const;

	/** Returns the owner's current death state. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Health")
	EDeathState GetDeathState() const { return DeathState; }



	// Attribute changes.

// Delegates fired when an attribute is changed.
public:

	/** Delegate fired when the health attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature HealthChangedDelegate;

	/** Delegate fired when the maximum health attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature MaximumHealthChangedDelegate;

	/** Delegate fired when the overhealth attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature OverhealthChangedDelegate;

// Virtual functions called when an attributes is changed.
public:

	/** Called when the health attribute value changes. Broadcasts HealthChangedDelegate. */
	virtual void OnHealthChanged(const FOnAttributeChangeData& ChangeData);

	/** Called when the maximum health attribute value changes. Broadcasts MaximumHealthChangedDelegate. */
	virtual void OnMaximumHealthChanged(const FOnAttributeChangeData& ChangeData);

	/** Called when the overhealth attribute value changes. Broadcasts OverhealthChangedDelegate. */
	virtual void OnOverhealthChanged(const FOnAttributeChangeData& ChangeData);


	// Critical hits.

public:

	/** Whether or not this actor has a critical hit point that increases the amount of damage received when hit. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Critical Hits")
	bool bHasCriticalHitPoint;

	/** The mesh where this actor's critical hit point is located, if they have one. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Critical Hits")
	USkeletalMeshComponent* CriticalHitMesh;

	/** The names of all bones in the CriticalHitMesh that are critical hit points. This is usually just the "Head" bone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Critical Hits")
	TArray<FName> CriticalHitBones;



	// Death.

// Functions.
public:

	/** Begins the death sequence for the owner. Broadcasts OnDeathStarted, where the death sequence logic should take
	 * place. */
	virtual void StartDeath();

	/** Ends the death sequence for the owner. Broadcasts OnDeathFinished, where final death sequence logic should take
	 * place. */
	virtual void FinishDeath();

	/** Called when the owning actor's current death state changes to replicate the death sequence to other clients.
	 * This reverts the given state change and calls StartDeath or finish death, which will not have been called yet,
	 * to handle updating the value locally. */
	UFUNCTION()
	virtual void OnRep_DeathState(EDeathState OldValue);

	/** Applies enough damage to kill the owner. Overrides damage invulnerabilities. */
	virtual void SelfDestructWithDamage();

	/** Bound to the health attribute set's OutOfHealthDelegate to handle running out of health (i.e. death). */
	virtual void OnOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

// Delegates.
public:

	/** Delegate fired when this character's death sequence begins. Bind the death sequence logic here. FinishDeath
	 * MUST be called at the end of the death sequence to properly end the sequence. */
	UPROPERTY(BlueprintAssignable)
	FHealthDeathEvent OnDeathStarted;

	/** Delegate fired when the death sequence ends. */
	UPROPERTY(BlueprintAssignable)
	FHealthDeathEvent OnDeathFinished;



	// Internal variables.

protected:

	/** Cached ability system used by this component. */
	UPROPERTY()
	TObjectPtr<UHeroesAbilitySystemComponent> HeroesASC;

	/** The health attribute set used by this component. */
	UPROPERTY()
	TObjectPtr<const UHealthAttributeSet> HealthAttributeSet;

	/** The current state of the owning actor's death. */
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EDeathState DeathState;
	

};