// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterAnimationData.generated.h"

/**
 * Collection of animation data used by character animation systems to dynamically change animation sets during
 * runtime. This is the base class for animation data, which is extended for more specific implementations (e.g. items,
 * weapons, etc.).
 *
 * Each character state with a unique animation set should only have one animation data asset, which should be an
 * instance of the most specific class possible. For example, weapons should only have one data asset, of the
 * UWeaponCharacterAnimationData class. They should not have data assets of the UItemCharacterAnimationData or
 * UCharacterAnimationData classes.
 *
 * Currently, the only application of this class, as opposed to its children, is unarmed animation data.
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UCharacterAnimationData : public UDataAsset
{
	GENERATED_BODY()

	// First-person.

// Base poses. Additive animations are applied on top of these so they can be re-used for different states (e.g. standing, crouching, etc.).
public:

	/** Base pose when standing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Base Poses", DisplayName = "Standing Pose (First-Person)")
	TObjectPtr<UAnimSequenceBase> StandingPose_FPP = nullptr;

	/** Base pose when crouching. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Base Poses", DisplayName = "Crouching Pose (First-Person)")
	TObjectPtr<UAnimSequenceBase> CrouchingPose_FPP = nullptr;

// Additive poses. These are applied onto base poses before animations.
public:

	/** Additive poses applied to offset the character's base pose based on their camera pitch. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Additive Poses", DisplayName = "Pitch Offset Poses (First-Person)")
	TObjectPtr<UBlendSpace1D> PitchOffsetBS_FPP = nullptr;

// Locomotion.
public:

	/** Idle animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion", DisplayName = "Idle (First-Person)")
	TObjectPtr<UAnimSequenceBase> Idle_FPP = nullptr;

	/** Walking and running animations. This includes the idle animation when speed is 0. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion", DisplayName = "Walking (First-Person)")
	TObjectPtr<UBlendSpace1D> WalkRunBS_FPP = nullptr;

	/** Jump animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion", DisplayName = "Jump (First-Person)")
	TObjectPtr<UAnimSequenceBase> Jump_FPP = nullptr;

	/** Falling animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion", DisplayName = "Falling (First-Person)")
	TObjectPtr<UAnimSequenceBase> Falling_FPP = nullptr;

	/** Procedural landing animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion", DisplayName = "Land (First-Person)")
	TObjectPtr<UAnimSequenceBase> Landing_FPP = nullptr;



// Deprecated.
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Locomotion", DisplayName = "Walking (First-Person)", meta=(DeprecatedProperty))
	UBlendSpace* WalkingBS_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Jump", DisplayName = "Jump (First-Person)", meta=(DeprecatedProperty))
	UAnimSequenceBase* JumpAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Jump", DisplayName = "Falling (First-Person)", meta=(DeprecatedProperty))
	UAnimSequenceBase* FallingAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Jump", DisplayName = "Landing (First-Person)", meta=(DeprecatedProperty))
	UAnimSequenceBase* LandingAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Locomotion", DisplayName = "Standing (Third-Person, Lower)", meta=(DeprecatedProperty))
	UAnimSequenceBase* StandingAnim_LowerTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Locomotion", DisplayName = "Idle (Third-Person, Upper)", meta=(DeprecatedProperty))
	UAnimSequenceBase* IdleAnim_UpperTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Locomotion", DisplayName = "Walking (Third-Person, Lower)", meta=(DeprecatedProperty))
	UBlendSpace* WalkingBS_TPP_LowerTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Locomotion", DisplayName = "Crouching (Third-Person, Lower)", meta=(DeprecatedProperty))
	UAnimSequenceBase* CrouchingAnim_LowerTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Locomotion", DisplayName = "Crouch-Walking (Third-Person, Lower)", meta=(DeprecatedProperty))
	UBlendSpace* CrouchWalkingBS_LowerTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Locomotion", DisplayName = "Slow-Walking (Third-Person, Lower)", meta=(DeprecatedProperty))
	UBlendSpace* SlowWalkingBS_LowerTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Jump", DisplayName = "Jump (Third-Person, Lower)", meta=(DeprecatedProperty))
	UAnimSequenceBase* JumpAnim_LowerTPP = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Jump", DisplayName = "Falling (Third-Person, Lower)", meta=(DeprecatedProperty))
	UAnimSequenceBase* FallingAnim_LowerTPP = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Jump", DisplayName = "Landing (Third-Person, Lower)", meta=(DeprecatedProperty))
	UAnimSequenceBase* LandingAnim_LowerTPP = nullptr;
};
