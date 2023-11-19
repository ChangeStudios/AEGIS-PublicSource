// Copyright Samuel Reitich 2023.


#include "Animation/AnimInstances/Characters/PrototypeAnimInstance.h"

#include "HeroesLogChannels.h"
#include "AbilitySystem/HeroesNativeGameplayTags.h"
#include "AbilitySystem/Components/HeroesAbilitySystemComponent.h"
#include "AbilitySystem/Components/HeroesCharacterMovementComponent.h"
#include "Animation/FloatSpringInterpDataAsset.h"
#include "Animation/CharacterAnimationData/ItemCharacterAnimationData.h"
#include "Camera/CameraComponent.h"
#include "Characters/Heroes/HeroBase.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItemDefinition.h"
#include "Inventory/InventoryItemInstance.h"
#include "Inventory/ItemTraits/EquippableItemTrait.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Player/PlayerStates/Game/HeroesGamePlayerStateBase.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

#define AddRotators(A, B) UKismetMathLibrary::ComposeRotators(A, B);

void UPrototypeAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UPrototypeAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OwningHero = TryGetPawnOwner() ? Cast<AHeroBase>(TryGetPawnOwner()) : nullptr;
	OwningACS = OwningHero && OwningHero->GetAbilitySystemComponent() ? OwningHero->GetHeroesAbilitySystemComponent() : nullptr;

	if (IsValid(OwningHero) && OwningHero != nullptr)
	{
		// OwningHero->LandedDelegate.AddDynamic(this, &UPrototypeAnimInstance::OnOwningPawnLanded);

		OwningPS = OwningHero->GetPlayerState<AHeroesGamePlayerStateBase>();

		OwningInventory = IsValid(OwningPS) ? OwningPS->GetInventoryComponent() : nullptr;

		EquippedItem = IsValid(OwningInventory) ? OwningInventory->GetEquippedItem() : nullptr;

		EquippedItemDefinition = IsValid(EquippedItem) ? const_cast<UInventoryItemDefinition*>(EquippedItem->GetItemDefinition()) : nullptr;

		EquippedItemEquippableTrait = IsValid(EquippedItemDefinition) ? EquippedItemDefinition->FindTraitByClass<UEquippableItemTrait>() : nullptr;
	}

	CrouchingTag = FHeroesNativeGameplayTags::Get().State_Movement_Crouching;
	AimingTag = FHeroesNativeGameplayTags::Get().State_AimedDownSights;
}

void UPrototypeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(OwningHero))
	{
		UpdateLookSpeed();

		UpdateVelocity();

		UpdateCameraPitch();

		UpdateHandIK();

		UpdateCamRotation();

		UpdateLagLeanSway();

		UpdateIKRot();

		UpdateIKRot_ADS();

		UpdateIKLoc();

		UpdateIKLoc_ADS();

		// UpdateFireJitter();

		// UpdateFirePullback();

		// UpdateRapidFire();

		UpdateTagStates();
	}
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

	OutCurrentSpring = UKismetMathLibrary::FloatSpringInterp(
		SpringCurrent,
		LocalScaledSpringTarget,
		SpringState,
		LocalStiffness,
		LocalDampingFactor,
		LocalScaledDeltaTime,
		LocalMass
		);
	OutCurrentFInterp = LocalCurrentFInterp;
}

void UPrototypeAnimInstance::UpdateVelocity()
{
	if (!IsValid(OwningHero))
	{
		return;
	}

	const FVector Velocity = OwningHero->GetVelocity();
	const FVector UnrotatedVelocity = OwningHero->GetActorRotation().UnrotateVector(Velocity);

	SignedSpeed = Velocity.Length();

	ForwardBackwardMovementSpeed = UnrotatedVelocity.X;
	RightLeftMovementSpeed = UnrotatedVelocity.Y;
	UpDownMovementSpeed = UnrotatedVelocity.Z;
}

