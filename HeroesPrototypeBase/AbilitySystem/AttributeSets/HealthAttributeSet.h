// Copyright Samuel Reitich 2024.

#pragma once

#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"

#include "CoreMinimal.h"
#include "AbilitySystem/AttributeSets/HeroesAttributeSetBase.h"
#include "HealthAttributeSet.generated.h"

/** Native gameplay tags relevant to this class. */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_ImmuneToDamage);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Death);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Death_Dead);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Death_Dying);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Death_Dying_SelfDestructing);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Event_Death);

/** Delegate fired when an actor "dies" from running out of health. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthDeathEvent, AActor*, OwningActor);

class UBaseHealthAttributeValueData;

/**
 * The attribute set for all health attributes: health, maximum health, overhealth, and maximum overhealth.
 *
 * Note: Health atttributes should NEVER be modified directly by gameplay effects. They should only be modified by
 * executions. Gameplay effects should always use executions to modify health attributes. Otherwise, certain functions
 * won't properly trigger.
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UHealthAttributeSet : public UHeroesAttributeSetBase
{
	GENERATED_BODY()

	// Attribute initialization.

public:

	/** Default constructor. Initializes attribute values. */
	UHealthAttributeSet();



	// Attribute modification.

protected:

	/** Called before modifying the value of an attribute with a gameplay effect. Return true to continue the
	 * modification; return false to cancel the modification. */
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

	/** Called before a gameplay effect modifies the value of an attribute with an execution. */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/** Called before an attribute is modified. This is called after PreGameplayEffectExecute and
	 * PostGameplayEffectExecute. This should enforce things like clamping attributes to their minimum and maximum
	 * values. */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** Called immediately after an attribute is modified. */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	/** Called before an attribute's base value is modified when an attribute aggregator exists. This is called before
	 * PreAttributeChange and should enforce things like clamping attributes' base values to their minimum and maximum
	 * values. */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/** Clamps the given attribute to its minimum value and its current maximum attribute value. Then rounds the
	 * attribute down to the nearest whole number, because only whole numbers are ever displayed to players. This is
	 * a redundant fallback measure because damage and healing should also only ever be applied in whole numbers. */
	void ClampAndRoundAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;



	// Attribute accessors. Creates Get, GetAttribute, Set, and Init functions for each attribute.

public:

	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaximumHealth);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Overhealth);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaximumOverhealth);

	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Damage);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Healing);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Overhealing);



	// Attribute events.

public:

	/** Delegate to broadcast when health reaches 0. */
	mutable FAttributeEventSignature OutOfHealthDelegate;



	// This set's attributes.

private:

	/** The current health attribute. Health is capped at MaximumHealth. Health is hidden from modifiers so it can only
	 * be modified by executions. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Heroes|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

		/** Tracks when health reaches 0. */
		bool bOutOfHealth;

	/** The maximum health attribute. This caps the current health attribute. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaximumHealth, Category = "Heroes|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaximumHealth;

	/** The current overhealth attribute. Overhealth is capped at MaximumOverhealth. Overhealth is hidden from
	 * modifiers so it can only be modified by executions. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Overhealth, Category = "Heroes|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Overhealth;

	/** The maximum overhealth attribute. This caps the current overhealth attribute. Our base maximum overhealth
	 * should usually be the same as our base maximum health (this is a game design choice, not a programming choice). */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaximumOverhealth, Category = "Heroes|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaximumOverhealth;



	// Meta attributes. These are placeholders for temporary values that modify attributes. For example, instead of
	// directly subtracting Health, damage adds to Damage, which in turn subtracts from Health.

private:

	/** Incoming damage. This is mapped directly to -Overhealth, then -Health if Overhealth is 0. Damage is hidden from
	 * modifiers so it can only be modified by executions. */
	UPROPERTY(BlueprintReadOnly, Category="Heroes|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	/** Incoming healing. This mapped directly to +Health. Healing is hidden from modifiers so it can only be modified
	 * by executions.*/
	UPROPERTY(BlueprintReadOnly, Category="Heroes|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Healing;

	/** Incoming healing. This mapped directly to +Health. If health is full, this is mapped to +Overhealth.
	 * Overhealing is hidden from modifiers so it can only be modified by executions. */
	UPROPERTY(BlueprintReadOnly, Category="Heroes|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Overhealing;



	// OnRep functions for attribute changes.

protected:

	/** OnRep for Health. */
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	/** OnRep for MaximumHealth. */
	UFUNCTION()
	void OnRep_MaximumHealth(const FGameplayAttributeData& OldValue);

	/** OnRep for Overhealth. */
	UFUNCTION()
	void OnRep_Overhealth(const FGameplayAttributeData& OldValue);

	/** OnRep for MaximumOverhealth. */
	UFUNCTION()
	void OnRep_MaximumOverhealth(const FGameplayAttributeData& OldValue);

};
