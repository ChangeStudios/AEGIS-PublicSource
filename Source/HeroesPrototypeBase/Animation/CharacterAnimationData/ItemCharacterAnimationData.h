// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Animation/HeroesAnimationTypes.h"
#include "Animation/FloatSpringInterpDataAsset.h"
#include "Animation/CharacterAnimationData/CharacterAnimationData.h"
#include "ItemCharacterAnimationData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UItemCharacterAnimationData : public UCharacterAnimationData
{
	GENERATED_BODY()

	// First-person.

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Item", DisplayName = "Equip Item (First-Person)")
	UAnimMontage* EquipAnim_FPP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Item", DisplayName = "Inspect Item (First-Person)")
	UAnimMontage* InspectAnim_FPP = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Item|Spring Interpolation Data", DisplayName = "Move Forward/Backward Spring Interpolation Data")
	UFloatSpringInterpDataAsset* SpringInterpDataMoveForwardBackward = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Item|Spring Interpolation Data", DisplayName = "Move Right/Left Spring Interpolation Data")
	UFloatSpringInterpDataAsset* SpringInterpDataMoveRightLeft = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Item|Spring Interpolation Data", DisplayName = "Aim Up/Down Spring Interpolation Data")
	UFloatSpringInterpDataAsset* SpringInterpDataAimUpDown = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Item|Spring Interpolation Data", DisplayName = "Aim Right/Left Spring Interpolation Data")
	UFloatSpringInterpDataAsset* SpringInterpDataAimRightLeft = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Item|Spring Interpolation Data")
	FAdditiveAnimationMultipliers AdditiveAnimationMultipliers;



	// Third-Person.

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Third-Person|Item", DisplayName = "Equip Item (Third-Person, Upper)")
	UAnimMontage* EquipAnim_UpperTPP = nullptr;
};