void UPrototypeAnimInstance::UpdateLookSpeed()
{
	if (!IsValid(OwningHero))
	{
		return;
	}

	PawnRotation = OwningHero->GetActorRotation();

	PreviousLookRotation = CurrentLookRotation;
	CurrentLookRotation = OwningHero->GetFirstPersonCameraComponent()->GetComponentRotation();

	const FVector CurrentLookAsVector = FVector(CurrentLookRotation.Roll, CurrentLookRotation.Pitch, CurrentLookRotation.Yaw);
	const FVector PreviousLookAsVector = FVector(PreviousLookRotation.Roll, PreviousLookRotation.Pitch, PreviousLookRotation.Yaw);

	const FVector RotationSinceLastUpdate = CurrentLookAsVector - PreviousLookAsVector;

	const float DeltaSeconds = GetDeltaSeconds();
	const float TimeSinceLastUpdate = DeltaSeconds > 0.0f ? DeltaSeconds : 1.0f;

	const FVector RotationSpeed = RotationSinceLastUpdate * (1.0f / TimeSinceLastUpdate);

	LookUpDownSpeed = FMath::Clamp(RotationSpeed.Y, -MaxLookSpeed, MaxLookSpeed);
	LookRightLeftSpeed = FMath::Clamp(RotationSpeed.Z, -MaxLookSpeed, MaxLookSpeed);
}

void UPrototypeAnimInstance::UpdateCameraPitch()
{
	OwningHero->GetFirstPersonCameraComponent()->GetCameraView(GetDeltaSeconds(), PlayerCameraView);

	const float PitchOrig = PlayerCameraView.Rotation.Pitch;
	CameraPitch = PitchOrig > 90.0f ?
		FMath::GetMappedRangeValueClamped(FVector2D(270.0f, 360.0f), FVector2D(-90.0f, 0.0f), PitchOrig) :
		PitchOrig;
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
			
				- J: Vector 1
					- X: 0.0
					- Y: Current weapon's ADS distance from camera (static data for each weapon that determines how far to put it from the camera)
					- Z: 0.0
					
				- K: Vector 2 (might be optional - use if there are bugs): The camera's location relative to the character's head
				
				- L: Vector 3
					- I: Vector 1 - Vector 2
					
						- G: Vector 1:
							- E: Vector 1 - Vector 2

								- A: Vector 1
									- The location of the socket that the weapon is attached to, transformed to bone space
										- Position and rotation are taken from the socket transform of the equipped weapon's root (in world space)

								- B: Vector 2 (might be optional - use if there are bugs)
									- The location of the equipped weapon's root (in component space)

						- H: Vector 2:
							- F: Rotated Vector

								- C: Vector: The location of the socket that the weapon is attached to, transformed to bone space using the location and rotation of the equipped weapon's sight mesh's crosshair socket (in world space)

								- D: Rotator (might be optional - use if there are bugs): The rotation of the FPP mesh's head, or the player camera, transformed to bone space using the transform of the socket that the weapon is attached to (in world space)
		- Rotation
			- The rotation of the weapon's root, transformed to bone space using the transform of the crosshair socket of the equipped weapon's sights mesh (in world space), and inverted
	*/

	if (!IsValid(EquippedItemEquippableTrait))
	{
		if (IsValid(OwningHero))
		{
			EquippedItem = IsValid(OwningInventory) ? OwningInventory->GetEquippedItem() : nullptr;

			EquippedItemDefinition = IsValid(EquippedItem) ? const_cast<UInventoryItemDefinition*>(EquippedItem->GetItemDefinition()) : nullptr;

			EquippedItemEquippableTrait = IsValid(EquippedItemDefinition) ? EquippedItemDefinition->FindTraitByClass<UEquippableItemTrait>() : nullptr;
		}

		return FTransform();
	}
	
	FName WeaponRootBone = "root";
	FName AttachSocket = "ik_hand_gun";
	USkeletalMeshComponent* CharacterMesh = GetSkelMeshComponent();
	USkeletalMeshComponent* WeaponMesh = EquippedItemEquippableTrait->FirstPersonEquippedActor->FindComponentByClass<USkeletalMeshComponent>();

	const FTransform AttachedSocketTransform = WeaponMesh->GetSocketTransform(WeaponRootBone, RTS_World);
	FVector ALoc = FVector();
	FRotator ARot = FRotator();
	
	GetSkelMeshComponent()->TransformToBoneSpace(
		AttachSocket,
		AttachedSocketTransform.GetLocation(),
		AttachedSocketTransform.Rotator(),
		ALoc,
		ARot
		);

	const FTransform AttachedSocketTransformComponent = WeaponMesh->GetSocketTransform(WeaponRootBone, RTS_Component);
	const FVector BLoc = AttachedSocketTransformComponent.GetLocation();

	const FVector E = ALoc - BLoc;


	TArray<UActorComponent*> SightComponents = EquippedItemEquippableTrait->FirstPersonEquippedActor->GetComponentsByTag(UMeshComponent::StaticClass(), FName("Sight"));

	UMeshComponent* SightMesh = SightComponents.Num() > 0 ? Cast<UMeshComponent>(SightComponents[0]) : nullptr;

	if (!IsValid(SightMesh))
	{
		return FTransform();
	}

	const FTransform SightTransform = SightMesh->GetSocketTransform(FName("Socket"), RTS_World);

	FVector CLoc = FVector();
	FRotator CRot = FRotator();
	CharacterMesh->TransformToBoneSpace(
		AttachSocket,
		SightTransform.GetLocation(),
		SightTransform.Rotator(),
		CLoc,
		CRot
		);


	const FTransform HandSocketTransform = CharacterMesh->GetSocketTransform(FName(AttachSocket, RTS_World));

	FVector DLoc = FVector();
	FRotator DRot = FRotator();
	GetSkelMeshComponent()->TransformToBoneSpace(
		AttachSocket,
		HandSocketTransform.GetLocation(),
		HandSocketTransform.Rotator(),
		DLoc,
		DRot
		);

	const FVector F = DRot.RotateVector(CLoc);


	const FVector I = E - F;


	const FVector J = FVector(0.0f, 30.0f, 0.0f);


	const FVector FinalLocation = I + J;



	FVector RotLoc = FVector();
	FRotator RotRot = FRotator();
	WeaponMesh->TransformToBoneSpace(
		WeaponRootBone,
		SightTransform.GetLocation(),
		SightTransform.Rotator(),
		RotLoc,
		RotRot
		);

	FRotator FinalRotation = RotRot.GetInverse();



	return FTransform(FinalRotation, FinalLocation, FVector(1.0f));
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

	if (!IsValid(EquippedItemEquippableTrait) || !IsValid(EquippedItemEquippableTrait))
	{
		if (IsValid(OwningHero))
		{
			EquippedItem = IsValid(OwningInventory) ? OwningInventory->GetEquippedItem() : nullptr;

			EquippedItemDefinition = IsValid(EquippedItem) ? const_cast<UInventoryItemDefinition*>(EquippedItem->GetItemDefinition()) : nullptr;

			EquippedItemEquippableTrait = IsValid(EquippedItemDefinition) ? EquippedItemDefinition->FindTraitByClass<UEquippableItemTrait>() : nullptr;
		}

		return FTransform();
	}

	const FTransform RelativeView = OwningHero->GetFirstPersonCameraComponent()->GetRelativeTransform();
	const FVector Loc1 = RelativeView.Rotator().GetInverse().RotateVector(RelativeView.GetLocation());
	const FVector Loc2 = ItemAnimationData->RightHandPoseCorrectionOffset.GetLocation();
	const FVector FinalLoc = Loc1 + Loc2;

	const FName WeaponRootBone = "root";
	const FName AttachSocket = "ik_hand_gun";
	const USkeletalMeshComponent* CharacterMesh = GetSkelMeshComponent();
	const USkeletalMeshComponent* WeaponMesh = EquippedItemEquippableTrait->FirstPersonEquippedActor->FindComponentByClass<USkeletalMeshComponent>();

	// Might have to be bone transform, not socket
	const FTransform SocketTransform = CharacterMesh->GetSocketTransform(AttachSocket, RTS_World);
	FVector OutLoc;
	FRotator OutRot;

	CharacterMesh->TransformToBoneSpace(
		AttachSocket,
		SocketTransform.GetLocation(),
		SocketTransform.Rotator(),
		OutLoc,
		OutRot
		);
	
	return FTransform(FRotator(0.0f), FinalLoc, FVector(OutRot.Roll, OutRot.Pitch, OutRot.Yaw));
}

