// Copyright Samuel Reitich 2023.

#pragma once

#include "AbilitySystem/AttributeSets/HeroesAttributeSetBase.h"
#include "NativeGameplayTags.h"

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HeroesCharacterMovementComponent.generated.h"

/** Native gameplay tags relevant to this class. */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Identifier_Action_Generic_Jump);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SetByCaller_Movement);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SetByCaller_Movement_Acceleration);

class UCurveFloat;
class UHeroesAbilitySystemComponent;
class UMovementAttributeSet;
struct FOnAttributeChangeData;

/**
 * TODO: Document
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UHeroesCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	// Native initialization.

public:

	/** Default constructor. */
	UHeroesCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	/** Native initializer. */
	virtual void InitializeComponent() override;

protected:

#if WITH_EDITOR
	/** Determines whether certain properties can be changed in the editor. This is used to disable properties from
	 * parent classes that we don't want to change in the editor. */
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif



	// Ability system initialization.

public:

	/** Links this character movement component and its movement attribute set to the given ASC. Must be called after
	 * the ASC is initialized. */
	void InitializeWithAbilitySystem(UHeroesAbilitySystemComponent* InASC);

	/** Unlinks this character movement component and its movement attribute set from the given ASC. */
	void UninitializeFromAbilitySystem();

	/** Returns whether or not this component has been initialized with an ASC and a movement attribute set. */
	bool IsInitialized() const { return HeroesASC && MovementAttributeSet; }

protected:

	/** Called when this component is unregistered. Wraps UninitializeFromAbilitySystem. */
	virtual void OnUnregister() override;



	// Utils.

public:

	/** Retrieves the heroes character movement component from a given actor, if it has one. */
	UFUNCTION(BlueprintPure, Category = "Heroes|Character Movement")
	static UHeroesCharacterMovementComponent* FindHeroesCharacterMovementComponent(const AActor* ActorToSearch) { return ActorToSearch ? ActorToSearch->FindComponentByClass<UHeroesCharacterMovementComponent>() : nullptr; }



	// Attributes.

// Utils.
public:

	/** Returns the original instigator of the change data's gameplay effect. Returns nullptr if the data's gameplay
	 * effect is null. */
	static AActor* GetInstigatorFromAttributeChangeData(const FOnAttributeChangeData& ChangeData);

/* Base movement variables. These save the original values of their corresponding variables, so those that variables
 * can be modified by attributes without losing their starting value. These behave similar attributes' base values,
 * but more lightweight. */
public:

	/** Base value for AirControl. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Heroes|Character Movement|Attributes")
	float BaseAirControl;

	/** Base value for BrakingFrictionFactor. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Heroes|Character Movement|Attributes")
	float BaseBrakingFrictionFactor;

	/** Base value for BrakingDecelerationWalking. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Heroes|Character Movement|Attributes")
	float BaseBrakingDecelerationWalking;

// Internal variables.
protected:

	/** Cached ability system used by this component. */
	UPROPERTY()
	TObjectPtr<UHeroesAbilitySystemComponent> HeroesASC;

	/** The movement attribute set used by this component. */
	UPROPERTY()
	TObjectPtr<const UMovementAttributeSet> MovementAttributeSet;



	// Attribute accessors

public:

	/** Gets the current movement speed value. Wrapper for attribute set's GetMovementSpeed function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Character Movement")
	float GetMovementSpeed() const;

	/** Gets the current movement acceleration value. Wrapper for attribute set's GetMovementAcceleration function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Character Movement")
	float GetMovementAcceleration() const;

	/** Gets the current directional control value. Wrapper for attribute set's GetDirectionalControl function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Character Movement")
	float GetDirectionalControl() const;

	/** Gets the current gravity scale value. Wrapper for attribute set's GetGravityScale function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Character Movement")
	float GetGravityScale() const;

	/** Gets the current jump strength value. Wrapper for attribute set's GetJumpStrength function. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Character Movement")
	float GetJumpStrength() const;



	// Attribute changes.

// Delegates fired when an attribute is changed.
public:

	/** Delegate fired when the movement speed attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature MovementSpeedChangedDelegate;

	/** Delegate fired when the movement acceleration attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature MovementAccelerationChangedDelegate;

	/** Delegate fired when the directional control attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature DirectionalControlChangedDelegate;

	/** Delegate fired when the gravity scale attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature GravityScaleChangedDelegate;

	/** Delegate fired when the jump strength attribute value changes. */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature JumpStrengthChangedDelegate;

