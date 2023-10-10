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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Start Aim-Down-Sights (First-Person)")
	UAnimMontage* StartAimAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "End Aim-Down-Sights (First-Person)")
	UAnimMontage* EndAimAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Weapon", DisplayName = "Aimed Idle (First-Person)")
	UAnimSequenceBase* AimedIdleAnim_FPP = nullptr;

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
