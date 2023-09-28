// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "ViewModelSkeletalMeshComponent.generated.h"

/**
 * A skeletal mesh component that is effectively rendered separately from the world. This is used for first-person
 * meshes that need to be rendered consistently, regardless of any factors that affect players' view (FOV, aspect ratio,
 * etc.).
 */
UCLASS(Blueprintable, Blueprintable, meta = (BlueprintSpawnableComponent))
class HEROESPROTOTYPEBASE_API UViewModelSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:

	/** The FOV we want to render this component at. */
	UPROPERTY(EditAnywhere, Category = Rendering)
	float DesiredViewmodelFOV = 90.0f;



protected:

	virtual void BeginPlay() override;

	/** Adjusts the component's model matrix to render it with a different field of view from the world. */
	virtual FMatrix GetRenderMatrix() const override;



	// Cached for convenience.

private:

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	APlayerCameraManager* PlayerCameraManager;

};