void UPrototypeAnimInstance::UpdateCamRotation()
{
	PreviousYaw = CurrentYaw;
	CurrentYaw = PlayerCameraView.Rotation.Yaw;
	YawDelta = PreviousYaw - CurrentYaw;
}

void UPrototypeAnimInstance::UpdateLagLeanSway()
{
	if (!IsValid(OwningHero) || !IsValid(ItemAnimationData))
	{
		return;
	}
	
	NormalizedCameraPitch = FMath::GetMappedRangeValueClamped(FVector2D(-75.0f, 75.0f), FVector2D(1.0f, -1.0f), CameraPitch);

	const FRotator DeltaRotation = CurrentLookRotation - PreviousLookRotation; 

	const float MoveForwardBackwardOrig = ForwardBackwardMovementSpeed;
	const float MoveRightLeftOrig = RightLeftMovementSpeed;
	const float MoveForwardBackwardNormalized = MoveForwardBackwardOrig;
	const float MoveRightLeftNormalized = MoveRightLeftOrig;

	const float LookUpDownOrig = DeltaRotation.Pitch;
	const float LookRightLeftOrig = DeltaRotation.Yaw;
	const float LookUpDownNormalized = LookUpDownOrig;
	const float LookRightLeftNormalized = LookRightLeftOrig;

	// Hook in aim scaling rates here.
	LookUpDownRate = 1.0f;
	LookRightLeftRate = 1.0f;

	UpdateFloatSpringInterp(CurrentFInterpLookRightLeft, (LookRightLeftNormalized + LookRightLeftRate), CurrentSpringLookRightLeft, LookRightLeftSpringState, ItemAnimationData->SpringInterpDataAimRightLeft, true, CurrentFInterpLookRightLeft, CurrentSpringLookRightLeft);
	UpdateFloatSpringInterp(CurrentFInterpLookUpDown, (LookUpDownNormalized + LookUpDownRate), CurrentSpringLookUpDown, LookUpDownSpringState, ItemAnimationData->SpringInterpDataAimUpDown, true, CurrentFInterpLookUpDown, CurrentSpringLookUpDown);
	UpdateFloatSpringInterp(CurrentFInterpMoveForwardBackward, MoveForwardBackwardNormalized, CurrentSpringMoveForwardBackward, MoveForwardBackwardSpringState, ItemAnimationData->SpringInterpDataMoveForwardBackward, false, CurrentFInterpMoveForwardBackward, CurrentSpringMoveForwardBackward);
	UpdateFloatSpringInterp(CurrentFInterpMoveRightLeft, MoveRightLeftNormalized, CurrentSpringMoveRightLeft, MoveRightLeftSpringState, ItemAnimationData->SpringInterpDataMoveRightLeft, false, CurrentFInterpMoveRightLeft, CurrentSpringMoveRightLeft);
}

