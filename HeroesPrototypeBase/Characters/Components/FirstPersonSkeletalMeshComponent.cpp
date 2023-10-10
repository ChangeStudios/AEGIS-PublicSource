// Copyright Samuel Reitich 2023.


#include "Characters/Components/FirstPersonSkeletalMeshComponent.h"

FMatrix UFirstPersonSkeletalMeshComponent::GetRenderMatrix() const
{
	// The relative matrix of this component.
	const FMatrix OutModelMatrix = GetComponentTransform().ToMatrixWithScale();

	const APawn* Owner = GetOwner() ? GetOwner<APawn>() : nullptr;
	const APlayerController* PlayerController = Owner && Owner->GetController() ? Owner->GetController<APlayerController>() : nullptr;

	if (PlayerController)
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);

		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->Viewport)
		{
			FSceneViewFamily::ConstructionValues ViewFamilyValues =
				FSceneViewFamily::ConstructionValues
				(
					LocalPlayer->ViewportClient->Viewport,
					GetWorld()->Scene,
					LocalPlayer->ViewportClient->EngineShowFlags
				);
			ViewFamilyValues.SetRealtimeUpdate(true);
			FSceneViewFamilyContext ViewFamilyContext(ViewFamilyValues);

			FVector ViewLocation;
			FRotator ViewRotation;
			FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamilyContext, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);

			if (!SceneView)
			{
				return OutModelMatrix;
			}

			const float DesiredHalfFovRad = DesiredFOV * PI / 360.0f;

			const FIntPoint ViewportSize = LocalPlayer->ViewportClient->Viewport->GetSizeXY();
			const float Width = ViewportSize.X;
			const float Height = ViewportSize.Y;
			const float TargetWidth = 1920.0f;
			const float TargetHeight = 1080.0f;
			const float NearClippingPlaneDistance = SceneView->NearClippingDistance;

			
			const FMatrix NewProjectionMatrix = FReversedZPerspectiveMatrix(DesiredHalfFovRad, Width, Height, NearClippingPlaneDistance);
			const FMatrix ViewMatrix = SceneView->ViewMatrices.GetViewMatrix();
			const FMatrix InverseViewMatrix = SceneView->ViewMatrices.GetInvViewMatrix();
			const FMatrix ProjectionMatrix = SceneView->ViewMatrices.GetProjectionMatrix();
			const FMatrix InverseProjectionMatrix = SceneView->ViewMatrices.GetInvProjectionMatrix();

			const FMatrix NewViewProjectionMatrix = ViewMatrix * NewProjectionMatrix;
			const FMatrix InverseOldViewProjectionMatrix = InverseProjectionMatrix * InverseViewMatrix;

			return OutModelMatrix * NewViewProjectionMatrix * InverseOldViewProjectionMatrix;
		}
	}

	return OutModelMatrix;
}
