// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "HeroesAnimationTypes.generated.h"

/**
 * A collection of transforms used to adjust the effect of additive animations.
 */
USTRUCT(BlueprintType)
struct FAdditiveAnimationMultipliers
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Right/Left")
	FVector HipTranslation_MoveRightLeft = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Right/Left")
	FRotator HipRotation_MoveRightLeft = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Right/Left")
	FVector HandTranslation_MoveRightLeft = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Right/Left")
	FRotator HandRotation_MoveRightLeft = FRotator::ZeroRotator;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Forward/Backward")
	FVector HipTranslation_MoveForwardBackward = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Forward/Backward")
	FRotator HipRotation_MoveForwardBackward = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Forward/Backward")
	FVector HandTranslation_MoveForwardBackward = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Forward/Backward")
	FRotator HandRotation_MoveForwardBackward = FRotator::ZeroRotator;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Up/Down")
	FVector HandTranslation_AimUpDown = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Up/Down")
	FRotator HandRotation_AimUpDown = FRotator::ZeroRotator;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Right/Left")
	FVector HandTranslation_AimRightLeft = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Right/Left")
	FRotator HandRotation_AimRightLeft = FRotator::ZeroRotator;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|NormalizedPitch")
	FVector HandTranslation_NormalizedAimPitch = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|NormalizedPitch")
	FRotator HandRotation_NormalizedAimPitch = FRotator::ZeroRotator;
};