void UPrototypeAnimInstance::OnOwningPawnLanded(const FHitResult& Hit)
{
	UpdateFall(Hit);
}

void UPrototypeAnimInstance::UpdateFall(const FHitResult& Hit)
{
	if (IsValid(OwningHero) && OwningHero->GetHeroesCharacterMovementComponent() && IsValid(ItemAnimationData))
	{
		const UHeroesCharacterMovementComponent* MovementComponent = OwningHero->GetHeroesCharacterMovementComponent();

		LandingSpeed = FMath::Abs(MovementComponent->GetLastUpdateVelocity().Z);
		const float GravityForce = UPhysicsSettings::Get()->DefaultGravityZ * MovementComponent->GetGravityScale();
		const float FallingTime = LandingSpeed / GravityForce;
		FallDistance = 0.5f * GravityForce * FMath::Pow(FallingTime, 2.0f);
		NormalizedFallDistance = FMath::GetMappedRangeValueClamped(
			FVector2D(ItemAnimationData->MinFallDistance, ItemAnimationData->MaxFallDistance),
			FVector2D(0.0f, 1.0f),
			FallDistance);
	}
}

void UPrototypeAnimInstance::UpdateIKRot()
{
	const float Roll = CurrentSpringLookUpDown * SwayLookUpDownScale;
	const float Pitch = CurrentSpringMoveRightLeft * LeanMoveRightLeftScale;
	const float Yaw = CurrentSpringLookRightLeft * SwayLookRightLeftScale;
	const FRotator SwayRot = FRotator(Pitch, Yaw, Roll);
	
	const FRotator SwayAndFireRot = AddRotators(SwayRot, CurrentFireJitterRotation);
	const FRotator FinalIKRot = SwayAndFireRot * FinalHipRotationScale;

	HandIKRotation = AddRotators(FinalIKRot, HandIKCorrection.Rotator());
}

void UPrototypeAnimInstance::UpdateIKRot_ADS()
{
	const float Roll = CurrentSpringLookUpDown * ADSSwayLookUpDownScale;
	const float Pitch = CurrentSpringMoveRightLeft * ADSLeanMoveRightLeftScale;
	const float Yaw = CurrentSpringLookRightLeft * ADSSwayLookRightLeftScale;
	const FRotator SwayRot = FRotator(Pitch, Yaw, Roll);
	
	const FRotator SwayAndFireRot = AddRotators(SwayRot, CurrentFireJitterRotation);
	const FRotator FinalIKRot = SwayAndFireRot * ADSFinalRotationScale;

	ADSHandIKRotation = AddRotators(FinalIKRot, HandADSIK.Rotator());
}

