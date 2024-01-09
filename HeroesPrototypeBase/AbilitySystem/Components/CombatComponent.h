// Copyright Samuel Reitich 2024.

#pragma once

#include "AbilitySystem/AttributeSets/HeroesAttributeSetBase.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UCombatAttributeSet;
class UHeroesAbilitySystemComponent;
struct FOnAttributeChangeData;

/**
 * A component used to provide combat multiplier functionality to its owner with the gameplay ability system. This acts
 * as an interface to the attribute set of combat multipliers used to apply damage and healing with the ability
 * system's health framework.
 *
 * To use this component, a UCombatAttributeSet sub-object needs to be created in the constructor of whichever class
 * contains the ability system component (i.e. the pawn or the player state).
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	// Initialization.

public:

	/** Default constructor. */
	UCombatComponent(const FObjectInitializer& ObjectInitializer);

	/** Links this combat component and its combat attribute set to the given ASC. Must be called after the ASC is
	 * initialized. */
	void InitializeWithAbilitySystem(UHeroesAbilitySystemComponent* InASC);

	/** Unlinks this combat component and its combat attribute set from the given ASC. */
	void UninitializeFromAbilitySystem();

	/** Returns whether or not this component has been initialized with an ASC and a combat attribute set. */
	bool IsInitialized() const { return HeroesASC && CombatAttributeSet; }

protected:

	/** Called when this component is unregistered. Wraps UninitializeFromAbilitySystem. */
	virtual void OnUnregister() override;



	// Utils.

public:

	/** Retrieves the combat component from a given actor, if it has one. */
	UFUNCTION(BlueprintPure, Category = "Heroes|Combat")
	static UCombatComponent* FindCombatComponent(const AActor* ActorToSearch) { return ActorToSearch ? ActorToSearch->FindComponentByClass<UCombatComponent>() : nullptr; }

	/** Returns the original instigator of the change data's gameplay effect. Returns nullptr if the data's gameplay
	 * effect is null. */
	static AActor* GetInstigatorFromAttributeChangeData(const FOnAttributeChangeData& ChangeData);

	// Accessors

public:

	/** Gets the current outgoing damage multiplier value. Wrapper for attribute set's GetOutgoingDamageMultiplier
	 * function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Combat")
	float GetOutgoingDamageMultiplier() const;

	/** Gets the current incoming damage multiplier value. Wrapper for attribute set's GetIncomingDamageMultiplier
	 * function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Combat")
	float GetIncomingDamageMultiplier() const;

	/** Gets the current outgoing healing multiplier value. Wrapper for attribute set's GetOutgoingHealingMultiplier
	 * function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Combat")
	float GetOutgoingHealingMultiplier() const;

	/** Gets the current incoming healing multiplier value. Wrapper for attribute set's GetIncomingHealingMultiplier
	 * function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Combat")
	float GetIncomingHealingMultiplier() const;



	// Attribute changes.

// Delegates fired when an attribute is changed.
public:

	/** Delegate fired when the outgoing damage multiplier attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature OutgoingDamageMultiplierChangedDelegate;

	/** Delegate fired when the incoming damage multiplier attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature IncomingDamageMultiplierChangedDelegate;

	/** Delegate fired when the outgoing healing multiplier attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature OutgoingHealingMultiplierChangedDelegate;

	/** Delegate fired when the incoming healing multiplier attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature IncomingHealingMultiplierChangedDelegate;

// Virtual functions called when an attributes is changed.
public:

	/** Called when the outgoing damage multiplier attribute value changes. Broadcasts OutgoingDamageMultiplierChangedDelegate. */
	virtual void OnOutgoingDamageMultiplierChanged(const FOnAttributeChangeData& ChangeData);

	/** Called when the incoming damage multiplier attribute value changes. Broadcasts IncomingDamageMultiplierChangedDelegate. */
	virtual void OnIncomingDamageMultiplierChanged(const FOnAttributeChangeData& ChangeData);

	/** Called when the outgoing healing multiplier attribute value changes. Broadcasts OutgoingHealingMultiplierChangedDelegate. */
	virtual void OnOutgoingHealingMultiplierChanged(const FOnAttributeChangeData& ChangeData);

	/** Called when the incoming healing multiplier attribute value changes. Broadcasts IncomingHealingMultiplierChangedDelegate. */
	virtual void OnIncomingHealingMultiplierChanged(const FOnAttributeChangeData& ChangeData);



	// Internal variables.

protected:

	/** Cached ability system used by this component. */
	UPROPERTY()
	TObjectPtr<UHeroesAbilitySystemComponent> HeroesASC;

	/** The combat attribute set used by this component. */
	UPROPERTY()
	TObjectPtr<const UCombatAttributeSet> CombatAttributeSet;

};
