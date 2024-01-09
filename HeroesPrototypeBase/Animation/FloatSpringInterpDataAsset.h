// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FloatSpringInterpDataAsset.generated.h"

/**
 * This class stores data used to perform float spring model calculations. This is a temporary class that will be used
 * until a complete "item" system is implemented; this data will then be moved to each item's static data.
 */
UCLASS(BlueprintType, Blueprintable)
class HEROESPROTOTYPEBASE_API UFloatSpringInterpDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Scalar used to scale the amplitude of the spring model and control the strength of its effect. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spring Model")
	float InterpSpeed = 50.0f;

	/** Represents the stiffness of this spring. Higher values reduce oscillation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spring Model")
	float Stiffness = 25.0f;

	/** The amount of damping applied to the spring. 0.0 means no damping (full oscillation), 1.0 means full damping
	 * (no oscillation). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spring Model")
	float CriticalDampingFactor = 0.5f;

	/** A multiplier that acts like mass on the spring, affecting the amount of force required to change its position. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spring Model")
	float Mass = 10.0f;

};
