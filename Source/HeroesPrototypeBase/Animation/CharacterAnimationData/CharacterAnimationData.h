// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterAnimationData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UCharacterAnimationData : public UDataAsset
{
	GENERATED_BODY()

	// First-person.

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion", DisplayName = "Idle (First-Person)")
	UAnimSequenceBase* IdleAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Locomotion", DisplayName = "Walking (First-Person)")
	UBlendSpace* WalkingBS_FPP = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Jump", DisplayName = "Jump (First-Person)")
	UAnimSequenceBase* JumpAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Jump", DisplayName = "Falling (First-Person)")
	UAnimSequenceBase* FallingAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Jump", DisplayName = "Landing (First-Person)")
	UAnimSequenceBase* LandingAnim_FPP = nullptr;



	// Third-Person.

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Locomotion", DisplayName = "Standing (Third-Person, Lower)")
	UAnimSequenceBase* StandingAnim_LowerTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Locomotion", DisplayName = "Idle (Third-Person, Upper)")
	UAnimSequenceBase* IdleAnim_UpperTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Locomotion", DisplayName = "Walking (Third-Person, Lower)")
	UBlendSpace* WalkingBS_TPP_LowerTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Locomotion", DisplayName = "Crouching (Third-Person, Lower)")
	UAnimSequenceBase* CrouchingAnim_LowerTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Locomotion", DisplayName = "Crouch-Walking (Third-Person, Lower)")
	UBlendSpace* CrouchWalkingBS_LowerTPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Locomotion", DisplayName = "Slow-Walking (Third-Person, Lower)")
	UBlendSpace* SlowWalkingBS_LowerTPP = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Jump", DisplayName = "Jump (Third-Person, Lower)")
	UAnimSequenceBase* JumpAnim_LowerTPP = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Jump", DisplayName = "Falling (Third-Person, Lower)")
	UAnimSequenceBase* FallingAnim_LowerTPP = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Jump", DisplayName = "Landing (Third-Person, Lower)")
	UAnimSequenceBase* LandingAnim_LowerTPP = nullptr;
};
