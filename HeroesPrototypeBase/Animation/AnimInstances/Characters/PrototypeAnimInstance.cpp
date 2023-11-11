// Copyright Samuel Reitich 2023.


#include "Animation/AnimInstances/Characters/PrototypeAnimInstance.h"

#include "HeroesLogChannels.h"
#include "Animation/FloatSpringInterpDataAsset.h"
#include "Animation/CharacterAnimationData/ItemCharacterAnimationData.h"
#include "Camera/CameraComponent.h"
#include "Characters/Heroes/HeroBase.h"

void UPrototypeAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	APawn* Pawn = TryGetPawnOwner();
	OwningHero = IsValid(Pawn) ? Cast<AHeroBase>(Pawn) : nullptr;
}

void UPrototypeAnimInstance::UpdateFloatSpringInterp(float FInterpCurrent, float FInterpTarget, float SpringCurrent, FFloatSpringState& SpringState, UFloatSpringInterpDataAsset* SpringData, bool bUseDeltaScalar, float& OutCurrentFInterp, float& OutCurrentSpring)
{
	const float DeltaSeconds = GetDeltaSeconds() > 0.0f ? GetDeltaSeconds() : 1.0f;

	if (DeltaSeconds > 0.1f)
	{
		OutCurrentFInterp = FInterpCurrent;
		OutCurrentSpring = SpringCurrent;
		return;
	}

	const float LocalCurrentFInterp = 0.0f;
	const float LocalFInterpSpeed = SpringData->InterpSpeed;
	const float LocalStiffness = SpringData->Stiffness;
	const float LocalDampingFactor = SpringData->CriticalDampingFactor;
	const float LocalMass = SpringData->Mass;

	const float LocalDeltaScalar = (1.0f / 60.0f) / DeltaSeconds;

	const float LocalScaledSpringTarget = bUseDeltaScalar ? FInterpTarget * FMath::Pow(LocalDeltaScalar, 0.75f) : FInterpTarget;
	const float LocalScaledDeltaTime = DeltaSeconds / FMath::Pow(LocalDeltaScalar, -0.1f);

	OutCurrentSpring = UKismetMathLibrary::FloatSpringInterp(SpringCurrent, LocalScaledSpringTarget, SpringState, LocalStiffness, LocalDampingFactor, LocalScaledDeltaTime, LocalMass);
	OutCurrentFInterp = LocalCurrentFInterp;
}

void UPrototypeAnimInstance::UpdateCameraPitch()
{
	OwningHero->GetFirstPersonCameraComponent()->GetCameraView(GetDeltaSeconds(), PlayerCameraView);

	const float PitchOrig = PlayerCameraView.Rotation.Pitch;
	CameraPitch = PitchOrig > 90.0f ?
		FMath::GetMappedRangeValueClamped(FVector2D(270.0f, 360.0f), FVector2D(-90.0f, 0.0f), PitchOrig) :
		PitchOrig;
}

void UPrototypeAnimInstance::UpdateVelocity()
{
	if (!IsValid(OwningHero))
	{
		return;
	}

	const FVector Velocity = OwningHero->GetVelocity();
	const FRotator Rotation = OwningHero->GetActorRotation();

	Speed = Velocity.Length();
	
	const FVector UnrotatedVelocity = Rotation.UnrotateVector(Velocity);

	ForwardBackwardSpeed = UnrotatedVelocity.X;
	RightLeftSpeed = UnrotatedVelocity.Y;
}

void UPrototypeAnimInstance::UpdateHandIK()
{
	HandADSIK = CalculateHandADSOffset();
	HandIKCorrection = CalculateHandCorrectionOffset();

	HandsAdditiveStrength = bAiming ? 1.0f : GetCurveValue(FName("HandsAdditiveStrength"));
}

FTransform UPrototypeAnimInstance::CalculateHandADSOffset()
{
	/*
		Make Transform:

		- Location
			- Add 3 vectors
			
				- Vector 1
					- X: 0.0
					- Y: Current weapon's ADS distance from camera (static data for each weapon that determines how far to put it from the camera)
					- Z: 0.0
					
				- Vector 2 (might be optional - use if there are bugs): The camera's location relative to the character's head
				
				- Vector 3
					- Vector 1 - Vector 2
					
						- Vector 1:
							- Vector 1 - Vector 2

								- Vector 1
									- The location of the socket that the weapon is attached to, transformed to bone space
										- Position and rotation are taken from the socket transform of the equipped weapon's root (in world space)

								- Vector 2 (might be optional - use if there are bugs)
									- The location of the equipped weapon's root (in component space)

						- Vector 2:
							- Rotated Vector

								- Vector: The location of the socket that the weapon is attached to, transformed to bone space using the location and rotation of the equipped weapon's sight mesh's crosshair socket (in world space)

								- Rotator (might be optional - use if there are bugs): The rotation of the FPP mesh's head, or the player camera, transformed to bone space using the transform of the socket that the weapon is attached to (in world space)

		- Rotation
			- The rotation of the weapon's root, transformed to bone space using the transform of the crosshair socket of the equipped weapon's sights mesh (in world space), and inverted
	*/

	return FTransform();
}