// Virtual functions called when an attributes is changed.
public:

	/** Called when the movement speed attribute value changes. Broadcasts MovementSpeedChangedDelegate. */
	virtual void OnMovementSpeedChangedChanged(const FOnAttributeChangeData& ChangeData);

	/** Called when the movement acceleration attribute value changes. Broadcasts MovementAccelerationChangedDelegate. */
	virtual void OnMovementAccelerationChanged(const FOnAttributeChangeData& ChangeData);

	/** Called when the directional control attribute value changes. Broadcasts DirectionalControlChangedDelegate. */
	virtual void OnDirectionalControlChanged(const FOnAttributeChangeData& ChangeData);

	/** Called when the gravity scale attribute value changes. Broadcasts GravityScaleChangedDelegate. */
	virtual void OnGravityScaleChanged(const FOnAttributeChangeData& ChangeData);

	/** Called when the jump strength attribute value changes. Broadcasts JumpStrengthChangedDelegate. */
	virtual void OnJumpStrengthChanged(const FOnAttributeChangeData& ChangeData);



	// Walking.

public:

	/** The normalized percentage by which walk speed is decreased when crouching. This is multiplied by the current
	 * maximum walking speed to set the crouch-walking speed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Walking", meta = (DisplayName = "Walk Speed Decrease from Crouch", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "0.0", DisplayAfter = "MaxWalkSpeedCrouched"))
	float CrouchWalkSpeedDecreaseNormalized;

	/** The normalized percentage by which walk speed is decreased when slow-walking. This is multiplied by the current
	 * maximum walking speed to set the slow-walking speed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Walking", meta = (DisplayName = "Walk Speed Decrease from Slow-Walk", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "0.0", DisplayAfter = "CrouchWalkSpeedDecreaseNormalized"))
	float SlowWalkSpeedDecreaseNormalized;



	// Landing.

public:

	/** Determines whether or not hard landings are enabled for this character. Hard landings slow characters when they
	 * land on the ground depending on the speed at which they fell. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Jumping / Falling", meta = (DisplayName = "Enable Hard Landing"))
	bool bHardLandingEnabled;

	/** If the character lands on the ground while falling at this speed or slower, they will not make a hard landing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Jumping / Falling", meta = (DisplayName = "Minimum Hard Landing Speed", ClampMin = "0", UIMin = "0", ForceUnits = "cm/s", EditCondition = "bHardLandingEnabled"))
	float MinimumHardLandingSpeed;

	/** How much the character will be slowed by a hard landing. If the character falls at the maximum falling speed, their velocity will be divided by this value. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Jumping / Falling", meta = (DisplayName = "Hard Landing Slow Scale", ClampMin = "0", UIMin = "0", EditCondition = "bHardLandingEnabled"))
	float HardLandingSlowScale;

	/**
	 * Returns how hard a character's landing was at the given speed. Returns 0.0 if hard landings are disabled or if the
	 * landing speed is below the "hard landing" threshold.
	 *
	 * @param LandingSpeed				The speed at which the actor landed (Z velocity).
	 * @param HardLandingEffectCurve	A curve that determines the rate at which landing hardness scales with falling
	 *									speed. Linear by default.
	 *
	 * @return							A normalized float that reflects the landing hardness normalized to the minimum
	 *									hard landing speed and the character's terminal velocity.
	 * 
	 */
	UFUNCTION(BlueprintPure, Category = "Heroes|Character Movement|Landing")
	float CalculateLandingHardness(float LandingSpeed, const UCurveFloat* HardLandingEffectCurve = nullptr) const;

protected:

	/** Scales back the player's velocity and acceleration depending on how fast they hit the ground. */
	UFUNCTION()
	void OnLanded(const FHitResult& Hit);

};
