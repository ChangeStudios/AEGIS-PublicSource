// Copyright Samuel Reitich 2023.


#include "Animation/AnimInstances/Characters/HeroFirstPersonAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Components/HeroesCharacterMovementComponent.h"
#include "Animation/CharacterAnimationData/ItemCharacterAnimationData.h"
#include "Animation/FloatSpringInterpDataAsset.h"
#include "Camera/CameraComponent.h"
#include "Characters/Heroes/HeroBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/HeroesNativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

void UHeroFirstPersonAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Utilize multi-threading to update animations.
	bUseMultiThreadedAnimationUpdate = true;
}

void UHeroFirstPersonAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// Cache the owning pawn and its ACS for convenience.
	OwningHero = TryGetPawnOwner() ? Cast<AHeroBase>(TryGetPawnOwner()) : nullptr;

	OwningACS = OwningHero && OwningHero->GetAbilitySystemComponent() ? OwningHero->GetAbilitySystemComponent() : nullptr;

	// Initialize the current animation data with the default animation data.
	AnimationData = DefaultAnimationData;

	// Initialize the current item animation data with the default animation data, if it is item data.
	ItemAnimationData = Cast<UItemCharacterAnimationData>(DefaultAnimationData);

	if (IsValid(OwningHero))
	{
		// Bind the OnLanded function to whenever the owning pawn lands.
		OwningHero->LandedDelegate.AddDynamic(this, &UHeroFirstPersonAnimInstance::OnOwningPawnLanded);
	}
}

void UHeroFirstPersonAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// Update the current state of ADS.
	bAimingDownSights = OwningACS ? OwningACS->HasMatchingGameplayTag(FHeroesNativeGameplayTags::Get().State_AimedDownSights) : false;

	UpdateAimOffset();

	UpdateMovementVelocity();
	UpdateAimSpeed();

	CalculateMovementSway();
	CalculateAimSway();
}

void UHeroFirstPersonAnimInstance::OnOwningPawnLanded(const FHitResult& Hit)
{
	if (IsValid(OwningHero) && OwningHero->GetHeroesCharacterMovementComponent())
	{
		const UHeroesCharacterMovementComponent* MovementComponent = OwningHero->GetHeroesCharacterMovementComponent();

		// Cache the vertical speed at which the pawn landed.
		FallingSpeedBeforeLanding = FMath::Abs(MovementComponent->GetLastUpdateVelocity().Z);

		// Calculate and cache how hard the pawn hit the ground to scale their camera shake.
		HardLandingScale = MovementComponent->CalculateLandingHardness(FallingSpeedBeforeLanding, HardLandingEffectCurve);

		// Apply the landing camera shake to the landing pawn.
		if (const APlayerController* PC = Cast<APlayerController>(OwningHero->GetController()))
		{
			PC->PlayerCameraManager->StartCameraShake(HardLandingCameraShake, HardLandingScale * LandingCameraShakeScale);
		}
	}
}

void UHeroFirstPersonAnimInstance::UpdateCharacterAnimationData(UCharacterAnimationData* NewAnimationData)
{
	if (NewAnimationData)
	{
		AnimationData = NewAnimationData;

		// If this character's new animation data is for an item, cache it as such.
		if (UItemCharacterAnimationData* NewItemAnimationData = Cast<UItemCharacterAnimationData>(NewAnimationData))
		{
			ItemAnimationData = NewItemAnimationData;
		}
	}
}

void UHeroFirstPersonAnimInstance::UpdateAimOffset()
{
	// Normalize the pawn's camera's current pitch.
	if (IsValid(OwningHero))
	{
		const FRotator CurrentRotation = OwningHero->GetFirstPersonCameraComponent()->GetRelativeRotation();
		FVector RotationAsVector = UKismetMathLibrary::Conv_RotatorToVector(CurrentRotation);
		RotationAsVector.Normalize();
		PawnNormalizedPitch = RotationAsVector.Z;
	}
}

void UHeroFirstPersonAnimInstance::UpdateMovementVelocity()
{
	// Cache the owning pawn's current movement values.
	if (IsValid(OwningHero))
	{
		const FVector PawnVelocity = OwningHero->GetVelocity();
		const FVector UnrotatedVelocity = OwningHero->GetActorRotation().UnrotateVector(PawnVelocity);

		SignedSpeed = PawnVelocity.Length();

		ForwardBackwardSpeed = UnrotatedVelocity.X;
		RightLeftSpeed = UnrotatedVelocity.Y;
		UpDownSpeed = UnrotatedVelocity.Z;
	}
}

void UHeroFirstPersonAnimInstance::UpdateAimSpeed()
{
	// Cache the owning pawn's current rotation and aim values.
	if (IsValid(OwningHero))
	{
		PawnRotation = OwningHero->GetActorRotation();

		PreviousAimRotation = CurrentAimRotation;
		CurrentAimRotation = OwningHero->GetFirstPersonCameraComponent()->GetComponentRotation();

		/* To find the pawn's aim-rate in degrees/second, we use the equation:
		 *		Degrees/1 Second = (Seconds per Frame/1 Second) * (Degrees/Frame)
		 */
		const FVector CurrentAimAsVector = FVector(CurrentAimRotation.Roll, CurrentAimRotation.Pitch, CurrentAimRotation.Yaw);
		const FVector PreviousAimAsVector = FVector(PreviousAimRotation.Roll, PreviousAimRotation.Pitch, PreviousAimRotation.Yaw);

		const FVector RotationSinceLastUpdate = CurrentAimAsVector - PreviousAimAsVector;

		const float DeltaSeconds = GetDeltaSeconds();
		const float TimeSinceLastUpdate = DeltaSeconds > 0.0f ? DeltaSeconds : 1.0f;

		const FVector RotationSpeed =  RotationSinceLastUpdate * (1.0f / TimeSinceLastUpdate);

		/* Clamp the maximum pawn rotation speed used by the animation logic. If we don't do this, there could be
		 * problems if the pawn rotates too fast. For example, aim sway with unclamped speed could cause the player's
		 * mesh to briefly fly off-screen. 500.0 is an arbitrary number that I've found to work well. */
		AimUpDownSpeed = FMath::Clamp(RotationSpeed.Y, -MaximumAimSpeed, MaximumAimSpeed);
		AimRightLeftSpeed = FMath::Clamp(RotationSpeed.Z, -MaximumAimSpeed, MaximumAimSpeed);
	}
}

