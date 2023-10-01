// Copyright Samuel Reitich 2023.


#include "Characters/Components/ViewModelSkeletalMeshComponent.h"

#include "Kismet/GameplayStatics.h"

void UViewModelSkeletalMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache the player controller and camera manager.
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (PlayerController != nullptr)
	{
		PlayerCameraManager = PlayerController->PlayerCameraManager;
	}

}

FMatrix UViewModelSkeletalMeshComponent::GetRenderMatrix() const
{
	// Don't override the render matrix if we haven't cached the player controller and camera manager yet.
	if (!IsValid(PlayerController) || !IsValid(PlayerCameraManager))
	{
		return Super::GetRenderMatrix();
	}

	// The FOV that the world is currently being rendered with and the FOV with which we want this component to be rendered.
	const float WorldFOV = PlayerCameraManager->GetFOVAngle();
	const float ViewmodelFOV = DesiredViewmodelFOV;

	// The player's camera's current viewpoint.
	FVector ViewOrigin;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewOrigin, ViewRotation);

	// The FOV ratio will be used to calculate the adjusted projection matrix, which uses HALF of the FOV angle and is calculated in radians.
	const float WorldHalfFOVRadians = FMath::DegreesToRadians(FMath::Max(0.001f, WorldFOV)) / 2.0f;
	const float DesiredHalfFOVRadians = FMath::DegreesToRadians(FMath::Max(0.001f, ViewmodelFOV)) / 2.0f;
	const float FOVRatio = WorldHalfFOVRadians / DesiredHalfFOVRadians;

	// Calculate the projection matrix adjusted to the desired FOV.
	const FMatrix AdjustedProjectionMatrix = FMatrix
	(
		FPlane(FOVRatio, 0, 0, 0),
		FPlane(0, FOVRatio, 0, 0),
		FPlane(0, 0, 1, 0),
		FPlane(0, 0, 0, 1)
	);



	// Get the inverse of the player's camera's view matrix to adjust the render matrix.
	FMatrix ViewRotationMatrix = FInverseRotationMatrix(ViewRotation) *
	FMatrix
	(
		FPlane(0, 0, 1, 0),
		FPlane(1, 0, 0, 0),
		FPlane(0, 1, 0, 0),
		FPlane(0, 0, 0, 1)
	);

	// Add the rotation matrix's translation to the view origin and remove it from the view rotation if it's not at the origin.
	if (!ViewRotationMatrix.GetOrigin().IsNearlyZero(0.0f))
	{
		ViewOrigin += ViewRotationMatrix.InverseTransformPosition(FVector::ZeroVector);
		ViewRotationMatrix = ViewRotationMatrix.RemoveTranslation();
	}

	// Calculate the view matrix and inverse view matrix that will be use to adjust the render matrix.
	const FMatrix ViewMatrix = FTranslationMatrix(-ViewOrigin) * ViewRotationMatrix;
	const FMatrix InverseViewMatrix = FTranslationMatrix(-ViewMatrix.GetOrigin()) * ViewMatrix.RemoveTranslation().GetTransposed();



	// Find the desired render matrix by adjusting the world's render matrix with our adjusted view matrices and projection matrix.
	const FMatrix AdjustedRenderMatrix = GetComponentToWorld().ToMatrixWithScale() * ViewMatrix * AdjustedProjectionMatrix * InverseViewMatrix;

	return AdjustedRenderMatrix;
}
