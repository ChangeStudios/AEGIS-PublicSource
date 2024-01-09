// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Animation/CharacterAnimationData/ItemCharacterAnimationData.h"
#include "WeaponCharacterAnimationData.generated.h"

class UAnimMontage;
class UAnimSequenceBase;

/**
 * Collection of animation data specific to a weapon, used to perform weapon-specific animation on the character
 * currently using the weapon. When a character equips this weapon, it will switch to this animation set.
 *
 * This data class extends UItemCharacterAnimationData with additional functionality exclusive to weapons, such as
 * aiming down sights.
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UWeaponCharacterAnimationData : public UItemCharacterAnimationData
{
	GENERATED_BODY()

	// First-person.

// Extended base poses.
public:

	/** Base pose when aiming. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Base Poses", DisplayName = "Aiming Pose (First-Person)")
	TObjectPtr<UAnimSequenceBase> AimingPose_FPP = nullptr;

// Extended locomotion for aiming.
public:

	/** Aimed idle animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion|Aimed", DisplayName = "Idle, Aiming (First-Person)")
	TObjectPtr<UAnimSequenceBase> Idle_Aimed_FPP = nullptr;

	/** Aimed walking and running animations. This includes the aimed idle animation when speed is 0. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion|Aimed", DisplayName = "Walking, Aiming (First-Person)")
	TObjectPtr<UBlendSpace1D> WalkRunBS_Aimed_FPP = nullptr;

	/** Aimed jump animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion|Aimed", DisplayName = "Jump, Aiming (First-Person)")
	TObjectPtr<UAnimSequenceBase> Jump_Aimed_FPP = nullptr;

	/** Aimed falling animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion|Aimed", DisplayName = "Falling, Aiming (First-Person)")
	TObjectPtr<UAnimSequenceBase> Falling_Aimed_FPP = nullptr;

	/** Procedural aimed landing animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion|Aimed", DisplayName = "Land, Aiming (First-Person)")
	TObjectPtr<UAnimSequenceBase> Landing_Aimed_FPP = nullptr;

// Weapon.
public:

	/** Start aiming. This is an optional animation, as the procedural aiming system will automatically generate a
	 * smooth animation into aiming. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "(Optional) Start Aiming (First-Person)")
	TObjectPtr<UAnimSequenceBase> StartAiming_FPP = nullptr;

	/** Stop aiming. This is an optional animation, as the procedural aiming system will automatically generate a
	 * smooth animation out of aiming. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "(Optional) Stop Aiming (First-Person)")
	TObjectPtr<UAnimSequenceBase> StopAiming_FPP = nullptr;

	/** Reload with at least one remaining bullet. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Reload with a Chambered Round (First-Person)")
	TObjectPtr<UAnimMontage> Reload_Chambered_FPP = nullptr;

	/** Reload with a completely empty magazine. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Reload from an Empty Magazine (First-Person)")
	TObjectPtr<UAnimMontage> Reload_Empty_FPP = nullptr;

	/** Fire from the hip. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Fire (First-Person)")
	TObjectPtr<UAnimMontage> Fire_FPP = nullptr;

	/** Fire while aiming. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Fire, Aiming (First-Person)")
	TObjectPtr<UAnimMontage> Fire_Aimed_FPP = nullptr;



	// Third-person.

// To be added.



// Deprecated.
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Weapon", DisplayName = "Idle, Aiming (First-Person)", meta=(DeprecatedProperty))
	UAnimSequenceBase* AimedIdleAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Locomotion", DisplayName = "Walking, Aiming (First-Person)", meta=(DeprecatedProperty))
	UBlendSpace* WalkingAimedBS_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Jump", DisplayName = "Jump, Aiming (First-Person)", meta=(DeprecatedProperty))
	UAnimSequenceBase* JumpAimedAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Jump", DisplayName = "Falling, Aiming (First-Person)", meta=(DeprecatedProperty))
	UAnimSequenceBase* FallingAimedAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Jump", DisplayName = "Landing, Aiming (First-Person)", meta=(DeprecatedProperty))
	UAnimSequenceBase* LandingAimedAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Weapon", DisplayName = "Start Aiming (First-Person)", meta=(DeprecatedProperty))
	UAnimMontage* StartAimAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Weapon", DisplayName = "End Aiming (First-Person)", meta=(DeprecatedProperty))
	UAnimMontage* EndAimAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Weapon", DisplayName = "Hip-Fire (First-Person)", meta=(DeprecatedProperty))
	UAnimMontage* HipFireAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Weapon", DisplayName = "Aimed Fire (First-Person)", meta=(DeprecatedProperty))
	UAnimMontage* AimedFireAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Weapon", DisplayName = "Reload from Empty Magazine (First-Person)", meta=(DeprecatedProperty))
	UAnimMontage* EmptyReloadAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Weapon", DisplayName = "Reload with a Chambered Round (First-Person)", meta=(DeprecatedProperty))
	UAnimMontage* ChamberedReloadAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Weapon", DisplayName = "Fire (Third-Person, Upper)", meta=(DeprecatedProperty))
	UAnimMontage* FireAnim_UpperTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Weapon", DisplayName = "Reload from Empty Magazine (Third-Person, Upper)", meta=(DeprecatedProperty))
	UAnimMontage* EmptyReloadAnim_UpperTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Weapon", DisplayName = "Reload with a Chambered Round (Third-Person, Upper)", meta=(DeprecatedProperty))
	UAnimMontage* ChamberedReloadAnim_UpperTPP = nullptr;
};