void UPrototypeAnimInstance::UpdateIKLoc()
{
	const float A = CurrentSpringMoveRightLeft * LagMoveRightLeftScale;
	const float B = CurrentSpringLookRightLeft * LagLookRightLeftScale;
	const float X = A + B;

	const float C = CurrentSpringMoveForwardBackward * LagMoveForwardBackwardScale;
	const float Y = C + CurrentFirePullback;

	const float Z = WeaponSwayLocationOffsetInterpLookUp * WeaponSwayLocationOffsetLookUpScale;

	const FVector FinalLoc = FVector(X, Y, Z) * FinalHipLocationScale;
	const FVector CorrectedFinalLoc = FinalLoc + HandIKCorrection.GetLocation();

	const FVector Scale = HandIKCorrection.GetScale3D();
	const FRotator ScaleAsRot = FRotator(Scale.Y, Scale.Z, Scale.X);
	
	HandIKLocation = ScaleAsRot.RotateVector(CorrectedFinalLoc);
}

void UPrototypeAnimInstance::UpdateIKLoc_ADS()
{
	const float A = CurrentSpringMoveForwardBackward * ADSLagMoveForwardBackwardScale;
	const float B = CurrentFirePullback * bAiming ? 0.75f : 0.3f;
	const float Y = A + B;

	const float C = CurrentSpringMoveRightLeft * ADSLagMoveRightLeftScale;
	const float D = CurrentSpringLookRightLeft * ADSLagLookRightLeftScale;
	const float Z = (C + D) * -1.0f;

	const FVector FinalLoc = FVector(0.0f, Y, Z) * ADSFinalLocationScale;

	ADSHandIKLocation = FinalLoc + HandADSIK.GetLocation();
}

void UPrototypeAnimInstance::UpdateFireJitter()
{
	if (!IsValid(ItemAnimationData))
	{
		return;
	}
	
	const float DeltaTime = GetDeltaSeconds();
	
	if (DeltaTime > 0.1f)
	{
		return;
	}

	CurrentRInterpFireJitterRotation = FMath::RInterpTo(CurrentRInterpFireJitterRotation, TargetRInterpFireJitterRotation, DeltaTime, 15.0f);
	CurrentFireJitterTime = FMath::FInterpTo(CurrentFireJitterTime, 1.0f, DeltaTime, ItemAnimationData->FireInterpSpeed);

	const FVector CurrentVector = FVector(CurrentFireJitterRotation.Roll, CurrentFireJitterRotation.Pitch, CurrentFireJitterRotation.Yaw);
	const FRotator TargetVectorAsRot = CurrentRInterpFireJitterRotation * ItemAnimationData->FireJitterInterpCurve->GetFloatValue(CurrentFireJitterTime);
	const FVector TargetVector = FVector(TargetVectorAsRot.Roll, TargetVectorAsRot.Pitch, TargetVectorAsRot.Yaw);
	const FVector InterpVector = UKismetMathLibrary::VectorSpringInterp
	(
		CurrentVector,
		TargetVector,
		FireJitterSpringState,
		FireJitterData.Stiffness,
		FireJitterData.CriticalDampingFactor,
		DeltaTime
	);

	const FRotator InterpAsRot = FRotator(InterpVector.X, InterpVector.Y, InterpVector.Z);
	CurrentFireJitterRotation = bAiming ? InterpAsRot * 0.8f : InterpAsRot;
}

void UPrototypeAnimInstance::UpdateFirePullback()
{
	if (!bResettingAim)
	{
		return;
	}

	float CurrentFirePullbackUnclamped = 1.0f;

	UpdateFloatSpringInterp(CurrentFInterpFirePullback, 0.0f, CurrentFirePullback, PullbackSpringState, FirePullbackData, true, CurrentFInterpFirePullback, CurrentFirePullbackUnclamped);
	CurrentFirePullback = FMath::Clamp(CurrentFirePullbackUnclamped, -6.0f, 1.0f);
}

void UPrototypeAnimInstance::UpdateRapidFire()
{
	if (RapidFireAlpha > 0.0f)
	{
		RapidFireAlpha = FMath::FInterpConstantTo(RapidFireAlpha, 0.0f, GetDeltaSeconds(), RapidFireAlphaInterpSpeed);
	}
}

void UPrototypeAnimInstance::UpdateTagStates()
{
	if (!IsValid(OwningHero) || !IsValid(OwningACS))
	{
		return;
	}

	bCrouched = OwningACS->HasMatchingGameplayTag(CrouchingTag);
	bAiming = OwningACS->HasMatchingGameplayTag(AimingTag);
}

void UPrototypeAnimInstance::OnWeaponFire()
{
}
