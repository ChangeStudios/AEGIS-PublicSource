// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "PrototypeAnimInstance.generated.h"

class UHeroesAbilitySystemComponent;
class AHeroBase;
class UItemCharacterAnimationData;
class UFloatSpringInterpDataAsset;
class AHeroesGamePlayerStateBase;
class UInventoryComponent;
class UInventoryItemInstance;
class UInventoryItemDefinition;
class UEquippableItemTrait;

USTRUCT(BlueprintType)
struct FSpringInterpData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float FInterpSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float Stiffness = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float CriticalDampingFactor = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float Mass = 1.0f;
};

/**
 * 
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UPrototypeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
// Utils
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utils")
	AHeroBase* OwningHero;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utils")
	UHeroesAbilitySystemComponent* OwningACS = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utils")
	AHeroesGamePlayerStateBase* OwningPS = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utils")
	UInventoryComponent* OwningInventory = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utils")
	UInventoryItemInstance* EquippedItem = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utils")
	UInventoryItemDefinition* EquippedItemDefinition = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utils")
	UEquippableItemTrait* EquippedItemEquippableTrait = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utils")
	FMinimalViewInfo PlayerCameraView;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Data")
	TObjectPtr<UItemCharacterAnimationData> ItemAnimationData = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTag CrouchingTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTag AimingTag;

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

	void UpdateVelocity();

	// Velocity vector length.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Velocity")
	float SignedSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Velocity")
	float ForwardBackwardMovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Velocity")
	float RightLeftMovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Velocity")
	float UpDownMovementSpeed;



	void UpdateLookSpeed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Looking")
	FRotator PawnRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Looking")
	FRotator PreviousLookRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Looking")
	FRotator CurrentLookRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Looking")
	float LookUpDownSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Looking")
	float LookRightLeftSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|Looking")
	float MaxLookSpeed = 500.0f;

	



	void UpdateCameraPitch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Camera Pitch")
	float CameraPitch;

	// Normalized to -1.0 to 1.0
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Camera Pitch")
	float NormalizedCameraPitch;





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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Camera Rotation")
	FRotator PreviousRot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Camera Rotation")
	FRotator CurrentRot;



	void UpdateLagLeanSway();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Lag, Lean, & Sway|Camera")
	float WeaponSwayLocationOffsetInterpLookUp;

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



	virtual void OnOwningPawnLanded(const FHitResult& Hit);

	void UpdateFall(const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Movement|Falling")
	float LandingSpeed;

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
	float LeanMoveForwardBackwardScale = -1.25;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation")
	float LeanMoveRightLeftScale = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation")
	float SwayLookUpDownScale = 0.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation")
	float SwayLookRightLeftScale = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation")
	float FinalHipRotationScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation")
	FRotator HandIKRotation;



	void UpdateIKRot_ADS();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	float ADSLeanMoveForwardBackwardScale = -1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	float ADSLeanMoveRightLeftScale = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	float ADSSwayLookUpDownScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	float ADSSwayLookRightLeftScale = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	float ADSFinalRotationScale = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Rotation (ADS)")
	FRotator ADSHandIKRotation;



	void UpdateIKLoc();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float LagMoveForwardBackwardScale = -1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float LagMoveRightLeftScale = 1.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float LagLookUpDownScale = 0.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float LagLookRightLeftScale = 0.11f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float WeaponSwayLocationOffsetLookUpScale = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	float FinalHipLocationScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location")
	FVector HandIKLocation;



	void UpdateIKLoc_ADS();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location (ADS)")
	float ADSLagMoveForwardBackwardScale = -1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location (ADS)")
	float ADSLagMoveRightLeftScale = 0.7f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location (ADS)")
	float ADSLagLookUpDownScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location (ADS)")
	float ADSLagLookRightLeftScale = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Movement|IK Location (ADS)")
	float ADSFinalLocationScale = 0.3f;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Jitter Rotation")
	FVectorSpringState FireJitterSpringState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Pullback")
	FSpringInterpData FireJitterData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Pullback")
	bool bResettingAim;



	void UpdateFirePullback();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Pullback")
	float CurrentFInterpFirePullback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Pullback")
	float CurrentFirePullback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Pullback")
	FFloatSpringState PullbackSpringState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire Pullback")
	UFloatSpringInterpDataAsset* FirePullbackData;



	void UpdateRapidFire();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rapid Fire")
	float RapidFireAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rapid Fire")
	float RapidFireAlphaInterpSpeed;



	void UpdateTagStates();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player States")
	bool bRelaxed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player States")
	bool bCrouched;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player States")
	bool bAiming;



	void OnWeaponFire();

};