void UHeroFirstPersonAnimInstance::CalculateMovementSway()
{
	if (OwningHero)
	{
		/* If the player does not have an item equipped (meaning this cast fails), calculating movement sway is not necessary. */
		if (!ItemAnimationData || !ItemAnimationData->SpringInterpDataMoveForwardBackward || !ItemAnimationData->SpringInterpDataMoveRightLeft)
		{
			return;
		}

		// Our spring interpolation target is represented by our pawn's current movement speed's relationship to the pawn's maximum movement speed.
		const float SpringTargetForwardBackward = UKismetMathLibrary::NormalizeToRange(ForwardBackwardSpeed, 0.0f, OwningHero->GetCharacterMovement()->MaxWalkSpeed) * ItemAnimationData->SpringInterpDataMoveForwardBackward->InterpSpeed;
		const float SpringTargetRightLeft = UKismetMathLibrary::NormalizeToRange(RightLeftSpeed, 0.0f, OwningHero->GetCharacterMovement()->MaxWalkSpeed) * ItemAnimationData->SpringInterpDataMoveRightLeft->InterpSpeed;

		// Update the current spring value for movement sway from moving forward and backward.
		CurrentSpringMoveForwardBackward = UpdateFloatSpringInterp
		(
			CurrentSpringMoveForwardBackward,
			SpringTargetForwardBackward,
			SpringStateMoveForwardBackward,
			ItemAnimationData->SpringInterpDataMoveForwardBackward
		);

		// Update the current spring value for movement sway from moving right and left.
		CurrentSpringMoveRightLeft = UpdateFloatSpringInterp
		(
			CurrentSpringMoveRightLeft,
			SpringTargetRightLeft,
			SpringStateMoveRightLeft,
			ItemAnimationData->SpringInterpDataMoveRightLeft
		);
	}
}

void UHeroFirstPersonAnimInstance::CalculateAimSway()
{
	/* If the player does not have an item equipped (meaning this cast fails), calculating movement sway is not necessary. */
	if (!ItemAnimationData || !ItemAnimationData->SpringInterpDataAimUpDown || !ItemAnimationData->SpringInterpDataAimRightLeft)
	{
		return;
	}

	// Our spring interpolation target is represented by our pawn's current aim speed's relationship to the pawn's maximum aim speed, scaled by our interpolation speed.
	const float SpringTargetUpDown = UKismetMathLibrary::NormalizeToRange(AimUpDownSpeed, 0.0f, MaximumAimSpeed) * ItemAnimationData->SpringInterpDataAimUpDown->InterpSpeed;
	const float SpringTargetRightLeft = UKismetMathLibrary::NormalizeToRange(AimRightLeftSpeed, 0.0f, MaximumAimSpeed) * ItemAnimationData->SpringInterpDataAimRightLeft->InterpSpeed;

	// Update the current spring value for aim sway from aiming up and down.
	CurrentSpringAimUpDown = UpdateFloatSpringInterp
	(
		CurrentSpringAimUpDown,
		SpringTargetUpDown,
		SpringStateAimUpDown,
		ItemAnimationData->SpringInterpDataAimUpDown
	);

	// Update the current spring value for aim sway from aiming right and left.
	CurrentSpringAimRightLeft = UpdateFloatSpringInterp
	(
		CurrentSpringAimRightLeft,
		SpringTargetRightLeft,
		SpringStateAimRightLeft,
		ItemAnimationData->SpringInterpDataAimRightLeft
	);
}

float UHeroFirstPersonAnimInstance::UpdateFloatSpringInterp(float SpringCurrent, float SpringTarget, FFloatSpringState& SpringState, UFloatSpringInterpDataAsset* SpringData)
{
	const float DeltaSeconds = GetDeltaSeconds();

	// Don't perform the spring calculation at low frame-rates. I've set 10 FPS arbitrarily.
	if (DeltaSeconds > 0.1f || !SpringData)
	{
		return SpringCurrent;
	}

	/* The stiffness value given to the spring model requires extremely high numbers. We scale it here to make
	 * adjusting the spring data more intuitive. 35.0 is a magic number chosen based on testing. It effectively scales
	 * the stiffness to a range of 0-100. */
	const float EffectiveStiffness = SpringData->Stiffness * 35.0f;

	// Perform the float spring interpolation with the given data.
	return UKismetMathLibrary::FloatSpringInterp
	(
		SpringCurrent,
		SpringTarget,
		SpringState,
		EffectiveStiffness,
		SpringData->CriticalDampingFactor,
		DeltaSeconds,
		SpringData->Mass,
		1.0f,
		false
	);
}

void UHeroFirstPersonAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHeroFirstPersonAnimInstance, AnimationData);
}
