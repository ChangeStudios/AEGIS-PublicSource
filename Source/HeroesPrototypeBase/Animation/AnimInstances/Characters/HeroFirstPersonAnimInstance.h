	// Copyright Samuel Reitich 2023.

#pragma once

#include "Kismet/KismetMathLibrary.h"

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/HeroesAnimationTypes.h"
#include "HeroFirstPersonAnimInstance.generated.h"

	class UItemCharacterAnimationData;
class AHeroBase;
class UCharacterAnimationData;
class UFloatSpringInterpDataAsset;

/**
 * The base first-person animation instance for all playable characters ("heroes"). This class should only be
 * subclassed in blueprints, not in C++. This class is subclassed into a base animation blueprint, which can then be
 * subclassed into additional animation blueprints to add additional functionality on a per-hero basis, if needed.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UHeroFirstPersonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:

	/** Default initializer. */
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeBeginPlay() override;

	/** Thread-safe animation update function. We use this instead of the NativeUpdateAnimation function to utilize
	 * multi-threading in our animation system. This function can only call other thread-safe functions. */
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:

	/** Caches the velocity at which this animation instance's owning pawn hit the ground each time the player lands.
	 * Used for calculating the scale of the landing animation. */
	UFUNCTION()
	virtual void OnOwningPawnLanded(const FHitResult& Hit);



	// Animations.

public:

	/** Updates the animation data being used by this character. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Animation")
	void UpdateCharacterAnimationData(UCharacterAnimationData* NewAnimationData);

	/** The set of animations that this character reverts to when no item is equipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	const TObjectPtr<UCharacterAnimationData> DefaultAnimationData;

protected:

	/** The set of animations that this character is currently using. This is updated to match the player's currently
	 * equipped item, if they have one. */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	TObjectPtr<UCharacterAnimationData> AnimationData;

	/** This character's current animation data as item animation data, if their current animation data is item
	 * animation data, cached for convenience. This is updated to match the player's currently equipped item, if they
	 * have one. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	TObjectPtr<UItemCharacterAnimationData> ItemAnimationData;



	// Thread-safe animation updating.

protected:

	/** Uses the pawn's current camera rotation to offset animation in its direction. */
	void UpdateAimOffset();

	/** Updates the pawn movement information needed for animation logic. */
	void UpdateMovementVelocity();

	/** Updates the pawn rotation and aim information needed for animation logic. */
	void UpdateAimSpeed();

	/** Calculates values used to apply additive movement sway. */
	void CalculateMovementSway();

	/** Calculates values used to apply additive aim sway. */
	void CalculateAimSway();



	// Utils.

protected:

	/** This animation instance's owning pawn, cached for convenience. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation")
	TObjectPtr<AHeroBase> OwningHero;

	/**
	 * Performs a float spring interpolation using the given values.
	 *
	 * @param SpringCurrent			The current spring interpolation value.
	 * @param SpringTarget			The target spring interpolation value.
	 * @param SpringState			Data for the calculating spring model. Create a unique variable for each spring
	 *								model.
	 * @param SpringData			Data used for adjusting the spring model. Adjust this data to change the behavior
	 *								of the spring.
	 * @return						The resulting spring interpolation value.
	 */
	float UpdateFloatSpringInterp(float SpringCurrent, float SpringTarget, FFloatSpringState& SpringState, UFloatSpringInterpDataAsset* SpringData);



	// Pawn transform values updated each animation cycle to perform animation calculations.

// Movement speed.
protected:

	/** The Z velocity at which the pawn hit the ground the most recent time they landed. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Pawn Transform Values")
	float FallingSpeedBeforeLanding;

	/** How hard the pawn hit the ground the most recent time they landed, normalized with the minimum and maximum hard-landing speed. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Pawn Transform Values")
	float HardLandingScale;

	/** The signed vector length of the pawn's velocity. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Pawn Transform Values")
	float SignedSpeed;

	/** The pawn's signed speed relative to their X-axis. */
	float ForwardBackwardSpeed;

	/** The pawn's signed speed relative to their Y-axis. */
	float RightLeftSpeed;

	/** The pawn's signed speed relative to their Z-axis. */
	float UpDownSpeed;

// Actor/camera rotation.
protected:

	/** The pawn's world rotation. */
	FRotator PawnRotation;

	/** The pawn's camera's world rotation. */
	FRotator CurrentAimRotation;

	/** The pawn's camera's world rotation at the last update. */
	FRotator PreviousAimRotation;

	/** The pawn's camera aim rotation pitch normalized to -1.0 to 1.0. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Pawn Transform Values")
	float PawnNormalizedPitch = 0.0f;

	/** The rate at which the pawn's X-axis aim changed this update in degrees/second. */
	float AimRightLeftSpeed;

	/** The rate at which the pawn's Y-axis aim changed this update in degrees/second. */
	float AimUpDownSpeed;

	/** The maximum speed that aim-sway animations will scale to. This clamps the aiming speed used in animation logic. */
	float MaximumAimSpeed = 500.0f;	



	// Spring interpolation. These values are used to calculate movement and aim sway additives.

// Runtime spring values. These are applied separately for each item using its additive animation data.
protected:
	
	/** The current spring value for the forward/backward movement sway spring. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Spring Models|Current Values")
	float CurrentSpringMoveForwardBackward;

	/** The current spring value for the right/left movement sway spring. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Spring Models|Current Values")
	float CurrentSpringMoveRightLeft;

	/** The current spring value for the up/down aim sway spring. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Spring Models|Current Values")
	float CurrentSpringAimUpDown;

	/** The current spring value for the right/left aim sway spring. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Spring Models|Current Values")
	float CurrentSpringAimRightLeft;

/* Spring states. These are provide additional data for the spring model. These should not be used for anything outside
 * of the spring model calculation. */
protected:

	/** Spring state for the forward/backward movement sway's spring interpolation calculation. */
	FFloatSpringState SpringStateMoveForwardBackward;
	
	/** Spring state for the right/left movement sway's spring interpolation calculation. */
	FFloatSpringState SpringStateMoveRightLeft;

	/** Spring state for the up/down aim sway's spring interpolation calculation. */
	FFloatSpringState SpringStateAimUpDown;

	/** The spring state variable used for the right/left aim sway's spring interpolation calculation. */
	FFloatSpringState SpringStateAimRightLeft;



	// Landing.

protected:

	/** The camera shake to apply when the pawn lands. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Landing")
	TSubclassOf<UCameraShakeBase> HardLandingCameraShake;

	/** The curve that determines the rate at which the hard landing effect scales with falling speed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Landing")
	UCurveFloat* HardLandingEffectCurve;

	/** Linearly scales the weight of the landing dip animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Landing")
	float LandingDipScale = 1.0f;

	/** Linearly scales the effect of the landing camera shake. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Landing")
	float LandingCameraShakeScale = 1.0f;
};
