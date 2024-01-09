// Copyright Samuel Reitich 2024.

#pragma once

#include "AbilitySystemComponent.h"

#include "CoreMinimal.h"
#include "AbilitySystem/AttributeSets/HeroesAttributeSetBase.h"
#include "CombatAttributeSet.generated.h"

/**
 * The attribute set for all combat attributes: incoming and outgoing damage multipliers, and incoming and outgoing
 * healing multipliers.
 *
 * Note: Unlike health attributes, combat attributes can be directly modified by gameplay effect modifiers.
 */
UCLASS(BlueprintType)
class UCombatAttributeSet : public UHeroesAttributeSetBase
{
	GENERATED_BODY()

	// Attribute initialization.

public:

	/** Default constructor. Initializes attribute values. */
	UCombatAttributeSet();



	// Attribute modification.

protected:

	/** Called before an attribute is modified. This should enforce things like clamping attributes to their minimum
	 * and maximum values. */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** Called before an attribute's base value is modified when an attribute aggregator exists. This should enforce
	 * things like clamping attributes' base values to their minimum and maximum values. */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/** Clamps the given attribute to its minimum value and its current maximum attribute value. */
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;



	// Attribute accessors. Creates Get, GetAttribute, Set, and Init functions for each attribute.

public:

	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, OutgoingDamageMultiplier);
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, IncomingDamageMultiplier);
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, OutgoingHealingMultiplier);
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, IncomingHealingMultiplier);



	// This set's attributes.

private:

	/** The current outgoing damage multiplier attribute. Damage dealt by this actor is multiplied by this value. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_OutgoingDamageMultiplier, Category = "Heroes|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData OutgoingDamageMultiplier;

	/** The current incoming damage multiplier attribute. Damage received by this actor is multiplied by this value. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IncomingDamageMultiplier, Category = "Heroes|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingDamageMultiplier;

	/** The current outgoing damage multiplier attribute. Healing given by this actor is multiplied by this value. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_OutgoingHealingMultiplier, Category = "Heroes|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData OutgoingHealingMultiplier;

	/** The current incoming damage multiplier attribute. Healing received by this actor is multiplied by this value. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IncomingHealingMultiplier, Category = "Heroes|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingHealingMultiplier;



	// OnRep functions for attribute changes.

protected:

	/** OnRep for OutgoingDamageMultiplier. */
	UFUNCTION()
	void OnRep_OutgoingDamageMultiplier(const FGameplayAttributeData& OldValue);

	/** OnRep for IncomingDamageMultiplier. */
	UFUNCTION()
	void OnRep_IncomingDamageMultiplier(const FGameplayAttributeData& OldValue);

	/** OnRep for OutgoingHealingMultiplier. */
	UFUNCTION()
	void OnRep_OutgoingHealingMultiplier(const FGameplayAttributeData& OldValue);

	/** OnRep for IncomingHealingMultiplier. */
	UFUNCTION()
	void OnRep_IncomingHealingMultiplier(const FGameplayAttributeData& OldValue);




};