FTransform UPrototypeAnimInstance::CalculateHandCorrectionOffset()
{
	/*
		Note: I don't even know what this transform is frickin used for


		Make transform:

		- Location
			- Vector 1 + Vector 2

				- Vector 1: Rotate player FPP camera/view location by its inverted rotation

				- Vector 2: Add the equipped weapon's right hand pose correction offset location

		- Scale
			- Make Scale

				- X: X (Roll)
				- Y; Y (Pitch)
				- Z; Z (Yaw) ... from:

					- The rotation of the socket that the equipped weapon is attached to, transformed to bone space with the transform of the socket (in world space)

	 */

	return FTransform();
}

void UPrototypeAnimInstance::UpdateCamRotation()
{
	PreviousYaw = CurrentYaw;
	CurrentYaw = PlayerCameraView.Rotation.Yaw;
	YawDelta = PreviousYaw - CurrentYaw;
}

void UPrototypeAnimInstance::UpdateLagLeanSway()
{
	if (!IsValid(OwningHero))
	{
		return;
	}
	
	NormalizedCameraPitch = FMath::GetMappedRangeValueClamped(FVector2D(-75.0f, 75.0f), FVector2D(1.0f, -1.0f), CameraPitch);

	// Might need to unrotate this.
	const FVector CurrentMovement = OwningHero->Internal_GetPendingMovementInputVector();

	// Might need to change how this is retrieved in order to get the rotation change this frame.
	const APlayerController* PC = OwningHero->GetController<APlayerController>();
	const FRotator CurrentRotation = IsValid(PC) ? OwningHero->GetController<APlayerController>()->RotationInput : FRotator::ZeroRotator;
	
	MoveForwardBackward = CurrentMovement.X;
	MoveRightLeft = CurrentMovement.Y;
	LookUpDown = CurrentRotation.Pitch;
	LookRightLeft = CurrentRotation.Yaw;

	UE_LOG(LogHeroes, Warning, TEXT("Rotation: %s"), *CurrentRotation.ToString());

	// Hook in aim scaling rates here.
	LookUpDownRate = 1.0f;
	LookRightLeftRate = 1.0f;

	UpdateFloatSpringInterp(CurrentFInterpLookRightLeft, (LookRightLeft + LookRightLeftRate), CurrentSpringLookRightLeft, LookRightLeftSpringState, ItemAnimationData->SpringInterpDataAimRightLeft, true, CurrentFInterpLookRightLeft, CurrentSpringLookRightLeft);
	UpdateFloatSpringInterp(CurrentFInterpLookUpDown, (LookUpDown + LookUpDownRate), CurrentSpringLookUpDown, LookUpDownSpringState, ItemAnimationData->SpringInterpDataAimUpDown, true, CurrentFInterpLookUpDown, CurrentSpringLookUpDown);
	UpdateFloatSpringInterp(CurrentFInterpMoveForwardBackward, MoveForwardBackward, CurrentSpringMoveForwardBackward, MoveForwardBackwardSpringState, ItemAnimationData->SpringInterpDataMoveForwardBackward, false, CurrentFInterpMoveForwardBackward, CurrentSpringMoveForwardBackward);
	UpdateFloatSpringInterp(CurrentFInterpMoveRightLeft, MoveRightLeft, CurrentSpringMoveRightLeft, MoveRightLeftSpringState, ItemAnimationData->SpringInterpDataMoveRightLeft, false, CurrentFInterpMoveRightLeft, CurrentSpringMoveRightLeft);
}

void UPrototypeAnimInstance::UpdateFall()
{
}

void UPrototypeAnimInstance::UpdateIKRot()
{
}

void UPrototypeAnimInstance::UpdateIKRot_ADS()
{
}

void UPrototypeAnimInstance::UpdateIKLoc()
{
}

void UPrototypeAnimInstance::UpdateIKLoc_ADS()
{
}

void UPrototypeAnimInstance::UpdateFireJitter()
{
}

void UPrototypeAnimInstance::UpdateFirePullback()
{
}

void UPrototypeAnimInstance::UpdateRapidFire()
{
}

void UPrototypeAnimInstance::UpdateTagStates()
{
}

void UPrototypeAnimInstance::OnWeaponFire()
{
}
