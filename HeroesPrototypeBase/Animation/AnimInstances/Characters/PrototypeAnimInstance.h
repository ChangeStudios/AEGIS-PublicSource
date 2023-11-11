// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "PrototypeAnimInstance.generated.h"

class AHeroBase;
class UItemCharacterAnimationData;
class UFloatSpringInterpDataAsset;

USTRUCT(BlueprintType)
struct FSpringInterpData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float FInterpSpeed;

	UPROPERTY(EditDefaultsOnly)
	float Stiffness;

	UPROPERTY(EditDefaultsOnly)
	float CriticalDampingFactor;

	UPROPERTY(EditDefaultsOnly)
	float Mass;
};

/**
 * 
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UPrototypeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeBeginPlay() override;
	
// Utils
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utils")
	AHeroBase* OwningHero;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utils")
	FMinimalViewInfo PlayerCameraView;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	TObjectPtr<UItemCharacterAnimationData> ItemAnimationData;

private:

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
	void UpdateFloatSpringInterp(float FInterpCurrent, float FInterpTarget, float SpringCurrent, FFloatSpringState& SpringState, UFloatSpringInterpDataAsset* SpringData, bool bUseDeltaScalar, float &OutCurrentFInterp, float &OutCurrentSpring);


// Update functions.
public:

	void UpdateCameraPitch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Camera Pitch")
	float CameraPitch;

	// Normalized to -1.0 to 1.0
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Camera Pitch")
	float NormalizedCameraPitch;



	void UpdateVelocity();

	// Velocity vector length.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Velocity")
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Velocity")
	float ForwardBackwardSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Velocity")
	float RightLeftSpeed;



	void UpdateHandIK();

	FTransform CalculateHandADSOffset();

	FTransform CalculateHandCorrectionOffset();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Hand IK")
	FTransform HandADSIK;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Hand IK")
	FTransform HandIKCorrection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Hand IK")
	float HandsAdditiveStrength;



	void UpdateCamRotation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Camera Rotation")
	float PreviousYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Camera Rotation")
	float CurrentYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Camera Rotation")
	float YawDelta;



	void UpdateLagLeanSway();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Camera")
	float WeaponSwayLocationOffsetInterpLookUp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Movement")
	float MoveForwardBackward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Movement")
	float MoveRightLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Movement")
	float LookUpDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Movement")
	float LookRightLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Movement")
	float LookUpDownRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Movement")
	float LookRightLeftRate;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Look Right/Left Spring")
	float CurrentFInterpLookRightLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Look Right/Left Spring")
	float CurrentSpringLookRightLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Look Right/Left Spring")
	FFloatSpringState LookRightLeftSpringState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Look Right/Left Spring")
	FSpringInterpData LookRightLeftInterpData;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Look Up/Down Spring")
	float CurrentFInterpLookUpDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Look Up/Down Spring")
	float CurrentSpringLookUpDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Look Up/Down Spring")
	FFloatSpringState LookUpDownSpringState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Look Up/Down Spring")
	FSpringInterpData LookUpDownInterpData;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Move Forward/Backward Spring")
	float CurrentFInterpMoveForwardBackward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Move Forward/Backward Spring")
	float CurrentSpringMoveForwardBackward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Move Forward/Backward Spring")
	FFloatSpringState MoveForwardBackwardSpringState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Move Forward/Backward Spring")
	FSpringInterpData MoveForwardBackwardInterpData;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Move Right/Left Spring")
	float CurrentFInterpMoveRightLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Move Right/Left Spring")
	float CurrentSpringMoveRightLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Move Right/Left Spring")
	FFloatSpringState MoveRightLeftSpringState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Move Right/Left Spring")
	FSpringInterpData MoveRightLeftInterpData;


	
	void UpdateFall();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Falling")
	float FallDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Falling")
	float MinFallDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Falling")
	float MaxFallDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Falling")
	float NormalizedFallDistance;



	void UpdateIKRot();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation")
	float LeanMoveForwardBackwardScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation")
	float LeanMoveRightLeftScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation")
	float SwayLookUpDownScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation")
	float SwayLookRightLeftScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation")
	float FinalHipRotationScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation")
	FRotator HandIKRotation;



	void UpdateIKRot_ADS();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	float ADSLeanMoveForwardBackwardScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	float ADSLeanMoveRightLeftScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	float ADSSwayLookUpDownScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	float ADSSwayLookRightLeftScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	float ADSFinalRotationScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	FRotator ADSHandIKRotation;



	void UpdateIKLoc();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float LagMoveForwardBackwardScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float LagMoveRightLeftScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float LagLookUpDownScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float LagLookRightLeftScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float WeaponSwayLocationOffsetLookUpScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float FinalHipLocationScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	FVector HandIKLocation;



	void UpdateIKLoc_ADS();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location (ADS)")
	float ADSLagMoveForwardBackwardScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location (ADS)")
	float ADSLagMoveRightLeftScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location (ADS)")
	float ADSLagLookUpDownScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location (ADS)")
	float ADSLagLookRightLeftScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location (ADS)")
	float ADSFinalLocationScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location (ADS)")
	FVector ADSHandIKLocation;



	void UpdateFireJitter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Jitter Rotation")
	FRotator CurrentRInterpFireJitterRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Jitter Rotation")
	FRotator TargetRInterpFireJitterRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Jitter Rotation")
	float CurrentFireJitterTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Jitter Rotation")
	FRotator CurrentFireJitterRotation;



	void UpdateFirePullback();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Pullback")
	float CurrentFInterpFirePullback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Pullback")
	float CurrentFirePullback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Pullback")
	FFloatSpringState PullbackSpringState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Pullback")
	FSpringInterpData FirePullbackData;



	void UpdateRapidFire();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rapid Fire")
	float RapidFireAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rapid Fire")
	float rapidFireAlphaInterpSpeed;



	void UpdateTagStates();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player States")
	bool bRelaxed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player States")
	bool bCrouched;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player States")
	bool bAiming;



	void OnWeaponFire();

};
