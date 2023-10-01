// Copyright Samuel Reitich 2023.

#pragma once

#include "AbilitySystemComponent.h"

#include "CoreMinimal.h"
#include "AbilitySystem/AttributeSets/HeroesAttributeSetBase.h"
#include "MovementAttributeSet.generated.h"

/**
 * The attribute set for all movement-related attributes such as speed, gravity, etc.
 *
 * Note: Unlike health attributes, movement attributes can be directly modified by gameplay effect modifiers.
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UMovementAttributeSet : public UHeroesAttributeSetBase
{
	GENERATED_BODY()

	// Attribute initialization.

public:

	/** Default constructor. Initializes attribute values. */
	UMovementAttributeSet();



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

	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, MovementSpeed);
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, MovementAcceleration);
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, DirectionalControl);
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, GravityScale);
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, JumpStrength);



	// This set's attributes.

private:

	/** The value used for maximum walk speed. This affects the maximum crouch-walk speed and the maximum
	 * slow-walk speed, which are calculated using the maximum walk speed. Measured in cm/s. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed, Category = "Heroes|Movement", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MovementSpeed;

	/** The value used for maximum acceleration. Measured in cm/s^2. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementAcceleration, Category = "Heroes|Movement", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MovementAcceleration;

	/** An aggregate value used to determine directional movement control. This affects air control, braking friction
	 * factor, and walking brake deceleration. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DirectionalControl, Category = "Heroes|Movement", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DirectionalControl;

	/** A multiplier applied to gravity. A gravity scale of 1.0 results in a force of 980 cm/s. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GravityScale, Category = "Heroes|Movement", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData GravityScale;

	/** The force with which an actor is launched when jumping, measured in cm/s. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_JumpStrength, Category = "Heroes|Movement", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData JumpStrength;



	// OnRep functions for attribute changes.

protected:

	/** OnRep for MovementSpeed. */
	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldValue);

	/** OnRep for MovementAcceleration. */
	UFUNCTION()
	void OnRep_MovementAcceleration(const FGameplayAttributeData& OldValue);

	/** OnRep for DirectionalControl. */
	UFUNCTION()
	void OnRep_DirectionalControl(const FGameplayAttributeData& OldValue);

	/** OnRep for GravityScale. */
	UFUNCTION()
	void OnRep_GravityScale(const FGameplayAttributeData& OldValue);

	/** OnRep for JumpStrength. */
	UFUNCTION()
	void OnRep_JumpStrength(const FGameplayAttributeData& OldValue);

};
