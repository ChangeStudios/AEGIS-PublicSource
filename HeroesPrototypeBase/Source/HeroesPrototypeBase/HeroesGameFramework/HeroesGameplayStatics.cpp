// Copyright Samuel Reitich 2023.


#include "HeroesGameFramework/HeroesGameplayStatics.h"

bool UHeroesGameplayStatics::CanReachTarget(UWorld* InWorld, FVector StartVector, AActor* Target, TArray<AActor*> ActorsToIgnore)
{
	FCollisionQueryParams CollisionParams = FCollisionQueryParams();
	CollisionParams.bTraceComplex = true;
	CollisionParams.AddIgnoredActors(ActorsToIgnore);

	// If the target is a pawn, test line-of-sight to the bottom, middle, and top of their collision cylinder.
	if (APawn* TargetPawn = Cast<APawn>(Target))
	{
		bool bBottom = false, bMiddle = false, bTop = false;

		float TargetRadius, TargetHalfHeight;
		TargetPawn->GetSimpleCollisionCylinder(TargetRadius, TargetHalfHeight);
		
		const FVector BottomLocation = TargetPawn->GetActorLocation() - FVector(0.0f, 0.0f, TargetHalfHeight);
		bBottom = !InWorld->LineTraceTestByChannel(StartVector, BottomLocation, ECC_Visibility, CollisionParams);
		
		const FVector MiddleLocation = TargetPawn->GetActorLocation();
		bMiddle = !InWorld->LineTraceTestByChannel(StartVector, MiddleLocation, ECC_Visibility, CollisionParams);
		
		const FVector TopLocation = TargetPawn->GetActorLocation() + FVector(0.0f, 0.0f, TargetHalfHeight);
		bTop = !InWorld->LineTraceTestByChannel(StartVector, TopLocation, ECC_Visibility, CollisionParams);

		// If the vector has line-of-sight to the bottom and middle of the target or the top and middle of the target, it will reach the target.
		return (bBottom && bMiddle || bTop && bMiddle);
	}
	// If the target is not a pawn, do a simple trace to the center of the target to check if the vector will reach it.
	else
	{
		return !InWorld->LineTraceTestByChannel(StartVector, Target->GetActorLocation(), ECC_Visibility, CollisionParams);
	}
}
