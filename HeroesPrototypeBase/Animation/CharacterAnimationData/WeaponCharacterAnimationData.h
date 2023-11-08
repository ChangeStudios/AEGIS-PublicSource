// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Animation/CharacterAnimationData/ItemCharacterAnimationData.h"
#include "WeaponCharacterAnimationData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UWeaponCharacterAnimationData : public UItemCharacterAnimationData
{
	GENERATED_BODY()

	// First-person.

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Idle, Aiming (First-Person)")
	UAnimSequenceBase* AimedIdleAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion", DisplayName = "Walking, Aiming (First-Person)")
	UBlendSpace* WalkingAimedBS_FPP = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Jump", DisplayName = "Jump, Aiming (First-Person)")
	UAnimSequenceBase* JumpAimedAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Jump", DisplayName = "Falling, Aiming (First-Person)")
	UAnimSequenceBase* FallingAimedAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Jump", DisplayName = "Landing, Aiming (First-Person)")
	UAnimSequenceBase* LandingAimedAnim_FPP = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Start Aiming (First-Person)")
	UAnimMontage* StartAimAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "End Aiming (First-Person)")
	UAnimMontage* EndAimAnim_FPP = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Hip-Fire (First-Person)")
	UAnimMontage* HipFireAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Aimed Fire (First-Person)")
	UAnimMontage* AimedFireAnim_FPP = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Reload from Empty Magazine (First-Person)")
	UAnimMontage* EmptyReloadAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Reload with a Chambered Round (First-Person)")
	UAnimMontage* ChamberedReloadAnim_FPP = nullptr;



	// Third-Person.

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Weapon", DisplayName = "Fire (Third-Person, Upper)")
	UAnimMontage* FireAnim_UpperTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Weapon", DisplayName = "Reload from Empty Magazine (Third-Person, Upper)")
	UAnimMontage* EmptyReloadAnim_UpperTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Weapon", DisplayName = "Reload with a Chambered Round (Third-Person, Upper)")
	UAnimMontage* ChamberedReloadAnim_UpperTPP = nullptr;
};
