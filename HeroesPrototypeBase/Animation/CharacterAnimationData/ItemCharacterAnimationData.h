// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Animation/HeroesAnimationTypes.h"
#include "Animation/FloatSpringInterpDataAsset.h"
#include "Animation/CharacterAnimationData/CharacterAnimationData.h"
#include "ItemCharacterAnimationData.generated.h"

class UAnimMontage;
class UAnimSequenceBase;
class UBlendSpace1D;

/**
 * Collection of animation data specific to an item, used to perform item-specific animation on the character
 * currently using the item. When a character equips this item, it will switch to this animation set.
 *
 * This data class extends UCharacterAnimationData with additional functionality exclusive to items, such as inspection
 * animations.
 */
UCLASS(BlueprintType)
class HEROESPROTOTYPEBASE_API UItemCharacterAnimationData : public UCharacterAnimationData
{
	GENERATED_BODY()

// Item.
public:

	/** Equip item animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Item", DisplayName = "Equip Item (First-Person)")
	TObjectPtr<UAnimMontage> EquipAnim_FPP = nullptr;

	/** Inspect item animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Item", DisplayName = "Inspect Item (First-Person)")
	TObjectPtr<UAnimMontage> InspectAnim_FPP = nullptr;

	/** One-off fidgeting animations that play when a player has idled for a certain amount of time. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "First-Person|Item", DisplayName = "Fidgets (First-Person)")
	TArray<TObjectPtr<UAnimMontage>> Fidgets_FPP = TArray<TObjectPtr<UAnimMontage>>();



	// Third-person.

// To be added.



// Deprecated.
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Item|Spring Interpolation Data", DisplayName = "Move Forward/Backward Spring Interpolation Data", meta=(DeprecatedProperty))
	UFloatSpringInterpDataAsset* SpringInterpDataMoveForwardBackward = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Item|Spring Interpolation Data", DisplayName = "Move Right/Left Spring Interpolation Data", meta=(DeprecatedProperty))
	UFloatSpringInterpDataAsset* SpringInterpDataMoveRightLeft = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Item|Spring Interpolation Data", DisplayName = "Aim Up/Down Spring Interpolation Data", meta=(DeprecatedProperty))
	UFloatSpringInterpDataAsset* SpringInterpDataAimUpDown = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Item|Spring Interpolation Data", DisplayName = "Aim Right/Left Spring Interpolation Data", meta=(DeprecatedProperty))
	UFloatSpringInterpDataAsset* SpringInterpDataAimRightLeft = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Item|Spring Interpolation Data", meta=(DeprecatedProperty))
	FAdditiveAnimationMultipliers AdditiveAnimationMultipliers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Item|IK", meta=(DeprecatedProperty))
	FTransform RightHandPoseCorrectionOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Item|Falling", meta=(DeprecatedProperty))
	float MinFallDistance = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Item|Falling", meta=(DeprecatedProperty))
	float MaxFallDistance = 1200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Item|Firing", meta=(DeprecatedProperty))
	float FireInterpSpeed = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Item|Firing", meta=(DeprecatedProperty))
	float FireJitterBaseStrengthX = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|First-Person|Item|Firing", meta=(DeprecatedProperty))
	UCurveFloat* FireJitterInterpCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deprecated|Third-Person|Item", DisplayName = "Equip Item (Third-Person, Upper)", meta=(DeprecatedProperty))
	UAnimMontage* EquipAnim_UpperTPP = nullptr;
};
