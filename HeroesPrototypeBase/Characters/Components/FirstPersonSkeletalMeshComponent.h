// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "FirstPersonSkeletalMeshComponent.generated.h"

/**
 * A skeletal mesh component that is rendered separately from the world. This is used for first-person meshes that
 * always need to be rendered consistently, regardless of any factors that affect players' view (FOV, aspect ratio,
 * etc.).
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class HEROESPROTOTYPEBASE_API UFirstPersonSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:

	/** Overrides the default view projection to render this mesh separately from the rest of the world. This is used
	 * for to allow dynamic FOV without affecting our first-person animations. */
	virtual FMatrix GetRenderMatrix() const override;

	/** The FOV we want to render this component at. */
	UPROPERTY(EditDefaultsOnly, Category = Rendering)
	float DesiredFOV = 90.0f;

};
